/*  Copyright (C) 2015 National Institute For Space Research (INPE) - Brazil.

This file is part of the TerraRadar - a library and application for radar data manipulation.

TerraRadar is under development.
*/

/*!
\file terraradar/library/common/Functions.cpp
\brief Useful functions.
*/

// TerraRadar Includes
#include "Functions.hpp"

// TerraLib Includes
#include <terralib/rp/Functions.h>
#include <terralib/rp/RasterHandler.h>

//#include <string>

namespace teradar {
  namespace common {
    bool CopyComplex2DiskRaster( const te::rst::Raster& inputRaster,
      const std::string& fileName )
    {
      if( !( inputRaster.getAccessPolicy() & te::common::RAccess ) ) {
        return false;
      };

      const unsigned int nBands = (unsigned int)inputRaster.getNumberOfBands();
      const unsigned int nCols = inputRaster.getNumberOfColumns();
      const unsigned int nRows = inputRaster.getNumberOfRows();
      unsigned int bandIdx = 0;
      unsigned int col = 0;
      unsigned int row = 0;

      std::vector<te::rst::BandProperty*> bandsProperties;
      for( bandIdx = 0; bandIdx < nBands; ++bandIdx ) {
        bandsProperties.push_back
          ( new te::rst::BandProperty
          ( *( inputRaster.getBand( bandIdx )->getProperty() ) ) );
      }

      te::rp::RasterHandler outRasterHandler;

      if( !CreateNewGdalRaster( *(inputRaster.getGrid()), bandsProperties,
        fileName, outRasterHandler ) ) {
        return false;
      }

      std::complex<double> value = 0;

      for( bandIdx = 0; bandIdx < nBands; ++bandIdx ) {
        const te::rst::Band& inBand = *inputRaster.getBand( bandIdx );
        te::rst::Band& outBand = *outRasterHandler.getRasterPtr()->getBand( bandIdx );

        for( row = 0; row < nRows; ++row ) {
          for( col = 0; col < nCols; ++col ) {
            inBand.getValue( col, row, value );
            outBand.setValue( col, row, value );
          }
        }
      }

      return true;
    }
  }
}
