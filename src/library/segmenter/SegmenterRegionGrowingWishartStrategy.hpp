/*  Copyright (C) 2015 National Institute For Space Research (INPE) - Brazil.

This file is part of the TerraRadar - a library and application for radar data manipulation.

TerraRadar is under development.
*/

/*!
  \file terraradar/library/segmenter/SegmenterRegionGrowingWishartStrategy.h
  \brief Raster region growing segmenter Wishart strategy.
*/

#ifndef TERRARADAR_LIB_SEGM_INTERNAL_SEGMENTERREGIONGROWINGWISHARTSTRATEGY_HPP_
#define TERRARADAR_LIB_SEGM_INTERNAL_SEGMENTERREGIONGROWINGWISHARTSTRATEGY_HPP_

// TerraRadar includes
#include "config.hpp"

#include "../common/RadarFunctions.hpp"
#include "../common/Utils.hpp"

// TerraLib includes
#include <terralib/rp/SegmenterRegionGrowingFunctions.h>
#include <terralib/rp/SegmenterRegionGrowingSegment.h>
#include <terralib/rp/SegmenterRegionGrowingSegmentsPool.h>
#include <terralib/rp/SegmenterSegmentsBlock.h>
#include <terralib/rp/SegmenterStrategyFactory.h>
#include <terralib/rp/SegmenterStrategy.h>

namespace teradar {
  namespace segmenter {
    typedef std::complex<double> WishartFeatureType;

    /*!
      \class SegmenterRegionGrowingWishartStrategy
      \brief Raster region growing segmenter strategy.
    */
    class TERADARSEGMEXPORT SegmenterRegionGrowingWishartStrategy : public te::rp::SegmenterStrategy
    {
      public:
        /*!
          \class Parameters
          \brief Segmenter parameters.
        */
        class TERADARSEGMEXPORT Parameters : public te::rp::SegmenterStrategyParameters
        {
          public:
            /*!
              \brief Constructor.
            */
            Parameters();

            /*!
              \brief Destructor.
            */
            ~Parameters();

            //overload 
            const Parameters& operator=( const Parameters& params );

            //overload
            void reset() throw( te::rp::Exception );

            //overload
            te::common::AbstractParameters* clone() const;

            /*
            double m_segmentsSimilarityThreshold; //!< Segments similarity treshold - Use lower values to merge only those segments that are more similar - Higher values will allow more segments to be merged - valid values range: positive values - default: 0.03 ).

            unsigned int m_segmentsSimIncreaseSteps; //!< The maximum number of steps to increment the similarity threshold value for the cases where no segment merge occurred - zero will disable segment similarity threshold increments - defaul: 2.

            bool m_enableSameIterationMerges; //!< If enabled, a merged segment could be merged with another within the same iteration (default:false).

            bool m_enableLocalMutualBestFitting; //!< If enabled, a merge only occurs between two segments if the minimum dissimilarity criteria is best fulfilled mutually (default: false).
            */

            unsigned int m_minSegmentSize; //!< A positive minimum segment size - minimal area - (pixels number - default: 100).

            teradar::common::RadarDataType m_dataType; //!< Data type.

            bool m_enableAzimutalSimetry; //!< If azimutal simetry should be used (default: false).

            double m_enlLZero; //!< Equivalent number of looks of no compressed image (default - 1).

            double m_compressionLevel; //!< Image compression level.

            teradar::common::PixelConnectivityType m_connectivityType; //!< Pixel connectivity type.

            unsigned int m_regionGrowingLimit; //!< Region growing limit, in cycles (default - 15).
            
            double m_regionGrowingConfLevel; //!< Region growing confidence level, in percentage (default - 99,9).

            unsigned int m_regionMergingLimit; //<! Region merging limit, in cycles (default - 1).

            double m_regionMergingConfLevel; //!< Region merging confidence level, in percentage (default - 99,9).
        };

        /*!
          \brief Constructor.
         */
        SegmenterRegionGrowingWishartStrategy();

        /*!
          \brief Destructor.
         */
        ~SegmenterRegionGrowingWishartStrategy();

        //overload
        bool initialize( te::rp::SegmenterStrategyParameters const* const strategyParams )
          throw( te::rp::Exception );

        //overload
        void reset();

        //overload
        bool execute( te::rp::SegmenterIdsManager& segmenterIdsManager,
          const te::rp::SegmenterSegmentsBlock& block2ProcessInfo,
          const te::rst::Raster& inputRaster,
          const std::vector< unsigned int >& inputRasterBands,
          const std::vector< std::complex< double > >& inputRasterNoDataValues,
          const std::vector< std::complex< double > >& inputRasterBandMinValues,
          const std::vector< std::complex< double > >& inputRasterBandMaxValues,
          te::rst::Raster& outputRaster,
          const unsigned int outputRasterBand,
          const bool enableProgressInterface ) throw(te::rp::Exception);

        //overload         
        double getMemUsageEstimation( const unsigned int bandsToProcess,
          const unsigned int pixelsNumber ) const;

        //overload  
        unsigned int getOptimalBlocksOverlapSize() const;

        //overload
        bool shouldComputeMinMaxValues() const;

        //overload
        BlocksMergingMethod getBlocksMergingMethod() const;

      protected:
        /*!
          \brief Initialize the segment objects container and the segment IDs container.
          \param segmenterIdsManager A segments ids manager to acquire unique segments ids.
          \param block2ProcessInfo Info about the block to process.
          \param inputRaster The input raster.
          \param inputRasterBands Input raster bands to use.
          \param actSegsListHeadPtr A pointer the the active segments list head.
          \return true if OK, false on errors.
         */
        bool initializeSegments( te::rp::SegmenterIdsManager& segmenterIdsManager,
          const te::rp::SegmenterSegmentsBlock& block2ProcessInfo,
          const te::rst::Raster& inputRaster,
          const std::vector< unsigned int >& inputRasterBands,
          te::rp::SegmenterRegionGrowingSegment< WishartFeatureType >** actSegsListHeadPtr );
         
        /*!
          \brief true if this instance is initialized.
         */
        bool m_isInitialized;

        /*!
          \brief Internal segments ids matrix type definition.
         */
        typedef te::rp::Matrix< te::rp::SegmenterSegmentsBlock::SegmentIdDataType > SegmentsIdsMatrixT;

        /*!
          \brief Internal execution parameters.
         */
        SegmenterRegionGrowingWishartStrategy::Parameters m_parameters;

        /*!
          \brief A pool of segments that can be reused on each strategy execution.
         */
        te::rp::SegmenterRegionGrowingSegmentsPool< WishartFeatureType > m_segmentsPool;

        /*!
          \brief A internal segments IDs matrix that can be reused  on each strategy execution.
         */
        SegmentsIdsMatrixT m_segmentsIdsMatrix;
      };

    /*!
      \class SegmenterRegionGrowingWishartStrategyFactory
      \brief Raster region growing segmenter strategy factory.
      \note Factory key : RegionGrowingWishart
     */
    class TERADARSEGMEXPORT SegmenterRegionGrowingWishartStrategyFactory : public
      te::rp::SegmenterStrategyFactory
    {
      public:
        /*!
          \brief Constructor.
         */
        SegmenterRegionGrowingWishartStrategyFactory();

        /*!
          \brief Destructor.
         */
        ~SegmenterRegionGrowingWishartStrategyFactory();

        //overload
        te::rp::SegmenterStrategy* build();
    };
  } // end namespace segmenter
} // end namespace teradar

#endif  // TERRARADAR_LIB_SEGM_INTERNAL_SEGMENTERREGIONGROWINGWISHARTSTRATEGY_HPP_