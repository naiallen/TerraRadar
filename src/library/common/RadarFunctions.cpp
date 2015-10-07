/*  Copyright (C) 2015 National Institute For Space Research (INPE) - Brazil.

This file is part of the TerraRadar - a library and application for radar data manipulation.

TerraRadar is under development.
*/

/*!
\file terraradar/library/common/RadarFunctions.cpp
\brief Useful functions for radar data handling.
*/

// TerraRadar Includes
#include "RadarFunctions.hpp"

// TerraLib Includes
#include <terralib/plugin.h>
#include <terralib/common/PlatformUtils.h>

// STL Includes
#include <string>

namespace teradar {
  namespace common {
    bool CreateIntensityRaster( const te::rst::Raster* inputRasterPtr,
      const std::map<std::string, std::string>& intensityRasterInfo,
      const std::string& outputDataSourceType,
      std::auto_ptr<te::rst::Raster>& intensityRasterPtr,
      const bool enableProgressInterface ) {

      intensityRasterPtr.reset();

      size_t inputRasterBandsSize = inputRasterPtr->getNumberOfBands();

      // only 3 and 4 bands are supported.
      if( inputRasterBandsSize != 9 && inputRasterBandsSize != 16 ) {
        return false;
      }

      if( outputDataSourceType.empty() ) {
        return false;
      }

      // creating the output raster
      {
        // @todo - etore - what to do when grid is missing in the input file?
        std::auto_ptr<te::rst::Grid>
          outputGridPtr( new te::rst::Grid( *inputRasterPtr->getGrid() ));

        std::vector< te::rst::BandProperty* > bandsProperties;

        te::rst::BandProperty*
          inputBandProperty( new te::rst::BandProperty( *( inputRasterPtr->getBand( 0 )->getProperty() )));

        te::rst::BandProperty* bandProperty
          ( new te::rst::BandProperty( *inputBandProperty ));

        bandProperty->m_colorInterp = te::rst::GrayIdxCInt;

        bandsProperties.push_back( bandProperty );

        intensityRasterPtr.reset( te::rst::RasterFactory::make( outputDataSourceType,
          outputGridPtr.release(),
          bandsProperties,
          intensityRasterInfo,
          0, 0 ) );

        if( intensityRasterPtr.get() == 0 ) {
          return false;
        }
      }

      // create data
      {
        const unsigned int nOutRows = intensityRasterPtr->getNumberOfRows();
        const unsigned int nOutCols = intensityRasterPtr->getNumberOfColumns();

        // get bands for extract input values
        std::vector< size_t > intensityBands;
        std::complex< double > value = 0.;

        if( inputRasterBandsSize == 9 ) {
          intensityBands.push_back( 0 );
          intensityBands.push_back( 4 );
          intensityBands.push_back( 8 );

        } else {
          intensityBands.push_back( 0 );
          intensityBands.push_back( 5 );
          intensityBands.push_back( 10 );
          intensityBands.push_back( 15 );
        }

        te::rst::Band& outBand = *intensityRasterPtr->getBand( 0 );

        // for each pixel (line,column), compute the values
        for( unsigned int j = 0; j < nOutRows; ++j ) {
          for( unsigned int k = 0; k < nOutCols; ++k ) {
            std::complex< double > resultValue = 1.;

            for( size_t b = 0; b < intensityBands.size(); ++b ) {
              inputRasterPtr->getBand( intensityBands[b] )->getValue( k, j, value );
              resultValue *= value;
            }

            outBand.setValue( k, j, resultValue );
          }
        }
      }

      return true;
    }

    bool CreateCovarianceRaster( const std::vector<te::rst::Raster*>& inputRasterPointers,
      const std::vector<unsigned int>& inputRasterBands,
      const std::map<std::string, std::string>& outputRasterInfo,
      const std::string& outputDataSourceType,
      std::auto_ptr<te::rst::Raster>& outputRasterPtr,
      const bool enableProgressInterface )
    {
      outputRasterPtr.reset();

      size_t inputRasterBandsSize = inputRasterBands.size();

      if( inputRasterBandsSize != inputRasterPointers.size() ) {
        return false;
      }

      if( outputDataSourceType.empty() ) {
        return false;
      }

      size_t outputBands = inputRasterBandsSize * inputRasterBandsSize;

      // @todo - etore - rewrite it
      if( inputRasterBandsSize == 6 ) {
        outputBands = 9;
      } else if( inputRasterBandsSize == 10 ) {
        outputBands = 16;
      }

      // creating the output raster
      {
        te::rst::Raster const* inputRasterPtr = inputRasterPointers[0];

        // @todo - etore - what to do when grid is missing in the input file?
        std::auto_ptr<te::rst::Grid>
          outputGridPtr( new te::rst::Grid( *inputRasterPtr->getGrid() ) );

        // validate input data
        // for now, use the dimension of first raster as default
        // @todo - etore - Should we check if input dimensions are equal?
        for( size_t i = 0; i < inputRasterBandsSize; ++i ) {
          inputRasterPtr = inputRasterPointers[i];

          if( inputRasterBands[i] >= inputRasterPtr->getNumberOfBands() ) {
            return false;
          }
        }

        // use the properties from the first band / first raster to create
        // the output bands
        inputRasterPtr = inputRasterPointers[0];
        te::rst::BandProperty*
          inputBandProperty( new te::rst::BandProperty( *(inputRasterPtr->getBand
          ( 0 )->getProperty()) ) );

        std::vector< te::rst::BandProperty* > bandsProperties;

        for( size_t i = 0; i < outputBands; ++i ) {
          te::rst::BandProperty* bandProperty
            ( new te::rst::BandProperty( *inputBandProperty ) );

          // @todo - etore - check this. It sounds weird
          // if we don't do this, the warn is shown
          if( i != 0 ) {
            bandProperty->m_colorInterp = te::rst::UndefCInt;
          }
          else {
            bandProperty->m_colorInterp = te::rst::GrayIdxCInt;
          }
          bandsProperties.push_back( bandProperty );
        }

        outputRasterPtr.reset( te::rst::RasterFactory::make( outputDataSourceType,
          outputGridPtr.release(),
          bandsProperties, outputRasterInfo, 0, 0 ) );
        if( outputRasterPtr.get() == 0 ) {
          return false;
        }
      }

      // create data for each band
      const unsigned int nOutRows = outputRasterPtr->getNumberOfRows( );
      const unsigned int nOutCols = outputRasterPtr->getNumberOfColumns( );

      if( inputRasterBandsSize == 6 ) {
        // INPUT:     OUTPUT: (minus signal means conjugated complex)
        // 0  1  2     0  1  2
        //    3  4    -1  3  4
        //       5    -2 -4  5

        std::vector< std::complex< double > > inputComplex( inputRasterBandsSize );

        // for each pixel (line,column), compute the values
        for( unsigned int j = 0; j < nOutRows; ++j ) {
          for( unsigned int k = 0; k < nOutCols; ++k ) {
            // get the input values
            for( size_t i = 0; i < inputRasterPointers.size( ); ++i ) {
              std::complex< double > inputComplexValue;
              inputRasterPointers[i]->getBand( inputRasterBands[i] )->getValue( k, j, inputComplexValue );
              inputComplex[i] = inputComplexValue;
            }
            
            outputRasterPtr->getBand( 0 )->setValue( k, j, inputComplex[0] );
            outputRasterPtr->getBand( 1 )->setValue( k, j, inputComplex[1] );
            outputRasterPtr->getBand( 2 )->setValue( k, j, inputComplex[2] );
            outputRasterPtr->getBand( 3 )->setValue( k, j, std::conj( inputComplex[1] ) );
            outputRasterPtr->getBand( 4 )->setValue( k, j, inputComplex[3] );
            outputRasterPtr->getBand( 5 )->setValue( k, j, inputComplex[4] );
            outputRasterPtr->getBand( 6 )->setValue( k, j, std::conj( inputComplex[2] ) );
            outputRasterPtr->getBand( 7 )->setValue( k, j, std::conj( inputComplex[4] ) );
            outputRasterPtr->getBand( 8 )->setValue( k, j, inputComplex[5] );
          }
        }

      } else if( inputRasterBandsSize == 10 ) {
        // INPUT:       OUTPUT: (minus signal means conjugated complex)
        // 0  1  2  3    0  1  2  3
        //    4  5  6   -1  4  5  6
        //       7  8   -2 -5  7  8 
        //          9   -3 -6 -8  9 

        std::vector< std::complex< double > > inputComplex( inputRasterBandsSize );

        // for each pixel (line,column), compute the values
        for( unsigned int j = 0; j < nOutRows; ++j ) {
          for( unsigned int k = 0; k < nOutCols; ++k ) {
            // get the input values
            for( size_t i = 0; i < inputRasterPointers.size( ); ++i ) {
              std::complex< double > inputComplexValue;
              inputRasterPointers[i]->getBand( inputRasterBands[i] )->getValue( k, j, inputComplexValue );
              inputComplex[i] = inputComplexValue;
            }
            outputRasterPtr->getBand( 0 )->setValue( k, j, inputComplex[0] );
            outputRasterPtr->getBand( 1 )->setValue( k, j, inputComplex[1] );
            outputRasterPtr->getBand( 2 )->setValue( k, j, inputComplex[2] );
            outputRasterPtr->getBand( 3 )->setValue( k, j, inputComplex[3] );
            outputRasterPtr->getBand( 4 )->setValue( k, j, std::conj( inputComplex[1] ));
            outputRasterPtr->getBand( 5 )->setValue( k, j, inputComplex[4] );
            outputRasterPtr->getBand( 6 )->setValue( k, j, inputComplex[5] );
            outputRasterPtr->getBand( 7 )->setValue( k, j, inputComplex[6] );
            outputRasterPtr->getBand( 8 )->setValue( k, j, std::conj( inputComplex[2] ));
            outputRasterPtr->getBand( 9 )->setValue( k, j, std::conj( inputComplex[5] ));
            outputRasterPtr->getBand( 10 )->setValue( k, j, inputComplex[7] );
            outputRasterPtr->getBand( 11 )->setValue( k, j, inputComplex[8] );
            outputRasterPtr->getBand( 12 )->setValue( k, j, std::conj( inputComplex[3] ));
            outputRasterPtr->getBand( 13 )->setValue( k, j, std::conj( inputComplex[6] ));
            outputRasterPtr->getBand( 14 )->setValue( k, j, std::conj( inputComplex[8] ));
            outputRasterPtr->getBand( 15 )->setValue( k, j, inputComplex[9] );
          }
        }

      } else {
        std::vector< std::complex< double > > inputComplex( inputRasterBandsSize );
        std::vector< std::complex< double > > conjugatedComplex( inputRasterBandsSize );

        // for each pixel (line,column), compute the values
        for( unsigned int j = 0; j < nOutRows; ++j ) {
          for( unsigned int k = 0; k < nOutCols; ++k ) {
            // get the input values
            for( size_t i = 0; i < inputRasterPointers.size(); ++i ) {
              std::complex< double > inputComplexValue;
              inputRasterPointers[i]->getBand( inputRasterBands[i] )->getValue( k, j, inputComplexValue );
              inputComplex[i] = inputComplexValue;
              conjugatedComplex[i] = std::conj( inputComplexValue );
            }

            for( size_t l = 0; l < inputComplex.size(); ++l ) {
              for( size_t m = 0; m < conjugatedComplex.size(); ++m ) {
                size_t pos = (inputRasterBandsSize * l) + m;

                std::complex<double> value = inputComplex[l] * conjugatedComplex[m];
                outputRasterPtr->getBand( pos )->setValue( k, j, value );
              }
            }
          }
        }
      }

      return true;
    }

    bool ComputeCovarianceAndPearsonCorrelation( const te::rst::Raster* inputRaster1Ptr,
      unsigned int inputRaster1Band, const te::rst::Raster* inputRaster2Ptr, unsigned int inputRaster2Band,
      double& covariance, double& correlation, const bool enableProgressInterface )
    {
      const unsigned int nRows = inputRaster1Ptr->getNumberOfRows();
      const unsigned int nCols = inputRaster2Ptr->getNumberOfColumns();

      if( nRows != inputRaster2Ptr->getNumberOfRows() ) {
        return false;
      }

      if( nCols != inputRaster2Ptr->getNumberOfColumns() ) {
        return false;
      }

      return ComputeCovarianceAndPearsonCorrelation( inputRaster1Ptr, inputRaster1Band, 0, nCols, 0, nRows,
        inputRaster2Ptr, inputRaster2Band, 0, nCols, 0, nRows, covariance, correlation, enableProgressInterface );
    }

    bool ComputeCovarianceAndPearsonCorrelation( const te::rst::Raster* raster1Ptr, unsigned int raster1Band,
      unsigned int raster1XStart, unsigned int raster1XBound, unsigned int raster1YStart, unsigned int raster1YBound,
      const te::rst::Raster* raster2Ptr, unsigned int raster2Band, unsigned int raster2XStart, unsigned int raster2XBound,
      unsigned int raster2YStart, unsigned int raster2YBound,
      double& covariance, double& correlation, const bool enableProgressInterface ) {

      const unsigned int raster1Rows = raster1YBound - raster1YStart;
      const unsigned int raster1Cols = raster1XBound - raster1XStart;

      const unsigned int raster2Rows = raster2YBound - raster2YStart;
      const unsigned int raster2Cols = raster2XBound - raster2XStart;

      if(( raster1Cols != raster2Cols ) || ( raster1Rows != raster2Rows )) {
        return false;
      }

      std::complex< double > complex1Value;
      std::complex< double > complex2Value;

      std::complex<double> sum1 = 0.;
      std::complex<double> sum2 = 0.;

      for( unsigned int l = 0; l < raster1Rows; ++l ) {
        for( unsigned int c = 0; c < raster1Cols; ++c ) {
          raster1Ptr->getBand( raster1Band )->getValue( c + raster1XStart, l + raster1YStart, complex1Value );
          sum1 += complex1Value;

          raster2Ptr->getBand( raster2Band )->getValue( c + raster2XStart, l + raster2YStart, complex2Value );
          sum2 += complex2Value;
        }
      }

      unsigned int nElements = raster1Cols * raster1Rows;

      std::complex<double> mean1 = sum1 / (std::complex<double>)nElements;
      std::complex<double> mean2 = sum2 / (std::complex<double>)nElements;

      double diff11Sum = 0.;
      double diff22Sum = 0.;
      double diff12Sum = 0.;
      std::complex<double> diff1 = 0.;
      std::complex<double> diff2 = 0.;

      for( unsigned int l = 0; l < raster1Rows; ++l ) {
        for( unsigned int c = 0; c < raster1Cols; ++c ) {
          raster1Ptr->getBand( raster1Band )->getValue( c + raster1XStart, l + raster1YStart, complex1Value );
          diff1 = complex1Value - mean1;
          diff11Sum += std::real( diff1 * diff1 );

          raster2Ptr->getBand( raster2Band )->getValue( c + raster2XStart, l + raster2YStart, complex2Value );
          diff2 = complex2Value - mean2;
          diff22Sum += std::real( diff2 * diff2 );

          diff12Sum += std::real( diff1 * diff2 );
        }
      }

      double var1 = diff11Sum / (double)(nElements - 1);
      double var2 = diff22Sum / (double)(nElements - 1);

      double stdDev1 = sqrt( var1 );
      double stdDev2 = sqrt( var2 );

      covariance = diff12Sum / (double)(nElements - 1);
      correlation = covariance / (stdDev1 * stdDev2);

      return true;
    }

    unsigned int ComputeMinCompressionLevel( unsigned int maxLevel, const double& imageENL,
      const double& minENL, const double& autoCorrelation1, const double& autoCorrelation2,
      const double& autoCorrelation3 ) {
      unsigned int minLevel = 0;
      double levelENL = 0;

      while( (levelENL < minENL) && (minLevel != maxLevel) ) {
        minLevel++;

        double p1 = pow( imageENL * 2, 2 * minLevel );
        double p2 = 1 - (1 / pow( 2, minLevel ));

        levelENL = p1 / (1 + 2 * p2 * (autoCorrelation1 + autoCorrelation2 + (autoCorrelation3*p2)));
      }

      return minLevel;
    }

    std::pair<unsigned int, double> ComputeMinCompLevelENL( const teradar::common::RadarDataType& dataType,
      unsigned int numberOfBands, unsigned int maxLevel, const double& imageENL ) {
      double minENL = 0.;
      unsigned int minLevel = 0;
      
      if( dataType == ScatteringVectorT ) {
        if( numberOfBands == 1 ) {
          minENL = 1.0;
          minLevel = 0;
        }
        
        // @todo - etore - Ok. I know that it sounds weird, but the original code has it. Must verify why.
        // if( numberOfBands > 1 && numberOfBands <= 4 ) {
        if( numberOfBands >= 1 && numberOfBands <= 4 ) {
          switch( numberOfBands ) {
          case 1:
            minENL = 1.1;
            break;
          case 2:
            minENL = 2.2;
            break;
          case 3:
            minENL = 4.55;
            break;
          case 4:
            minENL = 7.65;
            break;
          default:
            // @todo - etore - log and throw
            break;
          }

          // If the minENL is bigger than the input's image ENL, compute the compression number to allow 
          // the hypothesis tests
          if( imageENL <= minENL ) {
            minLevel = ComputeMinCompressionLevel( maxLevel, imageENL, minENL );
          }          
        }
        
      } else if( dataType == CovarianceMatrixT ) {
        // This test assumes that the covariance matrix contains all bands (the conjugated too)
        // @todo - etore - verify if we should use only incomplete matrices, for performance purposes
        // 1:1 / 4:3 / 9:6 / 16:10

        if( numberOfBands == 1 ) {
          minENL = 1.0;
          minLevel = 0;
        } else if( numberOfBands > 1 && numberOfBands <= 16 ) {
          switch( numberOfBands ) {
          case 4:
            minENL = 2.2;
            break;
          case 9:
            minENL = 4.55;
            break;
          case 16:
            minENL = 7.65;
            break;
          default:
            // @todo - etore - log and throw
            break;
          }

          // If the minENL is bigger than the input's image ENL, compute the compression number to allow 
          // the hypothesis tests
          if( imageENL <= minENL ) {
            minLevel = ComputeMinCompressionLevel( maxLevel, imageENL, minENL );
          }

        } else {
          // @todo - etore - log and throw
        }
      } else {
        // @todo - etore - log and throw
      }

      return std::pair<unsigned int, double>( minLevel, minENL );
    }
  }
}
