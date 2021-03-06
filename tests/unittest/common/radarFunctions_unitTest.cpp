/*  Copyright (C) 2015 National Institute For Space Research (INPE) - Brazil.

This file is part of the TerraRadar - a library and application for radar data manipulation.

TerraRadar is under development.
*/

/*!
\file terraradar/tests/unittest/common/radarFunctions_unitTest.cpp
\brief A test suite for the functions that handles Radar data.
*/

#include "stdlib.h"
// TerraRadar includes
#include "BuildConfig.hpp"
#include "Functions.hpp"
#include "RadarFunctions.hpp"
#include "Utils.hpp"

// TerraLib includes
#include <terralib/common/TerraLib.h>
#include <terralib/rp/Functions.h>
#include <terralib/plugin.h>

// Gtest includes
#include <gtest/gtest.h>


/*TEST( RadarFunctions, createCovarianceTest1 )
{
	// open the input raster
	std::map<std::string, std::string> inputRasterInfo;
	inputRasterInfo["URI"] = "C:/Users/Diego/Desktop/ima/ALOS_FINAL_PREENCHIDA.tif";
	te::rst::Raster* inputRaster( te::rst::RasterFactory::open( inputRasterInfo ) );
	EXPECT_TRUE( inputRaster != NULL );

	std::size_t nBands = inputRaster->getNumberOfBands();
	// create input data (bands and rasters)
	std::vector<unsigned int> inputBands;
	std::vector<te::rst::Raster*> inputRasters;

	for (unsigned int i = 0; i < nBands; ++i) {
		inputBands.push_back( i );
		inputRasters.push_back( inputRaster );
	}

	// call CreateCovarianceRaster and check the return value.
	std::auto_ptr<te::rst::Raster> covMatrixRaster;
	std::map<std::string, std::string> covRasterInfo;

	bool covResult = teradar::common::CreateCovarianceRaster( inputRasters, inputBands, covRasterInfo, "MEM", covMatrixRaster);
	
	teradar::common::CopyComplex2DiskRaster(*covMatrixRaster, "C:/Users/Diego/Desktop/ima/covariancia.tif"); // save the created file

	delete inputRaster; // Free memory
}*/


/*TEST(RadarFunctions, createCoherenceTest1)
{
	std::map<std::string, std::string> inputRasterInfo;
	inputRasterInfo["URI"] = "C:/Users/Diego/Desktop/ima/ref_ImagPol240_0.bin";
	te::rst::Raster* inputRaster(te::rst::RasterFactory::open(inputRasterInfo));

	EXPECT_TRUE(inputRaster != NULL);
	std::size_t nBands = inputRaster->getNumberOfBands();

	// create input data(bands and rasters)
	std::vector<unsigned int> inputBands;
	std::vector<te::rst::Raster*> inputRasters;

	//get the number of bands
	for (unsigned int i = 0; i < nBands; ++i) {
		inputBands.push_back(i);
		inputRasters.push_back(inputRaster);
	}

	// call CreateCovarianceRaster and check the return value.
	std::auto_ptr<te::rst::Raster> cohMatrixRaster;
	std::map<std::string, std::string> cohRasterInfo;

	bool cohResult = teradar::common::CreateCoherenceRaster(inputRasters, inputBands, cohRasterInfo, "MEM", cohMatrixRaster);
	EXPECT_TRUE(cohResult);

	teradar::common::CopyComplex2DiskRaster(*cohMatrixRaster, "C:/Users/Diego/Desktop/ima/coerencia1.tif"); // save the created file

	delete inputRaster;  // Free memory
}*/


/*TEST(RadarFunctions, C2T)
{
	// open the input raster
	std::map<std::string, std::string> inputRasterInfo;
	inputRasterInfo["URI"] = "C:/Users/Diego/Desktop/ima/covariancia.tif"; 
	te::rst::Raster* inputRaster(te::rst::RasterFactory::open(inputRasterInfo));
	EXPECT_TRUE(inputRaster != NULL);


	std::size_t nBands = inputRaster->getNumberOfBands();
	// create input data (bands and rasters)
	std::vector<unsigned int> inputBands;
	std::vector<te::rst::Raster*> inputRasters;

	for (unsigned int i = 0; i < nBands; ++i) {
		inputBands.push_back(i);
		inputRasters.push_back(inputRaster);
	}

	std::auto_ptr<te::rst::Raster> MatrixRaster;
	std::map<std::string, std::string> RasterInfo;

	bool Result = teradar::common::ChangeCohtoCov(inputRasters, inputBands, 1, RasterInfo, "MEM", MatrixRaster);

	teradar::common::CopyComplex2DiskRaster(*MatrixRaster, "C:/Users/Diego/Desktop/ima/coerenciaBoost.tif"); // save the created file
	
	delete inputRaster; // Free memory
}*/

/*TEST( RadarFunctions, createIntensityTest1 )
{
  // open the input raster
  std::map<std::string, std::string> inputRasterInfo;
  inputRasterInfo["URI"] = TERRARADAR_DATA_DIR "/rasters/ref_ImagPol240_0_cov.tif";
  te::rst::Raster* inputRaster( te::rst::RasterFactory::open( inputRasterInfo ) );
  EXPECT_TRUE( inputRaster != NULL );

  // call CreateIntensityRaster and check the return value.
  std::auto_ptr<te::rst::Raster> intensityRaster;
  std::map<std::string, std::string> intRasterInfo;

  bool intResult = teradar::common::CreateIntensityRaster( inputRaster, intRasterInfo, "MEM", intensityRaster );
  EXPECT_TRUE( intResult );

  // save the created file
  teradar::common::CopyComplex2DiskRaster( *intensityRaster, "test_ImagPol240_0_int.tif" );

  // @todo - etore - check if the image has been generated correctly.

  // Free memory
  delete inputRaster;
}

TEST( RadarFunctions, computeCovarianceCorrelationTest1 )
{
  // open the input raster
  std::map<std::string, std::string> inputRasterInfo;
  inputRasterInfo["URI"] = TERRARADAR_DATA_DIR "/rasters/ref_ImagPol240_0_int.tif";
  te::rst::Raster* inputRaster( te::rst::RasterFactory::open( inputRasterInfo ) );
  EXPECT_TRUE( inputRaster != NULL );

  double computedCovariance;
  double computedCorrelation;

  EXPECT_TRUE( teradar::common::ComputeCovarianceAndPearsonCorrelation( inputRaster, 0, inputRaster, 0,
    computedCovariance, computedCorrelation ));

  double eps = 1e-6;

  EXPECT_NEAR( computedCovariance, 0., eps );
  EXPECT_NEAR( computedCorrelation, 1.0, eps );

  // Free memory
  delete inputRaster;
}

TEST( RadarFunctions, computeCovarianceCorrelationTest2 )
{
  // open the input raster
  std::map<std::string, std::string> inputRasterInfo;
  inputRasterInfo["URI"] = TERRARADAR_DATA_DIR "/rasters/ref_ImagPol240_0_cov_L1.tif";
  te::rst::Raster* inputRaster( te::rst::RasterFactory::open( inputRasterInfo ) );
  EXPECT_TRUE( inputRaster != NULL );

  double computedCovariance;
  double computedCorrelation;

  EXPECT_TRUE( teradar::common::ComputeCovarianceAndPearsonCorrelation( inputRaster, 0, inputRaster, 1,
    computedCovariance, computedCorrelation ) );

  double eps = 1e-8;

  EXPECT_NEAR( computedCovariance, 0.00000320040510322, eps );
  EXPECT_NEAR( computedCorrelation, 2.4766355446561, eps );

  // Free memory
  delete inputRaster;
}

TEST( RadarFunctions, computeCovarianceCorrelationTest3 )
{
  // open the input raster
  std::map<std::string, std::string> inputRasterInfo;
  inputRasterInfo["URI"] = TERRARADAR_DATA_DIR "/rasters/ref_ImagPol240_0_cov_L1.tif";
  te::rst::Raster* inputRaster( te::rst::RasterFactory::open( inputRasterInfo ) );
  EXPECT_TRUE( inputRaster != NULL );

  double computedCovariance;
  double computedCorrelation;

  EXPECT_TRUE( teradar::common::ComputeCovarianceAndPearsonCorrelation( inputRaster, 0, 0, 120, 0, 120, 
    inputRaster, 1, 0, 120, 0, 120, computedCovariance, computedCorrelation ) );

  double eps = 1e-8;

  EXPECT_NEAR( computedCovariance, 0.00000320040510322, eps );
  EXPECT_NEAR( computedCorrelation, 2.4766355446561, eps );

  // Free memory
  delete inputRaster;
}

TEST( RadarFunctions, computeCovarianceCorrelationTest4 )
{
  // open the input raster
  std::map<std::string, std::string> inputRasterInfo;
  inputRasterInfo["URI"] = TERRARADAR_DATA_DIR "/rasters/ref_ImagPol240_0_cov_L1.tif";
  te::rst::Raster* inputRaster( te::rst::RasterFactory::open( inputRasterInfo ) );
  EXPECT_TRUE( inputRaster != NULL );

  double computedCovariance;
  double computedCorrelation;

  EXPECT_TRUE( teradar::common::ComputeCovarianceAndPearsonCorrelation( inputRaster, 0, 0, 119, 0, 119,
    inputRaster, 1, 1, 120, 1, 120, computedCovariance, computedCorrelation ) );

  double eps = 1e-8;

  EXPECT_NEAR( computedCovariance, 0.00000230947539, eps );
  EXPECT_NEAR( computedCorrelation, 1.7083930593264, eps );

  // Free memory
  delete inputRaster;
}

TEST( RadarFunctions, computeMinCompressionLevelTests )
{
  EXPECT_EQ( teradar::common::ComputeMinCompressionLevel( 8, 1., 4.5 ), 3 );
  EXPECT_EQ( teradar::common::ComputeMinCompressionLevel( 8, 2., 2.2 ), 1 );
}

TEST( RadarFunctions, computeMinCompLevelENL )
{
  std::pair<unsigned int, double>
    p1 = teradar::common::ComputeMinCompLevelENL( teradar::common::ScatteringVectorT, 3, 8, 1. );

  EXPECT_EQ( p1.first, 3 );
  EXPECT_EQ( p1.second, 4.55 );

  std::pair<unsigned int, double>
    p2 = teradar::common::ComputeMinCompLevelENL( teradar::common::CovarianceMatrixT, 16, 8, 1. );

  EXPECT_EQ( p2.first, 3 );
  EXPECT_EQ( p2.second, 7.65 );
}
*/