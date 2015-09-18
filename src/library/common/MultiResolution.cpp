/*  Copyright (C) 2015 National Institute For Space Research (INPE) - Brazil.

This file is part of the TerraRadar - a library and application for radar data manipulation.

TerraRadar is under development.
*/

/*!
  \file terraradar/library/common/MultiResolution.cpp
  \brief Multiresolution class that handles complex data.
*/

// TerraRadar includes
#include "MultiResolution.hpp"

namespace teradar {
  namespace common {
    /*
     * MultiResolution
     */
    MultiResolution::MultiResolution( const te::rst::Raster& inputRaster, size_t levels, const bool enableProgressInterface )
      : m_enableProgress( enableProgressInterface ) {
      m_levels.resize( levels + 1 );
      m_levels[0] = const_cast<te::rst::Raster*>(&inputRaster);

      createLevels();
    }

    MultiResolution::MultiResolution( const te::rst::Raster& inputRaster,
      size_t levels,
      const std::vector<size_t>& bandsNumbers,
      const bool enableProgressInterface )
      : m_bandsNumbers( bandsNumbers ),
      m_enableProgress( enableProgressInterface ) {
      // @todo - etore - should we merge constructors?
      m_levels.resize( levels + 1 );
      m_levels[0] = const_cast<te::rst::Raster*>(&inputRaster);

      createLevels();
    }
	
    MultiResolution::~MultiResolution() {
      // @todo - etore - complete
    }

    void MultiResolution::createLevel( const te::rst::Raster& srcRaster,
      te::rst::Raster& dstRaster ) {
      // this code assumes that the dstRaster have been created in the correct size
      // that is the half number of columns and lines from srcRaster, and same
      // number of bands

      // Each pixel in the dstRaster has the mean between 4 pixels from srcRaster,
      // if applicable. If in the border, the number can be lower than 4.

      // For instance:
      // pixel dstRaster(0,0) contains the mean of srcRaster pixels (0,0), (0,1), (1,0) and (1,1)
      //
      // pixel dstRaster(0,1) contains the mean of srcRaster pixels (0,2), (0,3), (1,2) and (1,3)
      //
      // pixel dstRaster(1,0) contains the mean of srcRaster pixels (2,0), (2,1), (3,0) and (3,1)

      // @todo - etore - should we check it?
      size_t bands = dstRaster.getNumberOfBands();

      unsigned int srcCols = srcRaster.getNumberOfColumns();
      unsigned int srcRows = srcRaster.getNumberOfRows();

      unsigned int dstCols = dstRaster.getNumberOfColumns();
      unsigned int dstRows = dstRaster.getNumberOfRows();

      std::complex<double> value = 0.;

      for( unsigned int c = 0; c < dstCols; ++c ) {
        unsigned int cr = c * 2;

        for( unsigned int r = 0; r < dstRows; ++r ) {
          unsigned int rr = r * 2;

          for( unsigned int b = 0; b < bands; ++b ) {
            std::complex<double> mean = 0.;
            unsigned int pixelCount = 0;

            unsigned int colToRead = cr;
            unsigned int rowToRead = rr;

            if( colToRead < srcCols && rowToRead < srcRows ) {
              srcRaster.getValue( colToRead, rowToRead, value, b );
              mean += value;
              ++pixelCount;
            }

            rowToRead = rr + 1;

            if( colToRead < srcCols && rowToRead < srcRows ) {
              srcRaster.getValue( colToRead, rowToRead, value, b );
              mean += value;
              ++pixelCount;
            }

            colToRead = cr + 1;

            if( colToRead < srcCols && rowToRead < srcRows ) {
              srcRaster.getValue( colToRead, rowToRead, value, b );
              mean += value;
              ++pixelCount;
            }

            rowToRead = rr;

            if( colToRead < srcCols && rowToRead < srcRows ) {
              srcRaster.getValue( colToRead, rowToRead, value, b );
              mean += value;
              ++pixelCount;
            }

            if( pixelCount == 0 ) {
              // @todo - etore - create a message ou anything to warn
              assert( false );
            }

            mean /= (double)pixelCount;
            dstRaster.setValue( c, r, mean, b );
          }
        }
      }
    }

    void MultiResolution::createLevels() {
      if( m_levels.size() == 1 ) {
        // nothing to be done
        return;
      }

      // @todo - etore - handle progress

      for( size_t l = 1; l < m_levels.size(); ++l ) {

        const te::rst::Raster* srcRaster = m_levels[l - 1];

        // read the bands properties
        size_t srcRasterBands = srcRaster->getNumberOfBands();

        std::vector<te::rst::BandProperty* > bandsProperties;

        for( size_t b = 0; b < srcRasterBands; ++b ) {
          bandsProperties.push_back( new te::rst::BandProperty
            ( *(srcRaster->getBand( b )->getProperty()) ) );
        }

        // read the grid
        te::rst::Grid* dstGrid = new te::rst::Grid( *(srcRaster->getGrid()) );

        // change the size
        unsigned int nCols = dstGrid->getNumberOfColumns() / 2;
        unsigned int nRows = dstGrid->getNumberOfRows() / 2;
        dstGrid->setNumberOfRows( nRows );
        dstGrid->setNumberOfColumns( nCols );

        // read the raster info
        std::map<std::string, std::string> dstInfo = srcRaster->getInfo();

        te::rst::Raster* levelRaster( te::rst::RasterFactory::make
          ( "MEM", dstGrid, bandsProperties, dstInfo ) );

        createLevel( *srcRaster, *levelRaster );

        m_levels[l] = levelRaster;
      }
    }

    size_t MultiResolution::getNumberOfLevels() const
    {
      return m_levels.size();
    }

    te::rst::Raster* MultiResolution::getLevel( size_t level ) const
    {
      assert( level < m_levels.size() );
      return m_levels[level];
    }

    void MultiResolution::remove()
    {
      // The m_levels[0] is a copy from inputRaster. Don't destroy it.
      if( m_levels.size() == 1 ) {
        return;
      }

      for( size_t i = 1; i < m_levels.size(); ++i ) {
        if( m_levels[i] != NULL ) {
          delete m_levels[i];
          m_levels[i] = NULL;
        }
      }
    }
  } // end namespace common
} // end namespace teradar
