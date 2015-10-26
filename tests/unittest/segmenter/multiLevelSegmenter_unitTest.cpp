/*  Copyright (C) 2015 National Institute For Space Research (INPE) - Brazil.

This file is part of the TerraRadar - a library and application for radar data manipulation.

TerraRadar is under development.
*/

/*!
\file terraradar/tests/unittest/common/radarFunctions_unitTest.cpp
\brief A test suite for the functions that handles Radar data.
*/

// TerraRadar includes
#include "MultiLevelSegmenter.hpp"
#include "SegmenterRegionGrowingWishartMerger.hpp"
#include "SegmenterRegionGrowingWishartStrategy.hpp"

#include "BuildConfig.hpp"
#include "Functions.hpp"
#include "RadarFunctions.hpp"
#include "Utils.hpp"

// TerraLib includes
#include <terralib/common/progress/ConsoleProgressViewer.h>
#include <terralib/common/progress/ProgressManager.h>
#include <terralib/common/TerraLib.h>
#include <terralib/plugin.h>

// Gtest includes
#include <gtest/gtest.h>

TEST( MultiLevelSegmenter, xxxTest )
{
  // Progress interface
  te::common::ConsoleProgressViewer progressViewerInstance;
  int viewerId = te::common::ProgressManager::getInstance().addViewer( &progressViewerInstance );

  // open the input raster
  std::map<std::string, std::string> inputRasterInfo;
  inputRasterInfo["URI"] = TERRARADAR_DATA_DIR "/rasters/ref_ImagPol240_0_cov.tif";
  boost::shared_ptr< te::rst::Raster > inputRasterPointer( te::rst::RasterFactory::open(
    inputRasterInfo ) );
  EXPECT_TRUE( inputRasterPointer.get() != NULL );

  /////////////////////////////////////////
  // access a raster datasource to create the output raster

  std::map<std::string, std::string> outputRasterInfo;
  outputRasterInfo["URI"] = "xxx.tif";

  // Creating the algorithm parameters
  teradar::segmenter::SegmenterRegionGrowingWishartStrategy::Parameters strategyParameters;

  strategyParameters.m_minSegmentSize = 100;
  strategyParameters.m_dataType = teradar::common::CovarianceMatrixT;
  strategyParameters.m_enableAzimutalSimetry = false;
  strategyParameters.m_enlLZero = 1.583;
  strategyParameters.m_compressionLevel = 1;
  strategyParameters.m_connectivityType = teradar::common::VonNeumannNT;
  strategyParameters.m_regionGrowingLimit = 15;
  strategyParameters.m_regionGrowingConfLevel = 99.9;
  strategyParameters.m_regionMergingLimit = 1;
  strategyParameters.m_regionMergingConfLevel = 99.9;

  teradar::segmenter::MultiLevelSegmenter::InputParameters algoInputParams;
  algoInputParams.m_inputRasterPtr = inputRasterPointer.get();
  algoInputParams.m_inputRasterBands.push_back( 0 );
  algoInputParams.m_inputRasterBands.push_back( 1 );
  algoInputParams.m_inputRasterBands.push_back( 2 );
  algoInputParams.m_inputRasterBands.push_back( 3 );
  algoInputParams.m_inputRasterBands.push_back( 4 );
  algoInputParams.m_inputRasterBands.push_back( 5 );
  algoInputParams.m_inputRasterBands.push_back( 6 );
  algoInputParams.m_inputRasterBands.push_back( 7 );
  algoInputParams.m_inputRasterBands.push_back( 8 );
  algoInputParams.m_enableThreadedProcessing = false;
  algoInputParams.m_maxSegThreads = 0;
  algoInputParams.m_enableBlockProcessing = false;
  algoInputParams.m_blocksOverlapPercent = 0;
  algoInputParams.m_maxBlockSize = 0;
  algoInputParams.m_strategyName = "RegionGrowingWishart";
  algoInputParams.setSegStrategyParams( strategyParameters );
  algoInputParams.m_enableProgress = true;

  teradar::segmenter::MultiLevelSegmenter::OutputParameters algoOutputParams;
  algoOutputParams.m_rInfo = outputRasterInfo;
  algoOutputParams.m_rType = "GDAL";

  // Executing the algorithm

  teradar::segmenter::MultiLevelSegmenter algorithmInstance;

  EXPECT_TRUE( algorithmInstance.initialize( algoInputParams ) );
  EXPECT_TRUE( algorithmInstance.execute( algoOutputParams ) );

  te::common::ProgressManager::getInstance().removeViewer( viewerId );
}
// @todo - etore - fix it when the problem with SRS was fixed in TerraLib
TEST( EndMethods, finalizeTerralib )
{
  te::plugin::PluginManager::getInstance().shutdownAll();
  te::plugin::PluginManager::getInstance().unloadAll();
  TerraLib::getInstance().finalize();
}