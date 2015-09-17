/*  Copyright (C) 2015 National Institute For Space Research (INPE) - Brazil.

This file is part of the TerraRadar - a library and application for radar data manipulation.

TerraRadar is under development.
*/

/*!
\file terraradar/library/common/Functions.hpp
\brief Useful functions.
*/

#ifndef TERRARADAR_LIB_COMMON_INTERNAL_FUNCTIONS_HPP_
#define TERRARADAR_LIB_COMMON_INTERNAL_FUNCTIONS_HPP_

// TerraLib Includes
#include <terralib/raster.h>

// TerraRadar Includes
#include "config.hpp"

namespace teradar {
	namespace common {
    /*!
      \brief Create a new complex raster into a GDAL datasource.
      \param rasterGrid The template grid used to create the output raster.
      \param fileName The output tif file name.
      \return true if OK, false on errors.
    */
    TERADARCOMMONEXPORT
      bool CopyComplex2DiskRaster( const te::rst::Raster& inputRaster,
        const std::string& fileName );
  }  // end namespace common
}  // end namespace teradar

#endif  // TERRARADAR_LIB_COMMON_INTERNAL_FUNCTIONS_HPP_