/*  Copyright (C) 2015 National Institute For Space Research (INPE) - Brazil.

This file is part of the TerraRadar - a library and application for radar data manipulation.

TerraRadar is under development.
*/

/*!
\file terraradar/tests/unittest/common/radarFunctions_unitTest.cpp
\brief A test suite for the functions that handles Radar data.
*/

// TerraRadar includes
#include "BuildConfig.hpp"
#include "Functions.hpp"
#include "RadarFunctions.hpp"
#include "Utils.hpp"

// TerraLib includes
#include <terralib/common/TerraLib.h>
#include <terralib/plugin.h>

// Gtest includes
#include <gtest/gtest.h>

TEST( InitMethods, loadTerralib )
{
  // load TerraLib and it drivers
  TerraLib::getInstance().initialize();
  teradar::common::loadTerraLibDrivers();
}

TEST( RadarFunctions, createCovarianceTest1 )
{
  // open the input raster
  std::map<std::string, std::string> inputRasterInfo;
  inputRasterInfo["URI"] = TERRARADAR_DATA_DIR "/rasters/ref_ImagPol240_0.bin";
  te::rst::Raster* inputRaster( te::rst::RasterFactory::open( inputRasterInfo ) );
  EXPECT_TRUE( inputRaster != NULL );

  // create input data (bands and rasters)
  std::vector<unsigned int> inputBands;
  std::vector<te::rst::Raster*> inputRasters;

  for( unsigned int i = 0; i < 3; ++i ) {
    inputBands.push_back( i );
    inputRasters.push_back( inputRaster );
  }

  // call CreateCovarianceRaster and check the return value.
  std::auto_ptr<te::rst::Raster> covMatrixRaster;
  std::map<std::string, std::string> covRasterInfo;

  bool covResult = teradar::common::CreateCovarianceRaster( inputRasters, inputBands, covRasterInfo, "MEM", covMatrixRaster );
  EXPECT_TRUE( covResult );

  // save the created file
  teradar::common::CopyComplex2DiskRaster( *covMatrixRaster, "test_ImagPol240_0_cov.tif" );

  // @todo - etore - check if the image has been generated correctly.

  // Free memory
  delete inputRaster;
}

TEST( RadarFunctions, createIntensityTest1 )
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

TEST( EndMethods, finalizeTerralib )
{
  te::plugin::PluginManager::getInstance().shutdownAll();
  te::plugin::PluginManager::getInstance().unloadAll();
  TerraLib::getInstance().finalize();
}