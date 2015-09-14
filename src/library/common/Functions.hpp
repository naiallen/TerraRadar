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
    
    /*!
      \brief Convert to string.
      \param t What to convert.
      \return Converted string.
    */
    template<class T> TERADARCOMMONEXPORT
    std::string toString( const T& t ) {
      std::ostringstream ss;
      ss << t;
      return ss.str( );
    }

    /*!
      \brief Convert to string.
      \param t What to convert.
      \param s Converted string.
    */
    template<class T> TERADARCOMMONEXPORT
    void toString( const T& t, const std::string& s ) {
      t = toString<T>( s );
    }

    /*!
      \brief Convert from string.
      \param s String to be converted.
      \return Converted value.
    */
    template<class T> TERADARCOMMONEXPORT
    T fromString( const std::string& s ) {
      std::istringstream ss( s );
      T t = T( ); // set only to avoid valgrind error message
      ss >> t;
      return t;
    }

    /*!
      \brief Convert from string.
      \param t Converted value.
      \param s String to be converted.
    */
    template<class T> TERADARCOMMONEXPORT
    void fromString( T& t, const std::string& s ) {
      t = fromString<T>( s );
    }
    
  }  // end namespace common
}  // end namespace teradar

#endif  // TERRARADAR_LIB_COMMON_INTERNAL_FUNCTIONS_HPP_