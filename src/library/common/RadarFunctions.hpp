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
      \enum Radar Data Types.
    */
    enum RadarDataType {
      ScatteringVectorT = 0, //< Scattering vector.
      CovarianceMatrixT = 1, //< Covariance Matrix.
      IntensityT = 2, //< Intensity.
      AmplitudeT = 3 //< Amplitude.
    };

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

	/*NAIALLEN Compute the coherence*/
	/*!
	\brief Create a multi-band raster representing the coherence [T] matrix.
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
	TERADARCOMMONEXPORT bool CreateCoherenceRaster(const std::vector<te::rst::Raster*>& CohInputRasterPtrs,
		const std::vector<unsigned int>& CohInputRasterBands,
		const std::map<std::string, std::string>& CohOutputRasterInfo,
		const std::string& CohOutputDataSourceType,
		std::auto_ptr<te::rst::Raster>& CohOutputRasterPtr,
		const bool enableProgressInterface = false);

	/*NAIALLEN Compute the conversion
	0 to convert T to C 
	1 to convvert C to T*/
	/*!
	\brief Create a multi-band raster. Convert a covariance [C] matrix into
	a coherebce matrix [T] or the contrary.
	0 to convert T to C
	1 to convvert C to T	
	The number of bands is based on the number of input rasters, it can be 9 or 16.
	The method assumes that the bands from rasters have the same dimension.
	\param InputRasterPtrs Input rasters pointers.
	\param InputRasterBands Input raster bands (one band for each input raster).
	\param  t2c 0 to convert T to C  or 1 to convvert C to T
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
	TERADARCOMMONEXPORT bool ChangeCohtoCov(const std::vector<te::rst::Raster*>& InputRasterPtrs,
		const std::vector<unsigned int>& InputRasterBands,
		const int t2c,
		const std::map<std::string, std::string>& OutputRasterInfo,
		const std::string& OutputDataSourceType,
		std::auto_ptr<te::rst::Raster>& OutputRasterPtr,
		const bool enableProgressInterface = false);

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
      \note This method computes the number of rows and columns for each input band, validates it, and computes the values.
    */
    TERADARCOMMONEXPORT bool ComputeCovarianceAndPearsonCorrelation( const te::rst::Raster* inputRaster1Ptr,
      unsigned int inputRaster1Band, const te::rst::Raster* inputRaster2Ptr, unsigned int inputRaster2Band,
      double& covariance, double& correlation, const bool enableProgressInterface = false );

    /*!
      \brief Given two rasters, two band numbers and its boundaries, computes covariance and Pearson's correlation between them.
      \param raster1Ptr Pointer to the first raster used in computation.
      \param raster1Band Band to be used from the first raster.
      \param raster1XStart Left X coordinate of the raster 1.
      \param raster1YStart Upper Y coordinate of the raster 1.
      \param raster1XBound Right X coordinate of the raster 1.
      \param raster1YBound Lower Y coordinate of the raster 1.
      \param raster2Ptr Pointer to the second raster used in computation.
      \param raster2Band Band to be used from the second raster.
      \param raster2XStart Left X coordinate of the raster 2.
      \param raster2YStart Upper Y coordinate of the raster 2.
      \param raster2XBound Right X coordinate of the raster 2.
      \param raster2YBound Lower Y coordinate of the raster 2.
      \param covariance Computed covariance.
      \param correlation Computed correlation.
      \param enableProgressInterface Enable/disable the use of a progress interface when applicable.
      \return true if OK, false on errors.
    */
    TERADARCOMMONEXPORT bool ComputeCovarianceAndPearsonCorrelation( const te::rst::Raster* raster1Ptr, unsigned int raster1Band,
      unsigned int raster1XStart, unsigned int raster1XBound, unsigned int raster1YStart, unsigned int raster1YBound, 
      const te::rst::Raster* raster2Ptr, unsigned int raster2Band, unsigned int raster2XStart, unsigned int raster2XBound, 
      unsigned int raster2YStart, unsigned int raster2YBound,
      double& covariance, double& correlation, 
      const bool enableProgressInterface = false );

    /*!
      \brief This method computes the minimal compression level needed to allow the data being
      submitted in the Segmentation Process.
      \param maxLevel Max compression level.
      \param imageENL Equivalent Number of Looks of the image without compression.
      \param minENL Minimum ENL according to the number of bands being processed.
      \param autoCorrelation1 First element of autoCorrelation vector. Default is 0.8.
      \param autoCorrelation2 Second element of autoCorrelation vector. Default is 0.8.
      \param autoCorrelation3 Third element of autoCorrelation vector. Default is 0.8.
    */
    TERADARCOMMONEXPORT unsigned int ComputeMinCompressionLevel( unsigned int maxLevel, const double& imageENL,
      const double& minENL, const double& autoCorrelation1 = 0.8,
      const double& autoCorrelation2 = 0.8, const double& autoCorrelation3 = 0.8 );

    /*
      \brief 
      \param dataType Type of Radar Data.
      \param numberOfBands Number of input bands used in the computation.
      \param maxLevel Max compression level.
      \param imageENL Equivalent Number of Looks of the image without compression.
      \return A pair containing the minimal level of compression and the Equivalent Number of Looks.  
    */
    TERADARCOMMONEXPORT std::pair<unsigned int, double> 
      ComputeMinCompLevelENL( const teradar::common::RadarDataType& dataType,
      unsigned int numberOfBands, unsigned int maxLevel, const double& imageENL );
    
  }  // end namespace common
}  // end namespace teradar

#endif  // TERRARADAR_LIB_COMMON_INTERNAL_RADARFUNCTIONS_HPP_