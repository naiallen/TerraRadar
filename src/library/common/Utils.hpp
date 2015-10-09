/*  Copyright (C) 2015 National Institute For Space Research (INPE) - Brazil.

This file is part of the TerraRadar - a library and application for radar data manipulation.

TerraRadar is under development.
*/

/*!
\file terraradar/library/common/Utils.hpp
\brief Useful functions and procedures.
*/

#ifndef TERRARADAR_LIB_COMMON_INTERNAL_UTILS_HPP_
#define TERRARADAR_LIB_COMMON_INTERNAL_UTILS_HPP_

// TerraRadar Includes
#include "config.hpp"

namespace teradar {
	namespace common {
    /*!
      \enum PixelConnectivity type.
    */
    enum PixelConnectivityType {
      VonNeumannNT = 0, //< Von Neumann type - 4 connected.
      MooreNT = 1, //< Moore type - 8 connected.
    };

    /*! \brief Load TerraLib Modules needed */
    TERADARCOMMONEXPORT void loadTerraLibDrivers();
  }  // end namespace common
}  // end namespace teradar

#endif  // TERRARADAR_LIB_COMMON_INTERNAL_UTILS_HPP_