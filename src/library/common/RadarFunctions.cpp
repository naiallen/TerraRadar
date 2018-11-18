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
#include "MatrixUtilsComplex.hpp"

// TerraLib Includes
//#include <terralib/plugin.h>
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

    bool CreateCovarianceRaster( const std::vector<te::rst::Raster*>& CovInputRasterPtr,
      const std::vector<unsigned int>& CovInputRasterBands,
      const std::map<std::string, std::string>& CovOutputRasterInfo,
      const std::string& CovOutputDataSourceType,
      std::auto_ptr<te::rst::Raster>& CovOutputRasterPtr,
      const bool enableProgressInterface )
    {

		//CovInputRasterPtrs  -> vector of raster pointer with the images organaized in bands
		//CovInputRasterBands -> vector that has the number of bands, which can be 4 (HH,HV,VH,VV) or 3 (HH,VV,HV), 6 (Matriz de covariancia monoestica incompleta) or 10 (Matriz de cvariancia biestatica incompleta)
		//CovOutputRasterInfo -> path 
		//CovOutputDataSourceType ->  ??????
		//CovOutputRasterPtr  -> output raster pinter

		CovOutputRasterPtr.reset();  //Destructs the object pointed by the auto_ptr object and deallocates its memory
		
		size_t inputRasterBandsSize = CovInputRasterBands.size(); //get the number of bands are in the inputRaster
		

		//Check if the number of bands has the same size of the input vector
		if (inputRasterBandsSize != CovInputRasterPtr.size())
			return false;
		
		
		if( CovOutputDataSourceType.empty() ) 
			return false;
		
		size_t outputBands = inputRasterBandsSize * inputRasterBandsSize;  //size of matrix

		// @todo - etore - rewrite it
		if( inputRasterBandsSize == 6 ) 
			outputBands = 9;
		else if( inputRasterBandsSize == 10 ) 
			outputBands = 16;

		// creating the output raster
		{
			te::rst::Raster const* inputRasterPtr = CovInputRasterPtr[0];  //recive the GRID, PATH and ACCESS TYPE from the first band of te inpt raster

			// @todo - etore - what to do when grid is missing in the input file?
			// Grid -> the spatial support for raster data
			std::auto_ptr<te::rst::Grid>
				outputGridPtr( new te::rst::Grid( *inputRasterPtr->getGrid() ) ); //grid of output raster recives the input raster grid

			// validate input data
			// for now, use the dimension of first raster as default
			// @todo - etore - Should we check if input dimensions are equal?
			for( size_t i = 0; i < inputRasterBandsSize; ++i ) 
			{
				//@todo - Naiallen - It's necessary?
				inputRasterPtr = CovInputRasterPtr[i]; //receives the address of each input band

				if (CovInputRasterBands[i] >= inputRasterPtr->getNumberOfBands()) 
					return false;
			}

			// use the properties from the first band of the input raster to create the output raster
			inputRasterPtr = CovInputRasterPtr[0];
			te::rst::BandProperty*
				inputBandProperty( new te::rst::BandProperty( *(inputRasterPtr->getBand(0)->getProperty()) ) );

			std::vector< te::rst::BandProperty* > bandsProperties;

			for( size_t i = 0; i < outputBands; ++i ) {
				te::rst::BandProperty* bandProperty(new te::rst::BandProperty( *inputBandProperty ));

				// @todo - etore - check this. It sounds weird
				// if we don't do this, the warn is shown
				bandProperty->m_colorInterp = (i != 0) ? te::rst::UndefCInt : te::rst::GrayIdxCInt;

				/*if( i != 0 ) {
					bandProperty->m_colorInterp = te::rst::UndefCInt;
				}
				else {
					bandProperty->m_colorInterp = te::rst::GrayIdxCInt;
				}*/
          
				bandsProperties.push_back( bandProperty );
			}

			CovOutputRasterPtr.reset( te::rst::RasterFactory::make( CovOutputDataSourceType, 
				outputGridPtr.release(), bandsProperties, CovOutputRasterInfo, 0, 0 ) );
		
			if (CovOutputRasterPtr.get() == 0)
				return false;
		}

		// create data for each band
		const unsigned int nOutRows = CovOutputRasterPtr->getNumberOfRows();
		const unsigned int nOutCols = CovOutputRasterPtr->getNumberOfColumns();

		std::vector< std::complex< double > > inputComplex(inputRasterBandsSize);

		if( inputRasterBandsSize == 6 ) {
			// INPUT:     OUTPUT: (minus signal means conjugated complex)
			// 0  1  2     0  1  2
		    //    3  4    -1  3  4
			//       5    -2 -4  5
			
			// for each pixel (line,column), compute the values
			for (unsigned int j = 0; j < nOutRows-1; ++j) {
				for (unsigned int k = 0; k < nOutCols-1; ++k) {

					for (size_t i = 0; i < CovInputRasterPtr.size(); ++i)
						CovInputRasterPtr[i]->getBand(CovInputRasterBands[i])->getValue(k, j, inputComplex[i]);

					// get the input values
					CovOutputRasterPtr->getBand(0)->setValue(k, j, inputComplex[0]);
					CovOutputRasterPtr->getBand(1)->setValue(k, j, inputComplex[1]);
					CovOutputRasterPtr->getBand(2)->setValue(k, j, inputComplex[2]);
					CovOutputRasterPtr->getBand(3)->setValue(k, j, std::conj(inputComplex[1]));
					CovOutputRasterPtr->getBand(4)->setValue(k, j, inputComplex[3]);
					CovOutputRasterPtr->getBand(5)->setValue(k, j, inputComplex[4]);
					CovOutputRasterPtr->getBand(6)->setValue(k, j, std::conj(inputComplex[2]));
					CovOutputRasterPtr->getBand(7)->setValue(k, j, std::conj(inputComplex[4]));
					CovOutputRasterPtr->getBand(8)->setValue(k, j, inputComplex[5]);

				}//end for k 
			}//end for j
		} //end if inputRasterBandsSize == 6
		
		else if( inputRasterBandsSize == 10 ) {
			// INPUT:       OUTPUT: (minus signal means conjugated complex)
			// 0  1  2  3    0  1  2  3
			//    4  5  6   -1  4  5  6
			//       7  8   -2 -5  7  8 
			//          9   -3 -6 -8  9 
			
			// for each pixel (line,column), compute the values
			for (unsigned int j = 0; j < nOutRows-1; ++j) {
				for (unsigned int k = 0; k < nOutCols-1; ++k) {
					// get the input values

					for (size_t i = 0; i < CovInputRasterPtr.size(); ++i)
						CovInputRasterPtr[i]->getBand(CovInputRasterBands[i])->getValue(k, j, inputComplex[i]);

					CovOutputRasterPtr->getBand(0)->setValue(k, j, inputComplex[0]);
					CovOutputRasterPtr->getBand(1)->setValue(k, j, inputComplex[1]);
					CovOutputRasterPtr->getBand(2)->setValue(k, j, inputComplex[2]);
					CovOutputRasterPtr->getBand(3)->setValue(k, j, inputComplex[3]);
					CovOutputRasterPtr->getBand(4)->setValue(k, j, std::conj(inputComplex[1]));
					CovOutputRasterPtr->getBand(5)->setValue(k, j, inputComplex[4]);
					CovOutputRasterPtr->getBand(6)->setValue(k, j, inputComplex[5]);
					CovOutputRasterPtr->getBand(7)->setValue(k, j, inputComplex[6]);
					CovOutputRasterPtr->getBand(8)->setValue(k, j, std::conj(inputComplex[2]));
					CovOutputRasterPtr->getBand(9)->setValue(k, j, std::conj(inputComplex[5]));
					CovOutputRasterPtr->getBand(10)->setValue(k, j, inputComplex[7]);
					CovOutputRasterPtr->getBand(11)->setValue(k, j, inputComplex[8]);
					CovOutputRasterPtr->getBand(12)->setValue(k, j, std::conj(inputComplex[3]));
					CovOutputRasterPtr->getBand(13)->setValue(k, j, std::conj(inputComplex[6]));
					CovOutputRasterPtr->getBand(14)->setValue(k, j, std::conj(inputComplex[8]));
					CovOutputRasterPtr->getBand(15)->setValue(k, j, inputComplex[9]); 				
				}//end for k
			}//end for j
		} // end if inputRasterBandsSize == 10
	  
		else if (CovInputRasterPtr.size() == 3){
			// [C]:      
			//	   |Shh|²		√2.(Shh.Shv*)    (Shh.Svv*)
			// √2.(Shv.Shh*)        |Shv|²     √2.(Shv.Svv*)
			//  (Svv.Shh*)      √2.(Svv.Shv*)     |Svv|²

			double raiz = sqrt(2);

			// for each pixel (line,column), compute the values
			for (unsigned int j = 0; j < nOutRows-1; ++j) {
				for (unsigned int k = 0; k < nOutCols-1; ++k) {
					CovInputRasterPtr[0]->getBand(CovInputRasterBands[0])->getValue(k, j, inputComplex[0]); // Shh
					CovInputRasterPtr[1]->getBand(CovInputRasterBands[1])->getValue(k, j, inputComplex[1]); // Shv
					CovInputRasterPtr[2]->getBand(CovInputRasterBands[2])->getValue(k, j, inputComplex[2]); // Svv

					CovOutputRasterPtr->getBand(0)->setValue(k, j, std::abs(pow(inputComplex[0], 2)));					// |Shh|²
					CovOutputRasterPtr->getBand(1)->setValue(k, j, inputComplex[0] * std::conj(inputComplex[1])*raiz);  // √2.(Shh.Shv*)
					CovOutputRasterPtr->getBand(2)->setValue(k, j, inputComplex[0] * std::conj(inputComplex[2]));       // (Shh.Svv*)

					CovOutputRasterPtr->getBand(3)->setValue(k, j, inputComplex[1] * std::conj(inputComplex[0])*raiz);	// √2.(Shv.Shh*)  
					CovOutputRasterPtr->getBand(4)->setValue(k, j, std::abs(pow(inputComplex[1], 2)));					// |Shv|²
					CovOutputRasterPtr->getBand(5)->setValue(k, j, inputComplex[1] * std::conj(inputComplex[2])*raiz);  // √2.(Shv.Svv*)

					CovOutputRasterPtr->getBand(6)->setValue(k, j, inputComplex[2] * std::conj(inputComplex[0]));	    // (Svv.Shh*)
					CovOutputRasterPtr->getBand(7)->setValue(k, j, inputComplex[2] * std::conj(inputComplex[1])*raiz);  // √2.(Svv.Shv*)
					CovOutputRasterPtr->getBand(8)->setValue(k, j, std::abs(pow(inputComplex[2], 2)));					// |Svv|²
				}//end for k
			}// end for j
		}// end if inputRasterBandsSize == 3

		else if (CovInputRasterPtr.size() == 4){
			// [C]:      
			//	  |Shh|²		(Shh.Shv*)		(Shh.Svh*)		(Shh.Svv*)
			//  (Shv.Shh*)        |Shv|²		(Shv.Svh*)		(Shv.Svv*)
			//  (Svh.Shh*)		(Svh.Shv*)		  |Svh|²		(Svh.Svv*)
			//  (Svv.Shh*)		(Svv.Shv*)		(Svv.Svh*)		  |Svv|²

			double raiz = sqrt(2);

			// for each pixel (line,column), compute the values
			for (unsigned int j = 0; j < nOutRows-1; ++j) {
				for (unsigned int k = 0; k < nOutCols-1; ++k) {
					CovInputRasterPtr[0]->getBand(CovInputRasterBands[0])->getValue(k, j, inputComplex[0]); // Shh
					CovInputRasterPtr[1]->getBand(CovInputRasterBands[1])->getValue(k, j, inputComplex[1]); // Shv
					CovInputRasterPtr[2]->getBand(CovInputRasterBands[2])->getValue(k, j, inputComplex[2]); // Svh
					CovInputRasterPtr[3]->getBand(CovInputRasterBands[3])->getValue(k, j, inputComplex[3]); // Svv

					CovOutputRasterPtr->getBand(0)->setValue(k, j, std::abs(pow(inputComplex[0], 2)));				// |Shh|²
					CovOutputRasterPtr->getBand(1)->setValue(k, j, inputComplex[0] * std::conj(inputComplex[1]));	// (Shh.Shv*)	
					CovOutputRasterPtr->getBand(2)->setValue(k, j, inputComplex[0] * std::conj(inputComplex[2]));	// (Shh.Svh*)
					CovOutputRasterPtr->getBand(3)->setValue(k, j, inputComplex[0] * std::conj(inputComplex[3]));	// (Shh.Svv*)

					CovOutputRasterPtr->getBand(4)->setValue(k, j, inputComplex[1] * std::conj(inputComplex[0]));	// (Shv.Shh*)        
					CovOutputRasterPtr->getBand(5)->setValue(k, j, std::abs(pow(inputComplex[1], 2)));				// |Shv|²		
					CovOutputRasterPtr->getBand(6)->setValue(k, j, inputComplex[1] * std::conj(inputComplex[2]));	// (Shv.Svh*)		
					CovOutputRasterPtr->getBand(7)->setValue(k, j, inputComplex[1] * std::conj(inputComplex[3]));	// (Shv.Svv*)

					CovOutputRasterPtr->getBand(8)->setValue(k, j, inputComplex[2] * std::conj(inputComplex[0]));	// (Svh.Shh*)			
					CovOutputRasterPtr->getBand(9)->setValue(k, j, inputComplex[2] * std::conj(inputComplex[1]));	// (Svh.Shv*)    
					CovOutputRasterPtr->getBand(10)->setValue(k, j, std::abs(pow(inputComplex[2], 2)));				// |Svh|²		
					CovOutputRasterPtr->getBand(11)->setValue(k, j, inputComplex[2] * std::conj(inputComplex[3]));	// (Svh.Svv*)  

					CovOutputRasterPtr->getBand(12)->setValue(k, j, inputComplex[3] * std::conj(inputComplex[0]));	// (Svv.Shh*)			
					CovOutputRasterPtr->getBand(13)->setValue(k, j, inputComplex[3] * std::conj(inputComplex[1]));	// (Svv.Shv*)		
					CovOutputRasterPtr->getBand(14)->setValue(k, j, inputComplex[3] * std::conj(inputComplex[2]));	// (Svv.Svh*)		  
					CovOutputRasterPtr->getBand(15)->setValue(k, j, std::abs(pow(inputComplex[3], 2)));				// |Svv|²	 
				}// end for k
			}//end for j
		}// end if inputRasterBandsSize == 4
		else return false;

		return true;
	}// end CreateCovarianceRaster

	bool CreateCoherenceRaster(const std::vector<te::rst::Raster*>& CohInputRasterPtrs,
		const std::vector<unsigned int>& CohInputRasterBands,
		const std::map<std::string, std::string>& CohOutputRasterInfo,
		const std::string& CohOutputDataSourceType,
		std::auto_ptr<te::rst::Raster>& CohOutputRasterPtr,
		const bool enableProgressInterface)
	{
		//CohInputRasterPtrs -> vector of raster pointer with the images organaized in bands
		//CohInputRasterBands -> vector that has the number of bands, which can be 4 (HH,HV,VH,VV) or 3 (HH,VV,HV), 6 (Matriz de covariancia monoestica incompleta) or 10 (Matriz de cvariancia biestatica incompleta)
		//CohOutputRasterInfo -> path 
		//CohOutputDataSourceType -> 
		//CohOutputRasterPtr -> output raster pointer
		
		CohOutputRasterPtr.reset(); //Destructs the object pointed by the auto_ptr object and deallocates its memory

		size_t CohInputRasterBandsSize = CohInputRasterBands.size(); //get the number of bands are in the inputRaster

		//conferencias
		if (CohInputRasterBandsSize != CohInputRasterPtrs.size())
			return false;

		if (CohOutputDataSourceType.empty())
			return false;

		size_t CohOutputBands = CohInputRasterBandsSize * CohInputRasterBandsSize;

		if (CohInputRasterBandsSize == 6)
			CohOutputBands = 9;
		else if (CohInputRasterBandsSize == 10)
			CohOutputBands = 16;

		// creating the output raster
		{
			//CohInpuRastePointer recebe o endereço da banda 0 do CohInputRasterPtrs
			//CohInputRasterPtrs é um vetor de raster e nao um raster 
			te::rst::Raster const* CohInpuRastePointer = CohInputRasterPtrs[0];
			
			// @todo - etore - what to do when grid is missing in the input file?
			// Grid -> the spatial support for raster data
			std::auto_ptr<te::rst::Grid>
				CohOutputGridPtr( new te::rst::Grid(*CohInpuRastePointer->getGrid()));

			//@todo - Naiallen - It's necessary?
			for (size_t i = 0; i< CohInputRasterBandsSize; i++)
			{
				CohInpuRastePointer = CohInputRasterPtrs[i]; //recebe todas as bandas
				
				if (CohInputRasterBands[i] >= CohInpuRastePointer->getNumberOfBands())
					return false;
			}

			// use the properties from the first band of the input raster to create the output raster
			CohInpuRastePointer = CohInputRasterPtrs[0];
			te::rst::BandProperty* 
				CohInputBandProperty(new te::rst::BandProperty(*(CohInpuRastePointer->getBand(0)->getProperty())));

			std::vector< te::rst::BandProperty* > bandsProperties;

			for (size_t i = 0; i < CohOutputBands; ++i) {
				te::rst::BandProperty* bandProperty
					(new te::rst::BandProperty(*CohInputBandProperty));
				
				bandProperty->m_colorInterp = (i != 0) ? te::rst::UndefCInt : te::rst::GrayIdxCInt;

				bandsProperties.push_back(bandProperty);
			}

			CohOutputRasterPtr.reset(te::rst::RasterFactory::make(CohOutputDataSourceType,
				CohOutputGridPtr.release(),
				bandsProperties, CohOutputRasterInfo, 0, 0));

			if (CohOutputRasterPtr.get() == 0)
				return false;
		}

		const unsigned int nOutRows = CohOutputRasterPtr->getNumberOfRows();
		const unsigned int nOutCols = CohOutputRasterPtr->getNumberOfColumns();
		std::vector< std::complex< double > > inputComplex(CohInputRasterBandsSize);

		if (CohInputRasterBandsSize == 6) {
			// INPUT:     OUTPUT: (minus signal means conjugated complex)
			// 0  1  2     0  1  2
			//    3  4    -1  3  4
			//       5    -2 -4  5

			// for each pixel (line,column), compute the values
			for (unsigned int j = 0; j < nOutRows-1; ++j) {
				for (unsigned int k = 0; k < nOutCols-1; ++k) {
					// get the input values
					for (size_t i = 0; i < CohInputRasterPtrs.size(); ++i) 
						CohInputRasterPtrs[i]->getBand(CohInputRasterBands[i])->getValue(k, j, inputComplex[i]);

					CohOutputRasterPtr->getBand(0)->setValue(k, j, inputComplex[0]);
					CohOutputRasterPtr->getBand(1)->setValue(k, j, inputComplex[1]);
					CohOutputRasterPtr->getBand(2)->setValue(k, j, inputComplex[2]);
					CohOutputRasterPtr->getBand(3)->setValue(k, j, std::conj(inputComplex[1]));
					CohOutputRasterPtr->getBand(4)->setValue(k, j, inputComplex[3]);
					CohOutputRasterPtr->getBand(5)->setValue(k, j, inputComplex[4]);
					CohOutputRasterPtr->getBand(6)->setValue(k, j, std::conj(inputComplex[2]));
					CohOutputRasterPtr->getBand(7)->setValue(k, j, std::conj(inputComplex[4]));
					CohOutputRasterPtr->getBand(8)->setValue(k, j, inputComplex[5]);
				}//end for k
			}//end for j
		} //end if inputRasterBandsSize == 6

		else if (CohInputRasterBandsSize == 10) {
			// INPUT:       OUTPUT: (minus signal means conjugated complex)
			// 0  1  2  3    0  1  2  3
			//    4  5  6   -1  4  5  6
			//       7  8   -2 -5  7  8 
			//          9   -3 -6 -8  9 

			// for each pixel (line,column), compute the values
			for (unsigned int j = 0; j < nOutRows-1; ++j) {
				for (unsigned int k = 0; k < nOutCols-1; ++k) {
					// get the input values
					for (size_t i = 0; i < CohInputRasterPtrs.size(); ++i)
						CohInputRasterPtrs[i]->getBand(CohInputRasterBands[i])->getValue(k, j, inputComplex[i]);

					CohOutputRasterPtr->getBand(0)->setValue(k, j, inputComplex[0]);
					CohOutputRasterPtr->getBand(1)->setValue(k, j, inputComplex[1]);
					CohOutputRasterPtr->getBand(2)->setValue(k, j, inputComplex[2]);
					CohOutputRasterPtr->getBand(3)->setValue(k, j, inputComplex[3]);
					CohOutputRasterPtr->getBand(4)->setValue(k, j, std::conj(inputComplex[1]));
					CohOutputRasterPtr->getBand(5)->setValue(k, j, inputComplex[4]);
					CohOutputRasterPtr->getBand(6)->setValue(k, j, inputComplex[5]);
					CohOutputRasterPtr->getBand(7)->setValue(k, j, inputComplex[6]);
					CohOutputRasterPtr->getBand(8)->setValue(k, j, std::conj(inputComplex[2]));
					CohOutputRasterPtr->getBand(9)->setValue(k, j, std::conj(inputComplex[5]));
					CohOutputRasterPtr->getBand(10)->setValue(k, j, inputComplex[7]);
					CohOutputRasterPtr->getBand(11)->setValue(k, j, inputComplex[8]);
					CohOutputRasterPtr->getBand(12)->setValue(k, j, std::conj(inputComplex[3]));
					CohOutputRasterPtr->getBand(13)->setValue(k, j, std::conj(inputComplex[6]));
					CohOutputRasterPtr->getBand(14)->setValue(k, j, std::conj(inputComplex[8]));
					CohOutputRasterPtr->getBand(15)->setValue(k, j, inputComplex[9]);
				}//end for k
			}//end for j
		} // end if inputRasterBandsSize == 10

		else if (CohInputRasterBandsSize == 3){
			// [T]:      
			//	    |Shh+Svv|²			(Shh+Svv).(Shh-Svv)*		(Shh+Svv).2.Shv*
			// (Shh-Svv).(Shh+Svv)*			 |Shh-Svv|²			    (Shh-Svv).2.Shv*
			//   2.Shv.(Shh+Svv)*         2.Shv.(Shh-Svv)*			    4.|Shv|²	

			std::complex<double> k1 = 0.0, k2 = 0.0, k3 = 0.0;
			for (unsigned int j = 0; j < nOutRows-1; ++j) {
				for (unsigned int k = 0; k < nOutCols-1; ++k) {

					CohInputRasterPtrs[0]->getBand(CohInputRasterBands[0])->getValue(k, j, inputComplex[0]); // Shh
					CohInputRasterPtrs[1]->getBand(CohInputRasterBands[1])->getValue(k, j, inputComplex[1]); // Shv
					CohInputRasterPtrs[2]->getBand(CohInputRasterBands[2])->getValue(k, j, inputComplex[2]); // Svv
					
					k1 = inputComplex[0] + inputComplex[2]; //Shh + Svv
					k2 = inputComplex[0] - inputComplex[2]; //Shh - Svv
					k3 = 2.0 * inputComplex[1]; //2.Shv

					CohOutputRasterPtr->getBand(0)->setValue(k, j, std::abs(pow(k1, 2)));	// |Shh+Svv|²
					CohOutputRasterPtr->getBand(1)->setValue(k, j, k1 * std::conj(k2));		// (Shh+Svv).(Shh-Svv)*
					CohOutputRasterPtr->getBand(2)->setValue(k, j, k1 * std::conj(k3));		// (Shh+Svv).2.Shv*

					CohOutputRasterPtr->getBand(3)->setValue(k, j, k2 * std::conj(k1));		// (Shh-Svv).(Shh+Svv)*			 
					CohOutputRasterPtr->getBand(4)->setValue(k, j, std::abs(pow(k2, 2)));	// |Shh-Svv|²			    
					CohOutputRasterPtr->getBand(5)->setValue(k, j, k2 * std::conj(k3));		// (Shh-Svv).2.Shv*

					CohOutputRasterPtr->getBand(6)->setValue(k, j, k3 * std::conj(k1));	    // 2.Shv.(Shh+Svv)*        
					CohOutputRasterPtr->getBand(7)->setValue(k, j, k3 * std::conj(k2));		//  2.Shv.(Shh-Svv)*			    
					CohOutputRasterPtr->getBand(8)->setValue(k, j, std::abs(pow(k3, 2)));	// 4.|Shv|²
				}//end for k
			}//end for j
		}// end if inputRasterBandsSize == 3

		else if (CohInputRasterBandsSize == 4){
			// [T]:      
			//	    |Shh+Svv|²			(Shh+Svv).(Shh-Svv)*		(Shh+Svv).(Shv+Svh)*		-j(Shh+Svv).(Shv-Svh)*
			// (Shh-Svv).(Shh+Svv)*			 |Shh-Svv|²			    (Shh-Svv).(Shv+Svh)*		-j(Shh-Svv).(Shv-Svh)*
			// (Shv+Svh).(Shh+Svv)*     (Shv+Svh).(Shh-Svv)*			 |Shv+Svh|²				-j(Shv+Svh).(Shv-Svh)*
			// j(Shv-Svh).(Shh+Svv)*	j(Shv-Svh).(Shh-Svv)*		j(Shv-Svh).(Shv+Svh)*			   |Shv-Svh|²

			std::complex<double> k1 = 0.0, k2 = 0.0, k3 = 0.0, k4 = 0.0;
			for (unsigned int j = 0; j < nOutRows-1; ++j) {
				for (unsigned int k = 0; k < nOutCols-1; ++k) {

					CohInputRasterPtrs[0]->getBand(CohInputRasterBands[0])->getValue(k, j, inputComplex[0]); // Shh
					CohInputRasterPtrs[1]->getBand(CohInputRasterBands[1])->getValue(k, j, inputComplex[1]); // Shv
					CohInputRasterPtrs[2]->getBand(CohInputRasterBands[2])->getValue(k, j, inputComplex[2]); // Svh
					CohInputRasterPtrs[3]->getBand(CohInputRasterBands[3])->getValue(k, j, inputComplex[3]); // Svv

					k1 = inputComplex[0] + inputComplex[3]; //Shh + Svv
					k2 = inputComplex[0] - inputComplex[3]; //Shh - Svv
					k3 = inputComplex[1] + inputComplex[1]; //Shv + Svh
					k4 = -std::imag(inputComplex[1] + inputComplex[1]); //j(Shv-Svh)

					CohOutputRasterPtr->getBand(0)->setValue(k, j, 0.5 * std::abs(pow(k1, 2)));	// |Shh+Svv|²							
					CohOutputRasterPtr->getBand(1)->setValue(k, j, 0.5 * k1 * std::conj(k2));		// (Shh+Svv).(Shh-Svv)*
					CohOutputRasterPtr->getBand(2)->setValue(k, j, 0.5 * k1 * std::conj(k3));		// (Shh+Svv).(Shv+Svh)*
					CohOutputRasterPtr->getBand(3)->setValue(k, j, 0.5 * k1 * std::conj(k4));		// -j(Shh+Svv).(Shv-Svh)*

					CohOutputRasterPtr->getBand(4)->setValue(k, j, 0.5 * k2 * std::conj(k1));		// (Shh-Svv).(Shh+Svv)*			 			    	
					CohOutputRasterPtr->getBand(5)->setValue(k, j, 0.5 * std::abs(pow(k2, 2)));		// |Shh-Svv|²			    
					CohOutputRasterPtr->getBand(6)->setValue(k, j, 0.5 * k2 * std::conj(k3));		// (Shh-Svv).(Shv+Svh)*	
					CohOutputRasterPtr->getBand(7)->setValue(k, j, 0.5 * k2 * std::conj(k4));		// -j(Shh-Svv).(Shv-Svh)*

					CohOutputRasterPtr->getBand(8)->setValue(k, j, 0.5 * k3 * std::conj(k1));		// (Shv+Svh).(Shh+Svv)*     			 		
					CohOutputRasterPtr->getBand(9)->setValue(k, j, 0.5 * k3 * std::conj(k2));		// (Shv+Svh).(Shh-Svv)*
					CohOutputRasterPtr->getBand(10)->setValue(k, j, 0.5 * std::abs(pow(k3, 2)));	// |Shv+Svh|²			    
					CohOutputRasterPtr->getBand(11)->setValue(k, j, 0.5 * k3 * std::conj(k4));		// -j(Shv+Svh).(Shv-Svh)*

					CohOutputRasterPtr->getBand(12)->setValue(k, j, -0.5 * k4 * std::conj(k1));		// j(Shv-Svh).(Shh+Svv)*	
					CohOutputRasterPtr->getBand(13)->setValue(k, j, -0.5 * k4 * std::conj(k2));		// j(Shv-Svh).(Shh-Svv)*		
					CohOutputRasterPtr->getBand(14)->setValue(k, j, -0.5 * k4 * std::conj(k2));		// j(Shv-Svh).(Shv+Svh)*			   
					CohOutputRasterPtr->getBand(15)->setValue(k, j, -0.5 * std::abs(pow(k4, 2)));	// |Shv-Svh|²		 		
				}//end for k
			}//end for j
		}//// end if inputRasterBandsSize == 4
		else return false;

		return true;
	}//end CreateCoherenceRaster


	bool ChangeCohtoCov(const std::vector<te::rst::Raster*>& InputRasterPtrs,
		const std::vector<unsigned int>& InputRasterBands,
		const int t2c,
		const std::map<std::string, std::string>& OutputRasterInfo,
		const std::string& OutputDataSourceType,
		std::auto_ptr<te::rst::Raster>& OutputRasterPtr,
		const bool enableProgressInterface)
	{
		//CohInputRasterPtrs -> vector of raster pointer with the images organaized in bands
		//CohInputRasterBands -> vector that has the number of bands, which can be 4 (HH,HV,VH,VV) or 3 (HH,VV,HV), 6 (Matriz de covariancia monoestica incompleta) or 10 (Matriz de cvariancia biestatica incompleta)
		//CohOutputRasterInfo -> path 
		//CohOutputDataSourceType -> 
		//CohOutputRasterPtr -> output raster pointer

		OutputRasterPtr.reset();

		size_t InputRasterBandsSize = InputRasterBands.size(); //get the number of bands are in the inputRaster

		//conferencias
		if (InputRasterBandsSize != InputRasterPtrs.size())
			return false;

		if (OutputDataSourceType.empty())
			return false;

		size_t OutputBands = InputRasterBandsSize; //same number of bands

		// creating the output raster
		{
			te::rst::Raster const* InpuRastePointer = InputRasterPtrs[0];

			// Grid -> the spatial support for raster data
			std::auto_ptr<te::rst::Grid>
				OutputGridPtr(new te::rst::Grid(*InpuRastePointer->getGrid()));

			te::rst::BandProperty*
				InputBandProperty(new te::rst::BandProperty(*(InpuRastePointer->getBand
				(0)->getProperty())));

			std::vector< te::rst::BandProperty* > bandsProperties;

			//para cada banda do raster de saída, setar as propriedades
			for(size_t i = 0; i < OutputBands; ++i) {
				te::rst::BandProperty* bandProperty
					(new te::rst::BandProperty(*InputBandProperty));

				bandProperty->m_colorInterp = (i != 0) ? te::rst::UndefCInt : te::rst::GrayIdxCInt;

				bandsProperties.push_back(bandProperty);
			}

			OutputRasterPtr.reset(te::rst::RasterFactory::make(OutputDataSourceType,
				OutputGridPtr.release(),
				bandsProperties, OutputRasterInfo, 0, 0));

			if (OutputRasterPtr.get() == 0)
				return false;
		}

		const int msize = int(std::sqrt(InputRasterBandsSize));
		boost::numeric::ublas::matrix< std::complex<double> > A(msize, msize);
		boost::numeric::ublas::matrix< std::complex<double> > At(msize, msize);

		//std::vector< std::complex< double > > A(InputRasterBandsSize);
		//std::vector< std::complex< double > > At(InputRasterBandsSize);
		

		double r = 1 / (std::sqrt(2));
		if (InputRasterBandsSize == 9)
		{	
			// [A]:
			// 1/√2		1/√2		0
			//  0		 0			1
			// 1/√2		-1/√2		0

			/*A[0] = std::complex<double>(r, 0.0);
			A[1] = std::complex<double>(r, 0.0);
			A[2] = std::complex<double>(0.0, 0.0);

			A[3] = std::complex<double>(0.0, 0.0);
			A[4] = std::complex<double>(0.0, 0.0);
			A[5] = std::complex<double>(1.0, 0.0);

			A[6] = std::complex<double>(r, 0.0);
			A[7] = std::complex<double>(-r, 0.0);
			A[8] = std::complex<double>(0.0, 0.0);
			//---------------------------------------

			At[0] = std::complex<double>(r, 0.0);
			At[1] = std::complex<double>(0.0, 0.0);
			At[2] = std::complex<double>(r, 0.0);
			
			At[3] = std::complex<double>(r, 0.0);
			At[4] = std::complex<double>(0.0, 0.0);
			At[5] = std::complex<double>(-r, 0.0);

			At[6] = std::complex<double>(0.0, 0.0);
			At[7] = std::complex<double>(1, 0.0);
			At[8] = std::complex<double>(0.0, 0.0);*/

			

			A(0, 0) = std::complex<double>(r, 0.0);
			A(0, 1) = std::complex<double>(r, 0.0);
			A(0, 2) = std::complex<double>(0.0, 0.0);

			A(1, 0) = std::complex<double>(0.0, 0.0);
			A(1, 1) = std::complex<double>(0.0, 0.0);
			A(1, 2) = std::complex<double>(1.0, 0.0);

			A(2, 0) = std::complex<double>(r, 0.0);
			A(2, 1) = std::complex<double>(-r, 0.0);
			A(2, 2) = std::complex<double>(0.0, 0.0);

			At = boost::numeric::ublas::trans(A);

			//RAT format
			/*A(0, 0) = std::complex<double>(r, 0.0);
			A(0, 1) = std::complex<double>(r, 0.0);
			A(0, 2) = std::complex<double>(0.0, 0.0);

			A(1, 0) = std::complex<double>(r, 0.0);
			A(1, 1) = std::complex<double>(-r, 0.0);
			A(1, 2) = std::complex<double>(0.0, 0.0);

			A(2, 0) = std::complex<double>(0.0, 0.0);
			A(2, 1) = std::complex<double>(0.0, 0.0);
			A(2, 2) = std::complex<double>(1.0, 0.0);*/
		}//end if InputRasterBandsSize == 9

		else if (InputRasterBandsSize == 16)
		{
			// [A]:
			// 1/√2		1/√2		0		0
			// 1/√2		-1/√2		0		0		 
			//  0	      0			1		1
			//  0	      0			-j		j

			/*A[0] = std::complex<double>(r, 0.0);
			A[1] = std::complex<double>(r, 0.0);
			A[2] = std::complex<double>(0.0, 0.0);
			A[3] = std::complex<double>(0.0, 0.0);

			A[4] = std::complex<double>(r, 0.0);
			A[5] = std::complex<double>(-r, 0.0);
			A[6] = std::complex<double>(0.0, 0.0);
			A[7] = std::complex<double>(0.0, 0.0);

			A[8]= std::complex<double>(0.0, 0.0);
			A[9] = std::complex<double>(0.0, 0.0);
			A[10] = std::complex<double>(1.0, 0.0);
			A[11] = std::complex<double>(1.0, 0.0);

			A[12] = std::complex<double>(0.0, 0.0);
			A[13] = std::complex<double>(0.0, 0.0);
			A[14] = std::complex<double>(0.0, -1.0);
			A[15] = std::complex<double>(0.0, 1.0);
			//-----------------------------------------

			At[0] = std::complex<double>(r, 0.0);
			At[1] = std::complex<double>(r, 0.0);
			At[2] = std::complex<double>(0.0, 0.0);
			At[3] = std::complex<double>(0.0, 0.0);

			At[4] = std::complex<double>(r, 0.0);
			At[5] = std::complex<double>(-r, 0.0);
			At[6] = std::complex<double>(0.0, 0.0);
			At[7] = std::complex<double>(0.0, 0.0);

			At[8] = std::complex<double>(0.0, 0.0);
			At[9] = std::complex<double>(0.0, 0.0);
			At[10] = std::complex<double>(1.0, 0.0);
			At[11] = std::complex<double>(0.0, -1.0);

			At[12] = std::complex<double>(0.0, 0.0);
			At[13] = std::complex<double>(0.0, 0.0);
			At[14] = std::complex<double>(1.0, 0.0);
			At[15] = std::complex<double>(0.0, 1.0);*/

			A(0, 0) = std::complex<double>(r, 0.0);
			A(0, 1) = std::complex<double>(r, 0.0);
			A(0, 2) = std::complex<double>(0.0, 0.0);
			A(0, 3) = std::complex<double>(0.0, 0.0);

			A(1, 0) = std::complex<double>(r, 0.0);
			A(1, 1) = std::complex<double>(-r, 0.0);
			A(1, 2) = std::complex<double>(0.0, 0.0);
			A(1, 3) = std::complex<double>(0.0, 0.0);

			A(2, 0) = std::complex<double>(0.0, 0.0);
			A(2, 1) = std::complex<double>(0.0, 0.0);
			A(2, 2) = std::complex<double>(1.0, 0.0);
			A(2, 3) = std::complex<double>(1.0, 0.0);

			A(3, 0) = std::complex<double>(0.0, 0.0);
			A(3, 1) = std::complex<double>(0.0, 0.0);
			A(3, 2) = std::complex<double>(0.0, -1.0);
			A(3, 3) = std::complex<double>(0.0, 1.0);

			At = boost::numeric::ublas::herm(A);
		}
		else return false;

		const unsigned int nOutRows = OutputRasterPtr->getNumberOfRows();
		const unsigned int nOutCols = OutputRasterPtr->getNumberOfColumns();
		
		//std::vector< std::complex< double > > c_or_t(InputRasterBandsSize);
		//std::vector< std::complex< double > > t_or_c(InputRasterBandsSize);
		//std::vector< std::complex< double > > aux(InputRasterBandsSize);

		boost::numeric::ublas::matrix< std::complex<double> > c_or_t(msize, msize);
		boost::numeric::ublas::matrix< std::complex<double> > t_or_c(msize, msize);
		boost::numeric::ublas::matrix< std::complex<double> > aux(msize, msize);

		
		for (unsigned int j = 0; j < nOutRows - 1; ++j) {
			for (unsigned int k = 0; k < nOutCols - 1; ++k) {
				// [T] = [A].[C].[A]*
				// or
				// [C] = [A].[T].[A]*

				//get the value of each band of the input raster 
				for (size_t i = 0; i < InputRasterBandsSize; ++i) 
					InputRasterPtrs[i]->getBand(InputRasterBands[i])->getValue(k, j, c_or_t(i / msize, i % msize));
				aux = boost::numeric::ublas::prod(c_or_t, At);
				t_or_c = boost::numeric::ublas::prod(A, aux);
				for (size_t i = 0; i < InputRasterBandsSize; ++i) 
					OutputRasterPtr->getBand(i)->setValue(k, j, t_or_c(i / msize, i % msize));

				/*for (size_t i = 0; i < InputRasterBandsSize; ++i)
					InputRasterPtrs[i]->getBand(InputRasterBands[i])->getValue(k, j, c_or_t[i]);


				if (!teradar::common::GetProduct(c_or_t, At, aux))
					return false;

				if (!teradar::common::GetProduct(A, aux, t_or_c))
					return false;

				for (size_t i = 0; i < InputRasterBandsSize; ++i) 
					OutputRasterPtr->getBand(i)->setValue(k, j, t_or_c[i]);*/

			}//end for k
		}// end for j
		return true;
	}// end ChangeCohtoCov

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
