/*  Copyright (C) 2015 National Institute For Space Research (INPE) - Brazil.

This file is part of the TerraRadar - a library and application for radar data manipulation.

TerraRadar is under development.
*/

/*!
\file terraradar/tests/unittest/common/multiResolution_unitTest.cpp
\brief A test suite for the MultiResolution class.
*/

// TerraRadar includes
#include "BuildConfig.hpp"
#include "Functions.hpp"
#include "MultiResolution.hpp"
#include "RadarFunctions.hpp"
#include "Utils.hpp"

// TerraLib includes
#include <terralib/common/TerraLib.h>
#include <terralib/plugin.h>

// Gtest includes
#include <gtest/gtest.h>

// @todo - etore - fix it when the problem with SRS was fixed in TerraLib
TEST( InitMethods, loadTerralib )
{
  // load TerraLib and it drivers
  TerraLib::getInstance().initialize();
  teradar::common::loadTerraLibDrivers();
}

TEST( MultiResolution, basicTests )
{
  // open the input raster
  std::map<std::string, std::string> inputRasterInfo;
  inputRasterInfo["URI"] = TERRARADAR_DATA_DIR "/rasters/ref_ImagPol240_0.bin";
  te::rst::Raster* inputRaster( te::rst::RasterFactory::open( inputRasterInfo ) );
  EXPECT_TRUE( inputRaster != NULL );

  unsigned int m_inputLevels = 4;
  
  teradar::common::MultiResolution*
    multiRes = new teradar::common::MultiResolution( *inputRaster, m_inputLevels );

  size_t levels = multiRes->getNumberOfLevels();
  EXPECT_EQ( levels, 5 );
 
  //
  // L0
  //
  te::rst::Raster* levelZeroRaster = multiRes->getLevel( 0 );

  std::complex<double> v1, v2, v3, v4;
  levelZeroRaster->getValue( 0, 0, v1 );
  levelZeroRaster->getValue( 1, 0, v2 );
  levelZeroRaster->getValue( 0, 1, v3 );
  levelZeroRaster->getValue( 1, 1, v4 );

  EXPECT_EQ( levelZeroRaster->getNumberOfColumns(), 240 );
  EXPECT_EQ( levelZeroRaster->getNumberOfRows(), 240 );
  
  double eps = 1e-6; 
  
  EXPECT_NEAR( v1.real(), -0.003844, eps );
  EXPECT_NEAR( v1.imag(), 0.035258, eps );

  EXPECT_NEAR( v2.real(), 0.059669, eps );
  EXPECT_NEAR( v2.imag(), -0.006968, eps );

  EXPECT_NEAR( v3.real(), 0.011045, eps );
  EXPECT_NEAR( v3.imag(), -0.045011, eps );

  EXPECT_NEAR( v4.real(), 0.015729, eps );
  EXPECT_NEAR( v4.imag(), -0.049620, eps );

  std::complex<double> sum = v1 + v2 + v3 + v4;
  std::complex<double> mean = sum / 4.;

  EXPECT_NEAR( sum.real(), 0.082599, eps );
  EXPECT_NEAR( sum.imag(), -0.066342, eps );

  EXPECT_NEAR( mean.real(), 0.020650, eps );
  EXPECT_NEAR( mean.imag(), -0.016585, eps );

  // The values in L0 must be the same from Input image. Check it.
  std::complex<double> iv1, iv2, iv3, iv4;
  inputRaster->getValue( 0, 0, iv1 );
  inputRaster->getValue( 1, 0, iv2 );
  inputRaster->getValue( 0, 1, iv3 );
  inputRaster->getValue( 1, 1, iv4 );

  EXPECT_NEAR( v1.real(), iv1.real(), eps );
  EXPECT_NEAR( v2.real(), iv2.real(), eps );
  EXPECT_NEAR( v3.real(), iv3.real(), eps );
  EXPECT_NEAR( v4.real(), iv4.real(), eps );

  //
  // L1
  //
  te::rst::Raster* levelOneRaster = multiRes->getLevel( 1 );

  EXPECT_EQ( levelOneRaster->getNumberOfColumns(), 120 );
  EXPECT_EQ( levelOneRaster->getNumberOfRows(), 120 );

  std::complex<double> l2v1;
  levelOneRaster->getValue( 0, 0, l2v1 );
  
  EXPECT_NEAR( l2v1.real(), 0.020650, eps );
  EXPECT_NEAR( l2v1.imag(), -0.016585, eps );

  // Free memory
  delete inputRaster;
  delete levelOneRaster;
}

TEST( MultiResolution, removeTest )
{
  // open the input raster
  std::map<std::string, std::string> inputRasterInfo;
  inputRasterInfo["URI"] = TERRARADAR_DATA_DIR "/rasters/ref_ImagPol240_0.bin";
  te::rst::Raster* inputRaster( te::rst::RasterFactory::open( inputRasterInfo ) );
  EXPECT_TRUE( inputRaster != NULL );

  unsigned int m_inputLevels = 2;

  teradar::common::MultiResolution*
    multiRes = new teradar::common::MultiResolution( *inputRaster, m_inputLevels );

  size_t levels = multiRes->getNumberOfLevels();
  EXPECT_EQ( levels, 3 );

  multiRes->remove();
  EXPECT_TRUE( inputRaster != NULL );

  // Free memory
  delete inputRaster;
}

TEST( MultiResolution, createTest )
{
  // open the input raster
  std::map<std::string, std::string> inputRasterInfo;
  inputRasterInfo["URI"] = TERRARADAR_DATA_DIR "/rasters/ref_ImagPol240_0_cov.tif";
  te::rst::Raster* inputRaster( te::rst::RasterFactory::open( inputRasterInfo ) );
  EXPECT_TRUE( inputRaster != NULL );
  
  teradar::common::MultiResolution*
    multiRes = new teradar::common::MultiResolution( *inputRaster, 3, false );

  te::rst::Raster* levelRaster = multiRes->getLevel( 0 );
  EXPECT_EQ( levelRaster->getNumberOfColumns(), 240 );
  EXPECT_EQ( levelRaster->getNumberOfRows(), 240 );

  // @todo - etore - check against reference images
  teradar::common::CopyComplex2DiskRaster( *levelRaster, "test_ImagPol240_0_cov_L0.tif" );

  levelRaster = multiRes->getLevel( 1 );
  EXPECT_EQ( levelRaster->getNumberOfColumns(), 120 );
  EXPECT_EQ( levelRaster->getNumberOfRows(), 120 );

  teradar::common::CopyComplex2DiskRaster( *levelRaster, "test_ImagPol240_0_cov_L1.tif" );

  levelRaster = multiRes->getLevel( 2 );
  EXPECT_EQ( levelRaster->getNumberOfColumns(), 60 );
  EXPECT_EQ( levelRaster->getNumberOfRows(), 60 );

  teradar::common::CopyComplex2DiskRaster( *levelRaster, "test_ImagPol240_0_cov_L2.tif" );

  // Free memory
  delete levelRaster;
  delete inputRaster;
}

TEST( MultiResolution, computeMaxCompressionLevelTests )
{
  EXPECT_EQ( teradar::common::MultiResolution::computeMaxCompressionLevel( 440, 480 ), 9 );
  EXPECT_EQ( teradar::common::MultiResolution::computeMaxCompressionLevel( 240, 320 ), 8 );
  EXPECT_EQ( teradar::common::MultiResolution::computeMaxCompressionLevel( 4250, 5923 ), 12 );
  EXPECT_EQ( teradar::common::MultiResolution::computeMaxCompressionLevel( 4250, 8 ), 3 );
  EXPECT_EQ( teradar::common::MultiResolution::computeMaxCompressionLevel( 2, 4 ), 1 );
}
