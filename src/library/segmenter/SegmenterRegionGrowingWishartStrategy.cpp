/*  Copyright (C) 2015 National Institute For Space Research (INPE) - Brazil.

This file is part of the TerraRadar - a library and application for radar data manipulation.

TerraRadar is under development.
*/

/*!
  \file terraradar/library/segmenter/SegmenterRegionGrowingWishartStrategy.cpp
  \briefRaster region growing segmenter Wishart strategy.
*/

#include "SegmenterRegionGrowingWishartStrategy.hpp"
#include "SegmenterRegionGrowingWishartMerger.hpp"

#include <terralib/common/progress/TaskProgress.h>

/*
namespace
{
  static teradar::segmenter::SegmenterRegionGrowingWishartStrategyFactory
    segmenterRegionGrowingWishartStrategyFactoryInstance;
}
*/

namespace teradar {
  namespace segmenter {
      // Parameters
      SegmenterRegionGrowingWishartStrategy::Parameters::Parameters()
      {
        reset();
      }

      SegmenterRegionGrowingWishartStrategy::Parameters::~Parameters()
      {
      }

      const SegmenterRegionGrowingWishartStrategy::Parameters&
        SegmenterRegionGrowingWishartStrategy::Parameters::operator=(
        const SegmenterRegionGrowingWishartStrategy::Parameters& params )
      {
        reset();
        
        m_minSegmentSize = params.m_minSegmentSize;
        m_dataType = params.m_dataType;
        m_enableAzimutalSimetry = params.m_enableAzimutalSimetry;
        m_enlLZero = params.m_enlLZero;
        m_compressionLevel = params.m_compressionLevel;
        m_connectivityType = params.m_connectivityType;
        m_regionGrowingLimit = params.m_regionGrowingLimit;
        m_regionGrowingConfLevel = params.m_regionGrowingConfLevel;
        m_regionMergingLimit = params.m_regionMergingLimit;
        m_regionMergingConfLevel = params.m_regionMergingConfLevel;
        
        /*
        m_segmentsSimilarityThreshold = params.m_segmentsSimilarityThreshold;
        m_segmentsSimIncreaseSteps = params.m_segmentsSimIncreaseSteps;
        m_enableLocalMutualBestFitting = params.m_enableLocalMutualBestFitting;
        m_enableSameIterationMerges = params.m_enableSameIterationMerges;
        */

        return *this;
      }

      void SegmenterRegionGrowingWishartStrategy::Parameters::reset()
        throw(te::rp::Exception)
      {
        m_minSegmentSize = 100;
        m_dataType = teradar::common::CovarianceMatrixT;
        m_enableAzimutalSimetry = false;
        m_enlLZero = 1.583;
        m_compressionLevel = 1;
        m_connectivityType = teradar::common::VonNeumannNT;
        m_regionGrowingLimit = 15;
        m_regionGrowingConfLevel = 99.9;
        m_regionMergingLimit = 1;
        m_regionMergingConfLevel = 99.9;

        /*
        m_segmentsSimilarityThreshold = 0.05;
        m_segmentsSimIncreaseSteps = 2;
        m_enableLocalMutualBestFitting = false;
        m_enableSameIterationMerges = false;
        */
      }

      te::common::AbstractParameters* SegmenterRegionGrowingWishartStrategy::Parameters::clone() const
      {
        return new teradar::segmenter::SegmenterRegionGrowingWishartStrategy::Parameters( *this );
      }
      
      // SegmenterRegionGrowingStrategy
      SegmenterRegionGrowingWishartStrategy::SegmenterRegionGrowingWishartStrategy()
      {
        m_isInitialized = false;
      }

      SegmenterRegionGrowingWishartStrategy::~SegmenterRegionGrowingWishartStrategy()
      {
        reset();
      }

      bool SegmenterRegionGrowingWishartStrategy::initialize( te::rp::SegmenterStrategyParameters const* const strategyParams )
        throw(te::rp::Exception)
      {
        m_isInitialized = false;
        reset();

        SegmenterRegionGrowingWishartStrategy::Parameters const* paramsPtr =
          dynamic_cast<SegmenterRegionGrowingWishartStrategy::Parameters const*>(strategyParams);

        if( paramsPtr ) {
          m_parameters = *(paramsPtr);

          TERP_TRUE_OR_RETURN_FALSE( m_parameters.m_minSegmentSize > 0,
            "Invalid segmenter strategy parameter m_minSegmentSize" );
          
          TERP_TRUE_OR_RETURN_FALSE( m_parameters.m_enlLZero > 1,
            "Invalid segmenter strategy parameter m_enlLZero" );

          TERP_TRUE_OR_RETURN_FALSE( m_parameters.m_compressionLevel > 0,
            "Invalid segmenter strategy parameter m_compressionLevel" );

          TERP_TRUE_OR_RETURN_FALSE( m_parameters.m_regionGrowingConfLevel > 0,
            "Invalid segmenter strategy parameter m_regionGrowingConfLevel" );

          TERP_TRUE_OR_RETURN_FALSE( m_parameters.m_regionGrowingConfLevel <= 100,
            "Invalid segmenter strategy parameter m_regionGrowingConfLevel" );

          TERP_TRUE_OR_RETURN_FALSE( m_parameters.m_regionGrowingLimit > 0,
            "Invalid segmenter strategy parameter m_regionGrowingLimit" );
          
          TERP_TRUE_OR_RETURN_FALSE( m_parameters.m_regionMergingConfLevel > 0,
            "Invalid segmenter strategy parameter m_regionMergingConfLevel" );

          TERP_TRUE_OR_RETURN_FALSE( m_parameters.m_regionMergingConfLevel <= 100,
            "Invalid segmenter strategy parameter m_regionMergingConfLevel" );

          TERP_TRUE_OR_RETURN_FALSE( m_parameters.m_regionMergingLimit > 0,
            "Invalid segmenter strategy parameter m_regionMergingLimit" );
          
          /*
          TERP_TRUE_OR_RETURN_FALSE( m_parameters.m_segmentsSimilarityThreshold >= 0.0 ,
            "Invalid segmenter strategy parameter m_segmentsSimilarityThreshold" )
          */
                    
          m_isInitialized = true;

          return true;
        } else {
          return false;
        }
      }

      void SegmenterRegionGrowingWishartStrategy::reset()
      {
        m_isInitialized = false;
        m_segmentsPool.clear();
        m_segmentsIdsMatrix.reset();
        m_parameters.reset();
      }

      bool SegmenterRegionGrowingWishartStrategy::execute(
        te::rp::SegmenterIdsManager& segmenterIdsManager,
        const te::rp::SegmenterSegmentsBlock& block2ProcessInfo,
        const te::rst::Raster& inputRaster,
        const std::vector< unsigned int >& inputRasterBands,
        const std::vector< std::complex< double > >& inputRasterNoDataValues,
        const std::vector< std::complex< double > >& inputRasterBandMinValues,
        const std::vector< std::complex< double > >& inputRasterBandMaxValues,
        te::rst::Raster& outputRaster,
        const unsigned int outputRasterBand,
        const bool enableProgressInterface )
        throw(te::rp::Exception)
      {
        TERP_TRUE_OR_RETURN_FALSE( m_isInitialized, "Instance not initialized" );

        unsigned int featuresNumber = (unsigned int)inputRasterBands.size( );

        // Creating the merger instance
        std::auto_ptr< SegmenterRegionGrowingWishartMerger >
          mergerPtr( new SegmenterRegionGrowingWishartMerger( featuresNumber, m_parameters.m_enlLZero ) );

        // Initiating the segments pool
        const unsigned int segmentFeaturesSize = mergerPtr->getSegmentFeaturesSize( );

        // The number of segments plus 3 (due 3 auxiliary segments)
        TERP_TRUE_OR_RETURN_FALSE( m_segmentsPool.initialize( 3 + (block2ProcessInfo.m_height * block2ProcessInfo.m_width),
          segmentFeaturesSize ), "Segments pool initiation error" );

        te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType >* auxSeg1Ptr = m_segmentsPool.getNextSegment( );
        auxSeg1Ptr->disable( );
        te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType >* auxSeg2Ptr = m_segmentsPool.getNextSegment( );
        auxSeg2Ptr->disable( );
        te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType >* auxSeg3Ptr = m_segmentsPool.getNextSegment( );
        auxSeg3Ptr->disable( );

        // Allocating the ids matrix

        if( (m_segmentsIdsMatrix.getLinesNumber( ) != block2ProcessInfo.m_height) ||
          (m_segmentsIdsMatrix.getColumnsNumber( ) != block2ProcessInfo.m_width) ) {
          TERP_TRUE_OR_RETURN_FALSE( m_segmentsIdsMatrix.reset( block2ProcessInfo.m_height, block2ProcessInfo.m_width,
            te::rp::Matrix< te::rp::SegmenterSegmentsBlock::SegmentIdDataType >::RAMMemPol ),
            "Error allocating segments Ids matrix" );
        }

        // Initializing segments
        te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType >* actSegsListHeadPtr = 0;

        TERP_TRUE_OR_RETURN_FALSE( initializeSegments( segmenterIdsManager,
          block2ProcessInfo, inputRaster, inputRasterBands, &actSegsListHeadPtr ),
          "Segments initalization error" );

        TERP_TRUE_OR_RETURN_FALSE( actSegsListHeadPtr != 0, "Invalid active segments list header" );


        // @todo - etore - complete
        // Progress interface
        /*
        std::auto_ptr< te::common::TaskProgress > progressPtr;
        if( enableProgressInterface ) {
          progressPtr.reset( new te::common::TaskProgress );
          progressPtr->setTotalSteps( 2 + m_parameters.m_segmentsSimIncreaseSteps );
          progressPtr->setMessage( "Segmentation" );
        }
        */






        /* 
        
        // Globals
        te::rp::DissimilarityTypeT minFoundDissimilarity = 0.0;
        te::rp::DissimilarityTypeT maxFoundDissimilarity = 0.0;
        
        unsigned int totalMergesNumber = 0;
        te::rp::rg::IterationCounterType globalMergeIterationsCounter = 1;

        // STEP 1 - Merge of segments that are equal
        if( enableProgressInterface ) {
          if( !progressPtr->isActive() ) {
            return false;
          }
          progressPtr->pulse();
        }

        //TERP_LOGMSG( m_parameters.m_segmentsSimilarityThreshold );
        //TERP_LOGMSG( rg::getActiveSegmentsNumber< rg::WishartFeatureType >( actSegsListHeadPtr ) );
        
        te::rp::rg::mergeSegments< te::rp::rg::WishartFeatureType >(
          m_segmentsIdsMatrix,
          0.0,
          0,
          segmenterIdsManager,
          *mergerPtr,
          false,
          true,
          auxSeg1Ptr,
          auxSeg2Ptr,
          auxSeg3Ptr,
          minFoundDissimilarity,
          maxFoundDissimilarity,
          totalMergesNumber,
          globalMergeIterationsCounter,
          &actSegsListHeadPtr );

        //TERP_LOGMSG( rg::getActiveSegmentsNumber< rg::WishartFeatureType >( actSegsListHeadPtr ) );

        // STEP 2 - Main Segmentation loop
        for( unsigned int segmentsSimIncreaseStep = 1; segmentsSimIncreaseStep <= m_parameters.m_segmentsSimIncreaseSteps; ++segmentsSimIncreaseStep ) {
          const te::rp::DissimilarityTypeT disimilarityThreshold =
            (te::rp::DissimilarityTypeT)segmentsSimIncreaseStep
            *
            (te::rp::DissimilarityTypeT)m_parameters.m_segmentsSimilarityThreshold
            /
            (te::rp::DissimilarityTypeT)m_parameters.m_segmentsSimIncreaseSteps;

          //TERP_LOGMSG( segmentsSimIncreaseStep );
          //TERP_LOGMSG( disimilarityThreshold )

          te::rp::rg::mergeSegments< te::rp::rg::WishartFeatureType >(
            m_segmentsIdsMatrix,
            disimilarityThreshold,
            0,
            segmenterIdsManager,
            *mergerPtr,
            m_parameters.m_enableLocalMutualBestFitting,
            m_parameters.m_enableSameIterationMerges,
            auxSeg1Ptr,
            auxSeg2Ptr,
            auxSeg3Ptr,
            minFoundDissimilarity,
            maxFoundDissimilarity,
            totalMergesNumber,
            globalMergeIterationsCounter,
            &actSegsListHeadPtr );

          //TERP_LOGMSG( rg::getActiveSegmentsNumber< rg::WishartFeatureType >( actSegsListHeadPtr ) );

          if( enableProgressInterface ) {
            if( !progressPtr->isActive() ) {
              return false;
            }
            progressPtr->pulse();
          }
        }
        
        // STEP 3 - Forcing the merge of too small segments
        if( m_parameters.m_minSegmentSize > 1 ) {
          te::rp::rg::mergeSegments< te::rp::rg::WishartFeatureType >(
            m_segmentsIdsMatrix,
            std::numeric_limits< te::rp::DissimilarityTypeT >::max( ),
            m_parameters.m_minSegmentSize,
            segmenterIdsManager,
            *mergerPtr,
            false,
            true,
            auxSeg1Ptr,
            auxSeg2Ptr,
            auxSeg3Ptr,
            minFoundDissimilarity,
            maxFoundDissimilarity,
            totalMergesNumber,
            globalMergeIterationsCounter,
            &actSegsListHeadPtr );

          //TERP_LOGMSG( rg::getActiveSegmentsNumber< rg::WishartFeatureType >( actSegsListHeadPtr ) );
        }

        if( enableProgressInterface ) {
          if( !progressPtr->isActive() ) {
            return false;
          }
          progressPtr->pulse();
        }

        // Flush result to the output raster
        {
          unsigned int blkCol = 0;
          te::rp::SegmenterSegmentsBlock::SegmentIdDataType* segmentsIdsLinePtr = 0;

          for( unsigned int blkLine = 0; blkLine < block2ProcessInfo.m_height; ++blkLine ) {
            segmentsIdsLinePtr = m_segmentsIdsMatrix[blkLine];

            for( blkCol = 0; blkCol < block2ProcessInfo.m_width; ++blkCol ) {
              if( segmentsIdsLinePtr[blkCol] ) {
                outputRaster.setValue( blkCol + block2ProcessInfo.m_startX, 
                  blkLine + block2ProcessInfo.m_startY, segmentsIdsLinePtr[blkCol],
                  outputRasterBand );
              }
            }
          }
        }
        */        
        return true;
      }

      double SegmenterRegionGrowingWishartStrategy::getMemUsageEstimation(
        const unsigned int bandsToProcess,
        const unsigned int pixelsNumber ) const
      {
        // @todo - etore - complete

        TERP_TRUE_OR_THROW( m_isInitialized, "Instance not initialized" );

        // The features matrix inside the pool
        double featuresSizeBytes = (double)(pixelsNumber * bandsToProcess * sizeof(te::rp::rg::WishartFeatureType));

        return (double)(featuresSizeBytes + (pixelsNumber * (sizeof(te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType >)
          + (6 * sizeof(te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType >*))))
          + (pixelsNumber * sizeof(te::rp::SegmenterSegmentsBlock::SegmentIdDataType)));
      }

      unsigned int SegmenterRegionGrowingWishartStrategy::getOptimalBlocksOverlapSize() const
      {
        return 0;
      }

      bool SegmenterRegionGrowingWishartStrategy::shouldComputeMinMaxValues() const
      {
        return false;
      }

      te::rp::SegmenterStrategy::BlocksMergingMethod
        SegmenterRegionGrowingWishartStrategy::getBlocksMergingMethod() const
      {
        return SegmenterStrategy::NoMerging;
      }

      bool SegmenterRegionGrowingWishartStrategy::initializeSegments( te::rp::SegmenterIdsManager& segmenterIdsManager,
        const te::rp::SegmenterSegmentsBlock& block2ProcessInfo,
        const te::rst::Raster& inputRaster,
        const std::vector< unsigned int >& inputRasterBands,
        te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType >** actSegsListHeadPtr )
      {
        const unsigned int inputRasterBandsSize = (unsigned int)inputRasterBands.size();

        (*actSegsListHeadPtr) = 0;

        // Initializing each segment
        unsigned int blkLine = 0;
        unsigned int blkCol = 0;
        te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType >* segmentPtr = 0;
        te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType >* neighborSegmentPtr = 0;
        bool rasterValuesAreValid = true;
        unsigned int inputRasterBandsIdx = 0;
        std::complex< double > value = 0;
        
        const std::vector< std::complex< double > > dummyZeroesVector( inputRasterBandsSize, 0 );

        std::list< te::rp::SegmenterSegmentsBlock::SegmentIdDataType > unusedLineSegmentIds;

        std::vector< te::rp::SegmenterSegmentsBlock::SegmentIdDataType > lineSegmentIds;
        lineSegmentIds.reserve( block2ProcessInfo.m_width );

        std::vector< te::rp::rg::WishartFeatureType > rasterValues;
        rasterValues.resize( inputRasterBandsSize, 0 );

        std::vector< te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType >* > usedSegPointers1( block2ProcessInfo.m_width, 0 );
        std::vector< te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType >* > usedSegPointers2( block2ProcessInfo.m_width, 0 );
        std::vector< te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType >* >* lastLineSegsPtrs = &usedSegPointers1;
        std::vector< te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType >* >* currLineSegsPtrs = &usedSegPointers2;

        te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType >* prevActSegPtr = 0;

        unsigned int rasterValuesIdx = 0;

        for( blkLine = 0; blkLine < block2ProcessInfo.m_height; ++blkLine ) {
          segmenterIdsManager.getNewIDs( block2ProcessInfo.m_width, lineSegmentIds );
          
          for( blkCol = 0; blkCol < block2ProcessInfo.m_width; ++blkCol )  {

            if( (blkLine >= block2ProcessInfo.m_topCutOffProfile[blkCol])
              && (blkLine <= block2ProcessInfo.m_bottomCutOffProfile[blkCol])
              && (blkCol >= block2ProcessInfo.m_leftCutOffProfile[blkLine])
              && (blkCol <= block2ProcessInfo.m_rightCutOffProfile[blkLine]) ) {

              rasterValuesAreValid = true;

              for( inputRasterBandsIdx = 0; inputRasterBandsIdx < inputRasterBandsSize; ++inputRasterBandsIdx ) {
                inputRaster.getValue( blkCol + block2ProcessInfo.m_startX, 
                  blkLine + block2ProcessInfo.m_startY, 
                  value,
                  inputRasterBands[inputRasterBandsIdx] );

                rasterValues[inputRasterBandsIdx] = (te::rp::rg::WishartFeatureType) value;
              }
            } else {
              rasterValuesAreValid = false;
            }
            
            // assotiating a segment object
            if( rasterValuesAreValid ) {
              segmentPtr = m_segmentsPool.getNextSegment();
              assert( segmentPtr );

              for( rasterValuesIdx = 0; rasterValuesIdx < inputRasterBandsSize; ++rasterValuesIdx ) {
                segmentPtr->m_features[rasterValuesIdx] = rasterValues[rasterValuesIdx];
              }

              currLineSegsPtrs->operator[]( blkCol ) = segmentPtr;

              segmentPtr->m_id = lineSegmentIds[blkCol];
              segmentPtr->m_size = 1;
              segmentPtr->m_xStart = blkCol;
              segmentPtr->m_xBound = blkCol + 1;
              segmentPtr->m_yStart = blkLine;
              segmentPtr->m_yBound = blkLine + 1;
              segmentPtr->m_mergetIteration = 0;
              segmentPtr->m_prevActiveSegment = prevActSegPtr;
              segmentPtr->m_nextActiveSegment = 0;

              m_segmentsIdsMatrix( blkLine, blkCol ) = segmentPtr->m_id;
              
              // updating the neighboorhood info
              segmentPtr->removeAllNeighborSegmentsPtrs();

              if( blkLine ) {
                neighborSegmentPtr = lastLineSegsPtrs->operator[]( blkCol );

                if( neighborSegmentPtr ) {
                  segmentPtr->addNeighborSegmentPtr( neighborSegmentPtr );
                  neighborSegmentPtr->addNeighborSegmentPtr( segmentPtr );
                }
              }

              if( blkCol ) {
                neighborSegmentPtr = currLineSegsPtrs->operator[]( blkCol - 1 );

                if( neighborSegmentPtr ) {
                  segmentPtr->addNeighborSegmentPtr( neighborSegmentPtr );
                  neighborSegmentPtr->addNeighborSegmentPtr( segmentPtr );
                }
              }

              // Updating the active segments list header
              if( (*actSegsListHeadPtr) == 0 ) {
                (*actSegsListHeadPtr) = segmentPtr;
              }

              // Updating the previous active segment
              if( prevActSegPtr ) {
                prevActSegPtr->m_nextActiveSegment = segmentPtr;
              }

              prevActSegPtr = segmentPtr;
            } else { // !rasterValueIsValid
              m_segmentsIdsMatrix( blkLine, blkCol ) = 0;
              unusedLineSegmentIds.push_back( lineSegmentIds[blkCol] );
              currLineSegsPtrs->operator[]( blkCol ) = 0;
            }
          }
          
          // Free unused IDs
          if( !unusedLineSegmentIds.empty() ) {
            segmenterIdsManager.addFreeIDs( unusedLineSegmentIds );
            unusedLineSegmentIds.clear();
          }

          // Swapping the pointers to the vectors of used segment pointers
          if( lastLineSegsPtrs == (&usedSegPointers1) ) {
            lastLineSegsPtrs = &usedSegPointers2;
            currLineSegsPtrs = &usedSegPointers1;
          } else {
            lastLineSegsPtrs = &usedSegPointers1;
            currLineSegsPtrs = &usedSegPointers2;
          }
        }

        return true;
      }
      
      // Factory
      SegmenterRegionGrowingWishartStrategyFactory::SegmenterRegionGrowingWishartStrategyFactory()
        : te::rp::SegmenterStrategyFactory( "RegionGrowingWishart" )
      {
      }

      SegmenterRegionGrowingWishartStrategyFactory::~SegmenterRegionGrowingWishartStrategyFactory()
      {
      }

      te::rp::SegmenterStrategy* SegmenterRegionGrowingWishartStrategyFactory::build()
      {
        return new teradar::segmenter::SegmenterRegionGrowingWishartStrategy();
      }
    } // end namespace segmenter
  } // end namespace teradar
