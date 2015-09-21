/*  Copyright (C) 2015 National Institute For Space Research (INPE) - Brazil.

This file is part of the TerraRadar - a library and application for radar data manipulation.

TerraRadar is under development.
*/

/*!
\file terraradar/library/common/RadarFunctions.hpp
\brief Useful functions for radar data handling.
*/

#ifndef TERRARADAR_LIB_COMMON_INTERNAL_RADARFUNCTIONS_HPP_
#define TERRARADAR_LIB_COMMON_INTERNAL_RADARFUNCTIONS_HPP_

// TerraRadar Includes
#include "config.hpp"

// TerraLib Includes
#include <terralib/raster.h>

namespace teradar {
	namespace common {
    /*!
      \brief Create a one band raster representing the intensity matrix.
      The input raster must be a covariance matrix raster, containing (n ^ 2) bands.
      Actualy, supported values of n is 3 and 4.
      \param inputRasterPtr Input raster pointer, containing the covariance matrix.
      \param intensityRasterInfo Intensity raster connection info.
      \param outputDataSourceType Output raster datasource type.
      \param intensityRasterPtr A pointer to the created intensity raster.
      \param enableProgressInterface Enable/disable the use of a progress
      interface when applicable.
      \return true if OK, false on errors.
      \note The number of bands in output raster is aways one, independing on
      the number of bands in input raster.
    */
    TERADARCOMMONEXPORT bool CreateIntensityRaster( const te::rst::Raster* inputRasterPtr,
      const std::map<std::string, std::string>& intensityRasterInfo,
      const std::string& outputDataSourceType,
      std::auto_ptr<te::rst::Raster>& intensityRasterPtr,
      const bool enableProgressInterface = false );

    /*!
      \brief Create a multi-band raster representing the covariance matrix.
      The number of bands is based on the number of input rasters.
      If the number of input rasters is 3, the method assumes that the input
      is a scattering vector, and the rasters contain data for HH, HV(VH) and
      VV polarizations.
      In case of size 4, the method assumes that the input data is complete,
      containing the HH, HV, VH and VV polarizations.
      If the size is 6, the method assumes that the input is an incomplete covariance
      matrix which order is 3, and create a complete one.
      If the size is 10, the method assumes that the input is an incomplete covariance
      matrix which order is 4, and create a complete one.
      The method assumes that the bands from rasters have the same dimension.
      \param inputRasterPointers Input rasters pointers.
      \param inputRasterBands Input raster bands (one band for each input
      raster).
      \param outputRasterInfo Output raster connection info.
      \param outputDataSourceType Output raster datasource type.
      \param outputRasterPtr A pointer to the created output raster.
      \param enableProgressInterface Enable/disable the use of a progress
      interface when applicable.
      \return true if OK, false on errors.
      \note The number of bands in output raster is based on input. For the
      scattering vector, the output raster will contain 9 bands and, for the
      complete input (scattering matrix), the output will contain 16 bands.
    */
    TERADARCOMMONEXPORT bool CreateCovarianceRaster( const std::vector<te::rst::Raster*>& inputRasterPointers,
      const std::vector<unsigned int>& inputRasterBands,
      const std::map<std::string, std::string>& outputRasterInfo,
      const std::string& outputDataSourceType,
      std::auto_ptr<te::rst::Raster>& outputRasterPtr,
      const bool enableProgressInterface = false );

    /*!
      \brief Given two rasters and two band numbers, computes covariance and Pearson's correlation between them.
      \param inputRaster1Ptr Pointer to the first raster used in computation.
      \param inputRaster2Ptr Pointer to the second raster used in computation.
      \param inputRaster1Band Band to be used from the first raster.
      \param inputRaster2Band Band to be used from the second raster.
      \param covariance Computed covariance.
      \param correlation Computed correlation.
      \param enableProgressInterface Enable/disable the use of a progress interface when applicable.
      \return true if OK, false on errors.
      \note

    */
    TERADARCOMMONEXPORT bool ComputeCovarianceAndPearsonCorrelation( const te::rst::Raster* inputRaster1Ptr,
      unsigned int inputRaster1Band, const te::rst::Raster* inputRaster2Ptr, unsigned int inputRaster2Band,
      double& covariance, double& correlation, const bool enableProgressInterface = false );
  }  // end namespace common
}  // end namespace teradar

#endif  // TERRARADAR_LIB_COMMON_INTERNAL_RADARFUNCTIONS_HPP_