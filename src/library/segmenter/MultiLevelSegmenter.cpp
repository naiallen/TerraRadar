/*  Copyright (C) 2015 National Institute For Space Research (INPE) - Brazil.

This file is part of the TerraRadar - a library and application for radar data manipulation.

TerraRadar is under development.
*/

/*!
  \file terraradar/library/segmenter/MultiLevelSegmenter.cpp
  \brief MultiLevel Segmentation.
*/

// TerraRadar includes
#include "MultiLevelSegmenter.hpp"

// TerraLib includes
#include <terralib/raster/BandProperty.h>
#include <terralib/raster/Grid.h>
#include <terralib/raster/RasterFactory.h>
#include <terralib/raster/SynchronizedRaster.h>
#include <terralib/rp/SegmenterStrategyFactory.h>

namespace teradar {
  namespace segmenter {
    // Input parameters
    MultiLevelSegmenter::InputParameters::InputParameters()
    {
      m_segStratParamsPtr = 0;
      reset();
    }

    MultiLevelSegmenter::InputParameters::InputParameters( const InputParameters& other )
    {
      m_segStratParamsPtr = 0;
      reset();
      operator=(other);
    }

    MultiLevelSegmenter::InputParameters::~InputParameters()
    {
      reset();
    }

    void MultiLevelSegmenter::InputParameters::reset() throw(te::rp::Exception)
    {
      m_inputRasterPtr = 0;
      m_inputRasterBands.clear();
      m_inputRasterNoDataValues.clear();
      m_enableThreadedProcessing = false;
      m_maxSegThreads = 0;
      m_enableBlockProcessing = false;
      m_maxBlockSize = 0;
      m_blocksOverlapPercent = 10;
      m_strategyName.clear();
      m_enableProgress = false;
      m_enableRasterCache = true;

      if( m_segStratParamsPtr )
      {
        delete m_segStratParamsPtr;
        m_segStratParamsPtr = 0;
      }
    }

    const MultiLevelSegmenter::InputParameters& MultiLevelSegmenter::InputParameters::operator=(
      const MultiLevelSegmenter::InputParameters& params)
    {
      reset();

      m_inputRasterPtr = params.m_inputRasterPtr;
      m_inputRasterBands = params.m_inputRasterBands;
      m_inputRasterNoDataValues = params.m_inputRasterNoDataValues;
      m_enableThreadedProcessing = params.m_enableThreadedProcessing;
      m_maxSegThreads = params.m_maxSegThreads;
      m_enableBlockProcessing = params.m_enableBlockProcessing;
      m_maxBlockSize = params.m_maxBlockSize;
      m_blocksOverlapPercent = params.m_blocksOverlapPercent;
      m_strategyName = params.m_strategyName;
      m_enableProgress = params.m_enableProgress;
      m_enableRasterCache = params.m_enableRasterCache;

      m_segStratParamsPtr = params.m_segStratParamsPtr ?
        (te::rp::SegmenterStrategyParameters*)params.m_segStratParamsPtr->clone()
        : 0;

      return *this;
    }

    te::common::AbstractParameters* MultiLevelSegmenter::InputParameters::clone() const
    {
      return new InputParameters( *this );
    }

    void MultiLevelSegmenter::InputParameters::setSegStrategyParams(
      const te::rp::SegmenterStrategyParameters& params )
    {
      if( m_segStratParamsPtr )
      {
        delete m_segStratParamsPtr;
        m_segStratParamsPtr = 0;
      }

      m_segStratParamsPtr = (te::rp::SegmenterStrategyParameters*)params.clone();
    }

    te::rp::SegmenterStrategyParameters const*
      MultiLevelSegmenter::InputParameters::getSegStrategyParams() const
    {
        return m_segStratParamsPtr;
    }
  
    // Output parameters
    MultiLevelSegmenter::OutputParameters::OutputParameters()
    {
      reset();
    }

    MultiLevelSegmenter::OutputParameters::OutputParameters( const OutputParameters& other )
    {
      reset();
      operator=(other);
    }

    MultiLevelSegmenter::OutputParameters::~OutputParameters()
    {
      reset();
    }

    void MultiLevelSegmenter::OutputParameters::reset() throw(te::rp::Exception)
    {
      m_rType.clear();
      m_rInfo.clear();
      m_outputRasterPtr.reset();
    }

    const MultiLevelSegmenter::OutputParameters& MultiLevelSegmenter::OutputParameters::operator=(
      const MultiLevelSegmenter::OutputParameters& params)
    {
      reset();

      m_rType = params.m_rType;
      m_rInfo = params.m_rInfo;

      return *this;
    }

    te::common::AbstractParameters* MultiLevelSegmenter::OutputParameters::clone() const
    {
      return new OutputParameters( *this );
    }

    // SegmenterThreadEntryParams
    MultiLevelSegmenter::SegmenterThreadEntryParams::SegmenterThreadEntryParams()
    {
      m_inputParameters.reset();
      m_outputParametersPtr = 0;
      m_segsBlocksMatrixPtr = 0;
      m_generalMutexPtr = 0;
      m_inputRasterSyncPtr = 0;
      m_outputRasterSyncPtr = 0;
      m_blockProcessedSignalMutexPtr = 0;
      m_abortSegmentationFlagPtr = 0;
      m_segmentsIdsManagerPtr = 0;
      m_blockProcessedSignalPtr = 0;
      m_runningThreadsCounterPtr = 0;
      m_inputRasterBandMinValues.clear();
      m_inputRasterBandMaxValues.clear();
      m_inputRasterNoDataValues.clear();
      m_enableStrategyProgress = false;
      m_maxInputRasterCachedBlocks = 0;
    }

    MultiLevelSegmenter::SegmenterThreadEntryParams::~SegmenterThreadEntryParams()
    {
    }

    // MultiLevel Segmenter
    MultiLevelSegmenter::MultiLevelSegmenter()
    {
      m_instanceInitialized = false;
    }

    MultiLevelSegmenter::~MultiLevelSegmenter()
    {
    }

    bool MultiLevelSegmenter::execute( te::rp::AlgorithmOutputParameters& outputParams )
      throw(te::rp::Exception)
    {
      // @todo - etore - complete
      return false;
    }

    void MultiLevelSegmenter::reset() throw(te::rp::Exception)
    {
      m_instanceInitialized = false;
      m_inputParameters.reset();
    }

    bool MultiLevelSegmenter::initialize( const te::rp::AlgorithmInputParameters& inputParams )
      throw(te::rp::Exception)
    {
      reset();

      // @todo - etore - complete
      return false;
    }

    // @todo - etore - Verify under this line
    bool MultiLevelSegmenter::isInitialized() const
    {
      return m_instanceInitialized;
    }

    bool MultiLevelSegmenter::calcBestBlockSize(
      const unsigned int minExapandedBlockPixels,
      const unsigned int maxExapandedBlockPixels,
      unsigned int& blocksHOverlapSize,
      unsigned int& blocksVOverlapSize,
      unsigned int& nonExpandedBlockWidth,
      unsigned int& nonExpandedBlockHeight,
      unsigned int& expandedBlockWidth,
      unsigned int& expandedBlockHeight ) const
    {
      TERP_TRUE_OR_THROW( minExapandedBlockPixels <= maxExapandedBlockPixels,
        "Invalid min and mas block pixels number" );

      const double totalImageLines = (double)m_inputParameters.m_inputRasterPtr->getNumberOfRows();
      const double totalImageCols = (double)m_inputParameters.m_inputRasterPtr->getNumberOfColumns();

      const double maxScaleFactor = (totalImageLines * totalImageCols) /
        ((double)minExapandedBlockPixels);

      unsigned int rescaledAndExpandedBlockPixelsNmb = 0;

      for( double scaleFactor = 1.0; scaleFactor <= maxScaleFactor;
        scaleFactor += 1.0 )
      {
        nonExpandedBlockHeight = (unsigned int)std::ceil( ((double)totalImageLines) / scaleFactor );
        nonExpandedBlockWidth = (unsigned int)std::ceil( ((double)totalImageCols) / scaleFactor );

        blocksHOverlapSize = (unsigned int)(((double)m_inputParameters.m_blocksOverlapPercent) *
          ((double)nonExpandedBlockWidth) / 100.0);
        blocksVOverlapSize = (unsigned int)(((double)m_inputParameters.m_blocksOverlapPercent) *
          ((double)nonExpandedBlockHeight) / 100.0);

        expandedBlockHeight = nonExpandedBlockHeight + blocksVOverlapSize +
          blocksVOverlapSize;
        expandedBlockWidth = nonExpandedBlockWidth + blocksHOverlapSize +
          blocksHOverlapSize;

        rescaledAndExpandedBlockPixelsNmb = expandedBlockHeight * expandedBlockWidth;

        if( rescaledAndExpandedBlockPixelsNmb <= maxExapandedBlockPixels )
        {
          return true;
        }
      }

      return false;
    }

    void MultiLevelSegmenter::segmenterThreadEntry( SegmenterThreadEntryParams* paramsPtr )
    {
      TERP_DEBUG_TRUE_OR_THROW( paramsPtr, "Invalid pointer" );
      TERP_DEBUG_TRUE_OR_THROW( paramsPtr->m_outputParametersPtr,
        "Invalid parameter" );
      TERP_DEBUG_TRUE_OR_THROW( paramsPtr->m_segsBlocksMatrixPtr,
        "Invalid parameter" );
      TERP_DEBUG_TRUE_OR_THROW( paramsPtr->m_generalMutexPtr,
        "Invalid parameter" );
      TERP_DEBUG_TRUE_OR_THROW( paramsPtr->m_inputRasterSyncPtr,
        "Invalid parameter" );
      TERP_DEBUG_TRUE_OR_THROW( paramsPtr->m_outputRasterSyncPtr,
        "Invalid parameter" );
      TERP_DEBUG_TRUE_OR_THROW( paramsPtr->m_blockProcessedSignalMutexPtr,
        "Invalid parameter" );
      TERP_DEBUG_TRUE_OR_THROW( paramsPtr->m_abortSegmentationFlagPtr,
        "Invalid parameter" );
      TERP_DEBUG_TRUE_OR_THROW( paramsPtr->m_segmentsIdsManagerPtr,
        "Invalid parameter" );
      TERP_DEBUG_TRUE_OR_THROW( paramsPtr->m_blockProcessedSignalPtr,
        "Invalid parameter" );
      TERP_DEBUG_TRUE_OR_THROW( paramsPtr->m_runningThreadsCounterPtr,
        "Invalid parameter" );
      TERP_DEBUG_TRUE_OR_THROW( !paramsPtr->m_inputRasterBandMinValues.empty(),
        "Invalid parameter" );
      TERP_DEBUG_TRUE_OR_THROW( !paramsPtr->m_inputRasterBandMaxValues.empty(),
        "Invalid parameter" );
      TERP_DEBUG_TRUE_OR_THROW( paramsPtr->m_maxInputRasterCachedBlocks,
        "Invalid parameter" );

      // Creating the input raster instance

      te::rst::SynchronizedRaster inputRaster( paramsPtr->m_maxInputRasterCachedBlocks,
        *(paramsPtr->m_inputRasterSyncPtr) );

      // Creating the segmentation strategy instance

      paramsPtr->m_generalMutexPtr->lock();

      boost::shared_ptr< te::rp::SegmenterStrategy > strategyPtr(
        te::rp::SegmenterStrategyFactory::make(
        paramsPtr->m_inputParameters.m_strategyName ) );
      TERP_TRUE_OR_THROW( strategyPtr.get(),
        "Unable to create an segmentation strategy" );
      if( !strategyPtr->initialize(
        paramsPtr->m_inputParameters.getSegStrategyParams() ) )
      {
        *(paramsPtr->m_runningThreadsCounterPtr) =
          *(paramsPtr->m_runningThreadsCounterPtr) - 1;
        *(paramsPtr->m_abortSegmentationFlagPtr) = true;

        //                std::cout << std::endl<< "Thread exit (error)"
        //                  << std::endl;                

        paramsPtr->m_generalMutexPtr->unlock();

        return;
      }
      else
      {
        paramsPtr->m_generalMutexPtr->unlock();
      }

      // Looking for a non processed segments block

      for( unsigned int sBMLine = 0; sBMLine <
        paramsPtr->m_segsBlocksMatrixPtr->getLinesNumber(); ++sBMLine )
      {
        for( unsigned int sBMCol = 0; sBMCol <
          paramsPtr->m_segsBlocksMatrixPtr->getColumnsNumber();  ++sBMCol )
        {
          if( *(paramsPtr->m_abortSegmentationFlagPtr) )
          {
            paramsPtr->m_generalMutexPtr->lock();

            *(paramsPtr->m_runningThreadsCounterPtr) =
              *(paramsPtr->m_runningThreadsCounterPtr) - 1;

            //            std::cout << std::endl<< "Thread exit (error)"
            //              << std::endl;

            paramsPtr->m_generalMutexPtr->unlock();

            return;
          }
          else
          {
            paramsPtr->m_generalMutexPtr->lock();

            te::rp::SegmenterSegmentsBlock& segsBlk =
              paramsPtr->m_segsBlocksMatrixPtr->operator()( sBMLine, sBMCol );

            if( segsBlk.m_status == te::rp::SegmenterSegmentsBlock::BlockNotProcessed )
            {
              //               TERP_LOGMSG( "Thread:" + boost::lexical_cast< std::string >( 
              //                 boost::this_thread::get_id() ) + " - Processing block:[" +
              //                 boost::lexical_cast< std::string >( sBMLine ) + "," +
              //                 boost::lexical_cast< std::string >( sBMCol ) + "]" );

              segsBlk.m_status = te::rp::SegmenterSegmentsBlock::BlockUnderSegmentation;
              /*
              std::cout << std::endl<< "Start block processing [" +
              te::common::Convert2String( sBMCol ) + "," +
              te::common::Convert2String( sBMLine ) + "]"
              << std::endl;
              */
              paramsPtr->m_generalMutexPtr->unlock();

              // Creating the output raster instance

              te::rst::SynchronizedRaster outputRaster( 1, *(paramsPtr->m_outputRasterSyncPtr) );

              // Executing the strategy

              if( !strategyPtr->execute(
                *paramsPtr->m_segmentsIdsManagerPtr,
                segsBlk,
                inputRaster,
                paramsPtr->m_inputParameters.m_inputRasterBands,
                paramsPtr->m_inputRasterNoDataValues,
                paramsPtr->m_inputRasterBandMinValues,
                paramsPtr->m_inputRasterBandMaxValues,
                outputRaster,
                0,
                paramsPtr->m_enableStrategyProgress ) )
              {
                paramsPtr->m_generalMutexPtr->lock();

                *(paramsPtr->m_runningThreadsCounterPtr) =
                  *(paramsPtr->m_runningThreadsCounterPtr) - 1;
                *(paramsPtr->m_abortSegmentationFlagPtr) = true;

                //                std::cout << std::endl<< "Thread exit (error)"
                //                  << std::endl;                

                paramsPtr->m_generalMutexPtr->unlock();

                return;
              }

              // updating block status

              paramsPtr->m_generalMutexPtr->lock();

              segsBlk.m_status = te::rp::SegmenterSegmentsBlock::BlockSegmented;
              /*
              std::cout << std::endl<< "Block processed [" +
              te::common::Convert2String( sBMCol ) + "," +
              te::common::Convert2String( sBMLine ) + "]"
              << std::endl;
              */
              paramsPtr->m_generalMutexPtr->unlock();

              // notifying the main thread with the block processed signal

              boost::lock_guard<boost::mutex> blockProcessedSignalLockGuard(
                *(paramsPtr->m_blockProcessedSignalMutexPtr) );

              paramsPtr->m_blockProcessedSignalPtr->notify_one();

            }
            else
            {
              paramsPtr->m_generalMutexPtr->unlock();
            }
          }
        }
      }

      // Destroying the strategy object

      strategyPtr.reset();

      // ending tasks

      paramsPtr->m_generalMutexPtr->lock();

      *(paramsPtr->m_runningThreadsCounterPtr) =
        *(paramsPtr->m_runningThreadsCounterPtr) - 1;

      //      std::cout << std::endl<< "Thread exit (OK)"
      //        << std::endl;        

      paramsPtr->m_generalMutexPtr->unlock();
    }

    bool MultiLevelSegmenter::genImageHCutOffProfile( const unsigned int profileCenter,
      const te::rst::Raster& inRaster,
      const std::vector< unsigned int >& inRasterBands,
      const unsigned int pixelNeighborhoodSize,
      const unsigned int tileNeighborhoodSize,
      const unsigned int profileAntiSmoothingFactor,
      std::vector< unsigned int>& profile ) const
    {
      profile.clear();

      if( profileAntiSmoothingFactor == 0 )
      {
        return false;
      }
      if( profileCenter >= inRaster.getNumberOfRows() )
      {
        return false;
      }
      if( tileNeighborhoodSize < pixelNeighborhoodSize )
      {
        return false;
      }

      const int inRasterRowsNmb = (int)inRaster.getNumberOfRows();
      const int inRasterColsNmb = (int)inRaster.getNumberOfColumns();
      const int inRasterBandsSize = (int)inRasterBands.size();

      // Calculating the tiles buffer dimension

      const int tilesBufferStartIdx = MAX( 0, MIN( inRasterRowsNmb - 1,
        ((int)profileCenter) - ((int)tileNeighborhoodSize) ) );
      const int tilesBufferBoundIdx = MAX( 0, MIN( inRasterRowsNmb,
        1 + ((int)profileCenter) + ((int)tileNeighborhoodSize) ) );
      const int tilesBufferSize = tilesBufferBoundIdx - tilesBufferStartIdx;
      if( tilesBufferSize < (1 + (2 * ((int)(pixelNeighborhoodSize)))) )
        return false;

      const int minCrossProfileStartIdx = tilesBufferStartIdx +
        (int)pixelNeighborhoodSize;
      const int maxCrossProfileBoundIdx = tilesBufferBoundIdx -
        (int)pixelNeighborhoodSize;

      int crossProfileIdx = 0;
      int crossProfileStartIdx = 0;
      int crossProfileBoundIdx = 0;

      int pixelNBStartIdx = 0;
      int pixelNBEndIdx = 0;
      int pixel1NBIdx = 0;
      int pixel2NBIdx = 0;
      int pixelNBOffset = 0;

      double diffSum = 0;
      double currBandDiffSum = 0;
      double higherDiffSum = 0;
      int higherDiffSumIdx = 0;

      int inRasterBandsIdx = 0;
      unsigned int bandIdx = 0;

      double pixel1Value = 0;
      double pixel2Value = 0;

      profile.resize( inRasterColsNmb, 0 );

      for( int profileElementIdx = 0; profileElementIdx < inRasterColsNmb;
        ++profileElementIdx )
      {
        if( profileElementIdx )
        {
          crossProfileStartIdx = profile[profileElementIdx - 1] -
            (int)profileAntiSmoothingFactor;
          crossProfileStartIdx = MAX( crossProfileStartIdx,
            minCrossProfileStartIdx );

          crossProfileBoundIdx = profile[profileElementIdx - 1] + 1 +
            ((int)profileAntiSmoothingFactor);
          crossProfileBoundIdx = MIN( crossProfileBoundIdx,
            maxCrossProfileBoundIdx );
        }
        else
        {
          crossProfileStartIdx = minCrossProfileStartIdx;
          crossProfileBoundIdx = maxCrossProfileBoundIdx;
        }

        higherDiffSum = 0;
        higherDiffSumIdx = crossProfileStartIdx;

        for( crossProfileIdx = crossProfileStartIdx; crossProfileIdx <
          crossProfileBoundIdx; ++crossProfileIdx )
        {
          // look for the higher diff using all bands
          // within the defined neighborhood

          diffSum = 0;

          for( inRasterBandsIdx = 0; inRasterBandsIdx < inRasterBandsSize;
            ++inRasterBandsIdx )
          {
            bandIdx = inRasterBands[inRasterBandsIdx];
            TERP_DEBUG_TRUE_OR_THROW( bandIdx < inRaster.getNumberOfBands(),
              "Invalid band" )

              pixelNBStartIdx = crossProfileIdx - ((int)pixelNeighborhoodSize);
            pixelNBEndIdx = crossProfileIdx + ((int)pixelNeighborhoodSize);
            currBandDiffSum = 0;

            for( pixelNBOffset = 0; pixelNBOffset < ((int)pixelNeighborhoodSize);
              ++pixelNBOffset )
            {
              pixel1NBIdx = pixelNBStartIdx + pixelNBOffset;
              pixel2NBIdx = pixelNBEndIdx - pixelNBOffset;

              TERP_DEBUG_TRUE_OR_THROW( (pixel1NBIdx >= 0) &&
                (pixel1NBIdx < inRasterRowsNmb), "Invalid pixel2Idx" )
                TERP_DEBUG_TRUE_OR_THROW( (pixel2NBIdx >= 0) &&
                (pixel2NBIdx < inRasterRowsNmb), "Invalid pixel2Idx" )

                inRaster.getValue( profileElementIdx,
                pixel1NBIdx, pixel1Value, bandIdx );
              inRaster.getValue( profileElementIdx,
                pixel2NBIdx, pixel2Value, bandIdx );

              currBandDiffSum += ABS( pixel1Value - pixel2Value );
            }

            diffSum += (currBandDiffSum / ((double)(pixelNBEndIdx -
              pixelNBStartIdx + 1)));
          }

          if( diffSum > higherDiffSum )
          {
            higherDiffSum = diffSum;
            higherDiffSumIdx = crossProfileIdx;
          }
        }

        profile[profileElementIdx] = higherDiffSumIdx;
      }

      return true;
    }

    bool MultiLevelSegmenter::genImageVCutOffProfile( const unsigned int profileCenter,
      const te::rst::Raster& inRaster,
      const std::vector< unsigned int >& inRasterBands,
      const unsigned int pixelNeighborhoodSize,
      const unsigned int tileNeighborhoodSize,
      const unsigned int profileAntiSmoothingFactor,
      std::vector< unsigned int>& profile ) const
    {
      profile.clear();

      if( profileAntiSmoothingFactor == 0 )
      {
        return false;
      }
      if( profileCenter >= inRaster.getNumberOfColumns() )
      {
        return false;
      }
      if( tileNeighborhoodSize < pixelNeighborhoodSize )
      {
        return false;
      }

      const int inRasterRowsNmb = (int)inRaster.getNumberOfRows();
      const int inRasterColsNmb = (int)inRaster.getNumberOfColumns();
      const int inRasterBandsSize = (int)inRasterBands.size();

      // Calculating the tiles buffer dimension

      const int tilesBufferStartIdx = MAX( 0, MIN( inRasterColsNmb - 1,
        ((int)profileCenter) - ((int)tileNeighborhoodSize) ) );
      const int tilesBufferBoundIdx = MAX( 0, MIN( inRasterColsNmb,
        1 + ((int)profileCenter) + ((int)tileNeighborhoodSize) ) );
      const int tilesBufferSize = tilesBufferBoundIdx - tilesBufferStartIdx;
      if( tilesBufferSize < (1 + (2 * ((int)(pixelNeighborhoodSize)))) )
        return false;

      const int minCrossProfileStartIdx = tilesBufferStartIdx +
        (int)pixelNeighborhoodSize;
      const int maxCrossProfileBoundIdx = tilesBufferBoundIdx -
        (int)pixelNeighborhoodSize;

      int crossProfileIdx = 0;
      int crossProfileStartIdx = 0;
      int crossProfileBoundIdx = 0;

      int pixelNBStartIdx = 0;
      int pixelNBEndIdx = 0;
      int pixel1NBIdx = 0;
      int pixel2NBIdx = 0;
      int pixelNBOffset = 0;

      double diffSum = 0;
      double currBandDiffSum = 0;
      double higherDiffSum = 0;
      int higherDiffSumIdx = 0;

      int inRasterBandsIdx = 0;
      unsigned int bandIdx = 0;

      double pixel1Value = 0;
      double pixel2Value = 0;

      profile.resize( inRasterRowsNmb, 0 );

      for( int profileElementIdx = 0; profileElementIdx < inRasterRowsNmb;
        ++profileElementIdx )
      {
        if( profileElementIdx )
        {
          crossProfileStartIdx = profile[profileElementIdx - 1] -
            (int)profileAntiSmoothingFactor;
          crossProfileStartIdx = MAX( crossProfileStartIdx,
            minCrossProfileStartIdx );

          crossProfileBoundIdx = profile[profileElementIdx - 1] + 1 +
            ((int)profileAntiSmoothingFactor);
          crossProfileBoundIdx = MIN( crossProfileBoundIdx,
            maxCrossProfileBoundIdx );
        }
        else
        {
          crossProfileStartIdx = minCrossProfileStartIdx;
          crossProfileBoundIdx = maxCrossProfileBoundIdx;
        }

        higherDiffSum = 0;
        higherDiffSumIdx = crossProfileStartIdx;

        for( crossProfileIdx = crossProfileStartIdx; crossProfileIdx <
          crossProfileBoundIdx; ++crossProfileIdx )
        {
          // look for the higher diff using all bands
          // within the defined neighborhood

          diffSum = 0;

          for( inRasterBandsIdx = 0; inRasterBandsIdx < inRasterBandsSize;
            ++inRasterBandsIdx )
          {
            bandIdx = inRasterBands[inRasterBandsIdx];
            TERP_DEBUG_TRUE_OR_THROW( bandIdx < inRaster.getNumberOfBands(),
              "Invalid band" )

              pixelNBStartIdx = crossProfileIdx - ((int)pixelNeighborhoodSize);
            pixelNBEndIdx = crossProfileIdx + ((int)pixelNeighborhoodSize);
            currBandDiffSum = 0;

            for( pixelNBOffset = 0; pixelNBOffset < ((int)pixelNeighborhoodSize);
              ++pixelNBOffset )
            {
              pixel1NBIdx = pixelNBStartIdx + pixelNBOffset;
              pixel2NBIdx = pixelNBEndIdx - pixelNBOffset;

              TERP_DEBUG_TRUE_OR_THROW( (pixel1NBIdx >= 0) &&
                (pixel1NBIdx < inRasterColsNmb), "Invalid pixel2Idx" )
                TERP_DEBUG_TRUE_OR_THROW( (pixel2NBIdx >= 0) &&
                (pixel2NBIdx < inRasterColsNmb), "Invalid pixel2Idx" )

                inRaster.getValue( pixel1NBIdx, profileElementIdx,
                pixel1Value, bandIdx );
              inRaster.getValue( pixel2NBIdx, profileElementIdx,
                pixel2Value, bandIdx );

              currBandDiffSum += ABS( pixel1Value - pixel2Value );
            }

            diffSum += (currBandDiffSum / ((double)(pixelNBEndIdx -
              pixelNBStartIdx + 1)));
          }

          if( diffSum > higherDiffSum )
          {
            higherDiffSum = diffSum;
            higherDiffSumIdx = crossProfileIdx;
          }
        }

        profile[profileElementIdx] = higherDiffSumIdx;
      }

      return true;
    }

    bool MultiLevelSegmenter::updateBlockCutOffProfiles(
      const std::vector< std::vector< unsigned int> >& imageHorizontalProfiles,
      const std::vector< std::vector< unsigned int> >& imageVerticalProfiles,
      te::rp::SegmenterSegmentsBlock& segmentsBlock ) const
    {
      if( segmentsBlock.m_segmentsMatrixXIndex > imageVerticalProfiles.size() )
      {
        return false;
      }
      if( segmentsBlock.m_segmentsMatrixYIndex > imageHorizontalProfiles.size() )
      {
        return false;
      }

      // generating m_topCutOffProfile

      segmentsBlock.m_topCutOffProfile.clear();

      if( (segmentsBlock.m_segmentsMatrixYIndex > 0) &&
        (segmentsBlock.m_segmentsMatrixYIndex <=
        imageHorizontalProfiles.size()) )
      {
        const std::vector< unsigned int >& profile = imageHorizontalProfiles[
          segmentsBlock.m_segmentsMatrixYIndex - 1];

          if( profile.empty() )
          {
            return false;
          }
          else
          {
            segmentsBlock.m_topCutOffProfile.resize( segmentsBlock.m_width );
            int profElementValue = 0;

            for( unsigned int profEIdx = 0; profEIdx < segmentsBlock.m_width;
              ++profEIdx )
            {
              if( profEIdx + segmentsBlock.m_startX >= profile.size() )
              {
                return false;
              }

              profElementValue =
                ((int)profile[profEIdx + segmentsBlock.m_startX]) -
                ((int)segmentsBlock.m_startY);
              profElementValue = MAX( 0, profElementValue );
              profElementValue = MIN( (int)segmentsBlock.m_height, profElementValue );

              segmentsBlock.m_topCutOffProfile[profEIdx] = profElementValue;
            }
          }
      }
      else
      {
        segmentsBlock.m_topCutOffProfile.resize( segmentsBlock.m_width, 0 );
      }

      // generating m_bottomCutOffProfile

      segmentsBlock.m_bottomCutOffProfile.clear();

      if( segmentsBlock.m_segmentsMatrixYIndex <
        imageHorizontalProfiles.size() )
      {
        const std::vector< unsigned int >& profile = imageHorizontalProfiles[
          segmentsBlock.m_segmentsMatrixYIndex];

          if( profile.empty() )
          {
            return false;
          }
          else
          {
            segmentsBlock.m_bottomCutOffProfile.resize( segmentsBlock.m_width );
            int profElementValue = 0;

            for( unsigned int profEIdx = 0; profEIdx < segmentsBlock.m_width;
              ++profEIdx )
            {
              if( profEIdx + segmentsBlock.m_startX >= profile.size() )
              {
                return false;
              }

              profElementValue =
                ((int)profile[profEIdx + segmentsBlock.m_startX]) -
                ((int)segmentsBlock.m_startY);
              profElementValue = MAX( 0, profElementValue );
              profElementValue = MIN( (int)segmentsBlock.m_height, profElementValue );

              segmentsBlock.m_bottomCutOffProfile[profEIdx] = profElementValue;
            }
          }
      }
      else
      {
        segmentsBlock.m_bottomCutOffProfile.resize( segmentsBlock.m_width,
          segmentsBlock.m_height - 1 );
      }

      // generating m_leftCutOffProfile

      segmentsBlock.m_leftCutOffProfile.clear();

      if( (segmentsBlock.m_segmentsMatrixXIndex > 0) &&
        (segmentsBlock.m_segmentsMatrixXIndex <=
        imageVerticalProfiles.size()) )
      {
        const std::vector< unsigned int >& profile = imageVerticalProfiles[
          segmentsBlock.m_segmentsMatrixXIndex - 1];

          if( profile.empty() )
          {
            return false;
          }
          else
          {
            segmentsBlock.m_leftCutOffProfile.resize( segmentsBlock.m_height );
            int profElementValue = 0;

            for( unsigned int profEIdx = 0; profEIdx < segmentsBlock.m_height;
              ++profEIdx )
            {
              if( profEIdx + segmentsBlock.m_startY >= profile.size() )
              {
                return false;
              }

              profElementValue =
                ((int)profile[profEIdx + segmentsBlock.m_startY]) -
                ((int)segmentsBlock.m_startX);
              profElementValue = MAX( 0, profElementValue );
              profElementValue = MIN( (int)segmentsBlock.m_width, profElementValue );

              segmentsBlock.m_leftCutOffProfile[profEIdx] = profElementValue;
            }
          }
      }
      else
      {
        segmentsBlock.m_leftCutOffProfile.resize( segmentsBlock.m_height, 0 );
      }

      // generating m_rightCutOffProfile

      segmentsBlock.m_rightCutOffProfile.clear();

      if( segmentsBlock.m_segmentsMatrixXIndex <
        imageVerticalProfiles.size() )
      {
        const std::vector< unsigned int >& profile = imageVerticalProfiles[
          segmentsBlock.m_segmentsMatrixXIndex];

          if( profile.empty() )
          {
            return false;
          }
          else
          {
            segmentsBlock.m_rightCutOffProfile.resize( segmentsBlock.m_height );
            int profElementValue = 0;

            for( unsigned int profEIdx = 0; profEIdx < segmentsBlock.m_height;
              ++profEIdx )
            {
              if( profEIdx + segmentsBlock.m_startY >= profile.size() )
              {
                return false;
              }

              profElementValue =
                ((int)profile[profEIdx + segmentsBlock.m_startY]) -
                ((int)segmentsBlock.m_startX);
              profElementValue = MAX( 0, profElementValue );
              profElementValue = MIN( (int)segmentsBlock.m_width, profElementValue );

              segmentsBlock.m_rightCutOffProfile[profEIdx] = profElementValue;
            }
          }
      }
      else
      {
        segmentsBlock.m_rightCutOffProfile.resize( segmentsBlock.m_height,
          segmentsBlock.m_width - 1 );
      }

      assert( segmentsBlock.m_topCutOffProfile.size() == segmentsBlock.m_width );
      assert( segmentsBlock.m_bottomCutOffProfile.size() == segmentsBlock.m_width );
      assert( segmentsBlock.m_leftCutOffProfile.size() == segmentsBlock.m_height );
      assert( segmentsBlock.m_rightCutOffProfile.size() == segmentsBlock.m_height );

      return true;
    }

    bool MultiLevelSegmenter::createCutOffLinesTiff(
      const std::vector< std::vector< unsigned int> >& imageHorizontalProfiles,
      const std::vector< unsigned int >& imageHorizontalProfilesCenterLines,
      const std::vector< std::vector< unsigned int> >& imageVerticalProfiles,
      const std::vector< unsigned int >& imageVerticalProfilesCenterLines,
      const std::string& filename )
    {
      TERP_TRUE_OR_THROW( imageHorizontalProfiles.size() ==
        imageHorizontalProfilesCenterLines.size(), "Internal error" )
        TERP_TRUE_OR_THROW( imageVerticalProfiles.size() ==
        imageVerticalProfilesCenterLines.size(), "Internal error" )

        std::vector< te::rst::BandProperty* > bandsProperties;
      bandsProperties.push_back( new te::rst::BandProperty(
        *(m_inputParameters.m_inputRasterPtr->getBand(
        m_inputParameters.m_inputRasterBands[0] )->getProperty()) ) );
      bandsProperties[0]->m_colorInterp = te::rst::GrayIdxCInt;
      bandsProperties[0]->m_noDataValue = 0;
      bandsProperties[0]->m_type = te::dt::UCHAR_TYPE;
      //bandsProperties[ 0 ]->m_noDataValue = 0;

      std::map< std::string, std::string > rInfo;
      rInfo["URI"] = "cutofflines.tif";

      std::auto_ptr< te::rst::Raster > outRasterPtr(
        te::rst::RasterFactory::make( "GDAL",
        new te::rst::Grid( *m_inputParameters.m_inputRasterPtr->getGrid() ),
        bandsProperties, rInfo ) );
      TERP_TRUE_OR_RETURN_FALSE( outRasterPtr.get(),
        "Tiff creation error" )

      {
        for( unsigned int profIdx = 0; profIdx < imageHorizontalProfiles.size();
          ++profIdx )
        {
          const std::vector< unsigned int>& profile =
            imageHorizontalProfiles[profIdx];
          TERP_TRUE_OR_THROW( profile.size() ==
            outRasterPtr->getGrid()->getNumberOfColumns(), "Internal error" )

            const unsigned int centerLineIdx =
            imageHorizontalProfilesCenterLines[profIdx];
          TERP_TRUE_OR_THROW( centerLineIdx <
            outRasterPtr->getGrid()->getNumberOfRows(), "Internal error" )


          for( unsigned int eIdx = 0; eIdx < profile.size();
            ++eIdx )
          {
            TERP_TRUE_OR_THROW( profile[eIdx] <
              outRasterPtr->getGrid()->getNumberOfRows(), "Internal error" )

              outRasterPtr->setValue( eIdx, centerLineIdx, 255, 0 );
            outRasterPtr->setValue( eIdx, profile[eIdx], 255, 0 );
          }
        }
      }

      {
        for( unsigned int profIdx = 0; profIdx < imageVerticalProfiles.size();
          ++profIdx )
        {
          const std::vector< unsigned int>& profile =
            imageVerticalProfiles[profIdx];
          TERP_TRUE_OR_THROW( profile.size() ==
            outRasterPtr->getGrid()->getNumberOfRows(), "Internal error" )

            const unsigned int centerLineIdx =
            imageVerticalProfilesCenterLines[profIdx];
          TERP_TRUE_OR_THROW( centerLineIdx <
            outRasterPtr->getGrid()->getNumberOfColumns(), "Internal error" )

          for( unsigned int eIdx = 0; eIdx < profile.size();
            ++eIdx )
          {
            TERP_TRUE_OR_THROW( profile[eIdx] <
              outRasterPtr->getGrid()->getNumberOfRows(), "Internal error" )

              outRasterPtr->setValue( centerLineIdx, eIdx, 255, 0 );
            outRasterPtr->setValue( profile[eIdx], eIdx, 255, 0 );
          }
        }
      }

      return true;
    }
  }
}