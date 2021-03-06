/*  Copyright (C) 2015 National Institute For Space Research (INPE) - Brazil.

This file is part of the TerraRadar - a library and application for radar data manipulation.

TerraRadar is under development.
*/

/*!
  \file terraradar/library/segmenter/MultiLevelSegmenter.hpp
  \brief MultiLevel Segmentation.
*/

#ifndef TERRARADAR_LIB_SEGM_INTERNAL_MULTILEVELSEGMENTER_HPP_
#define TERRARADAR_LIB_SEGM_INTERNAL_MULTILEVELSEGMENTER_HPP_

// TerraRadar includes
#include "config.hpp"

// TerraLib includes
#include <terralib/raster/RasterSynchronizer.h>
#include <terralib/rp/Algorithm.h>
#include <terralib/rp/Matrix.h>
#include <terralib/rp/SegmenterSegmentsBlock.h>
#include <terralib/rp/SegmenterIdsManager.h>
#include <terralib/rp/SegmenterStrategyParameters.h>

namespace teradar {
  namespace segmenter {
    /*!
      \class Multi Level Segmenter
      \brief Radar Raster Multi Level segmentation.
      \details Raster segmentation following the choosed segmentation
      strategy.
      \note The created label image will be written to the raster instance
      pointed by m_outRasterPtr (in this case the output band must also be
      passed by m_outRasterBand ); or written to a new raster instance created
      inside the given data source pointed by m_outDataSourcePtr (in this case
      the data set name must be supplied - m_outDataSetName ).
    */
    class TERADARSEGMEXPORT MultiLevelSegmenter : public te::rp::Algorithm
    {
      public:
        /*!
          \class InputParameters
          \brief Segmenter Input Parameters
        */
        class TERADARSEGMEXPORT InputParameters : public te::rp::AlgorithmInputParameters
        {
          public:
            te::rst::Raster const* m_inputRasterPtr; //!< Input raster.

            std::vector< unsigned int > m_inputRasterBands; //!< Bands to be processed from the input raster.

            std::vector< std::complex< double > > m_inputRasterNoDataValues; //!< A vector of values to be used as input raster no-data values or an empty vector indicating to use the default values from the input raster..

            bool m_enableThreadedProcessing; //!< If true, threaded processing will be performed (best with  multi-core or multi-processor systems (default:false).

            unsigned int m_maxSegThreads; //!< The maximum number of concurrent segmenter threads (default:0 - automatically found).

            bool m_enableBlockProcessing; //!< If true, the original raster will be splitted into small blocks, each one will be segmented independently and the result will be merged (if possible) at the end (default:false).

            unsigned int m_maxBlockSize; //!< The input image will be split into blocks with this width for processing, this parameter tells the maximum block lateral size (width or height), the default: 0 - the size will be defined following the current system resources and physical processors number).

            unsigned char m_blocksOverlapPercent; //!< The percentage of blocks overlapped area (valid range:0-25, defaul:0).

            std::string m_strategyName; //!< The segmenter strategy name see each te::rp::SegmenterStrategyFactory inherited classes documentation for reference.

            bool m_enableProgress; //!< Enable/Disable the progress interface (default:false).

            bool m_enableRasterCache; //!< Enable/Disable the use of raster data cache (default:true).

            InputParameters();

            InputParameters( const InputParameters& other );

            ~InputParameters();

            /*!
              \brief Set specific segmenter strategy parameters.
              \param params The specific segmenter strategy parameters.
            */
            void setSegStrategyParams( const te::rp::SegmenterStrategyParameters& segStratParams );

            /*!
              \brief Returns a pointer to the internal specific segmenter strategy parameters.
              \return A pointer to the internal specific segmenter strategy parameters, or
              null if no parameters are present.
            */
            te::rp::SegmenterStrategyParameters const* getSegStrategyParams() const;

            //overload
            void reset() throw(te::rp::Exception);

            //overload
            const MultiLevelSegmenter::InputParameters& operator=(const MultiLevelSegmenter::InputParameters& params);

            //overload
            AbstractParameters* clone() const;
            
          protected:
            /*!
              \brief A pointer to the internal specific segmenter strategy parameters or
              NULL if no parameters are present.
            */
            te::rp::SegmenterStrategyParameters* m_segStratParamsPtr;
        };

        /*!
          \class OutputParameters
          \brief Segmenter Output Parameters
        */
        class TERADARSEGMEXPORT OutputParameters : public te::rp::AlgorithmOutputParameters
        {
          public:

            std::string m_rType; //!< Output raster data source type (as described in te::raster::RasterFactory).

            std::map< std::string, std::string > m_rInfo; //!< The necessary information to create the raster (as described in te::raster::RasterFactory). 

            std::auto_ptr< te::rst::Raster > m_outputRasterPtr; //!< A pointer the ge generated output raster (label image).

            OutputParameters();

            OutputParameters( const OutputParameters& other );

            ~OutputParameters();

            //overload
            void reset() throw(te::rp::Exception);

            //overload
            const MultiLevelSegmenter::OutputParameters& operator=(const MultiLevelSegmenter::OutputParameters& params);

            //overload
            AbstractParameters* clone() const;
        };
        
        /*!
          \brief Constructor.
        */
        MultiLevelSegmenter();

        /*!
          \brief Destructor.
        */
        ~MultiLevelSegmenter();

        //overload
        bool execute( te::rp::AlgorithmOutputParameters& outputParams ) throw(te::rp::Exception);

        //overload
        void reset() throw(te::rp::Exception);

        //overload
        bool initialize( const te::rp::AlgorithmInputParameters& inputParams )
          throw(te::rp::Exception);

      bool isInitialized() const;

      protected:
        /*! Segments blocks matrix type definition */
        typedef te::rp::Matrix< te::rp::SegmenterSegmentsBlock > SegmentsBlocksMatrixT;

        /*!
          \brief The parameters passed to the Segmenter::segmenterthreadEntry method.
        */
        class SegmenterThreadEntryParams
        {
          public:
            //! The global segmenter input execution parameters.
            MultiLevelSegmenter::InputParameters m_inputParameters;

            //! A pointer to the global segmenter input execution parameters (default:0).
            MultiLevelSegmenter::OutputParameters* m_outputParametersPtr;

            //! Pointer to the segments blocks matrix (default:0).
            SegmentsBlocksMatrixT* m_segsBlocksMatrixPtr;

            //! Pointer to a general global mutex (default:0).
            boost::mutex* m_generalMutexPtr;

            //! Pointer to the input raster synchronizer (default:0).
            te::rst::RasterSynchronizer* m_inputRasterSyncPtr;

            //! Pointer to the output raster synchronizer (default:0).
            te::rst::RasterSynchronizer* m_outputRasterSyncPtr;

            //! Pointer to the mutex used by the block processed signal (default:0).
            boost::mutex* m_blockProcessedSignalMutexPtr;

            //! Pointer to the abort segmentation flag (default:0).
            bool volatile* m_abortSegmentationFlagPtr;

            //! Pointer to the segments Ids manager - (default 0).
            te::rp::SegmenterIdsManager* m_segmentsIdsManagerPtr;

            //! Pointer to a signal to be emited when a segments block was processed (default:0).
            boost::condition_variable* m_blockProcessedSignalPtr;

            //! Pointer to the running threads counter - default 0).
            unsigned int volatile* m_runningThreadsCounterPtr;

            //! A vector of input raster bands minimum values.
            std::vector< std::complex< double > > m_inputRasterBandMinValues;

            //! A vector of input raster bands maximum values.
            std::vector< std::complex< double > > m_inputRasterBandMaxValues;

            //! A vector of values to be used as input raster no-data values.
            std::vector< std::complex< double > > m_inputRasterNoDataValues;

            //! Enable/Disable the segmentation strategy to use its own progress interface (default:false).
            bool m_enableStrategyProgress;

            //! The maximum number of input raster cached blocks per-thread.
            unsigned int m_maxInputRasterCachedBlocks;

            SegmenterThreadEntryParams();

            ~SegmenterThreadEntryParams();
        };
        
        bool m_instanceInitialized; //"< Is this instance already initialized ?

        MultiLevelSegmenter::InputParameters m_inputParameters; //!< Segmenter execution parameters.

        /*!
          \brief Calc the best sub-image block size for each thread to
          process.
          \param minBlockPixels The minimun allowed pixels number for each block (expanded block).
          \param maxBlockPixels The maximum allowed pixels number for each block (expanded block).
          \param blocksHOverlapSize The blocks horizontal overlap size (number of columns).
          \param blocksVOverlapSize The blocks vertical overlap size (number of rows).
          \param nonExpandedBlockWidth The calculated non-expanded block width (non-expanded block).
          \param nonExpandedBlockHeight The calculated non-expanded block height (non-expanded block).
          \param expandedBlockWidth The calculated expanded block width (non-expanded block).
          \param expandedBlockHeight The calculated expanded block height (non-expanded block).
          \return true if OK, false on errors.
        */
        bool calcBestBlockSize(
          const unsigned int minExapandedBlockPixels,
          const unsigned int maxExapandedBlockPixels,
          unsigned int& blocksHOverlapSize,
          unsigned int& blocksVOverlapSize,
          unsigned int& nonExpandedBlockWidth,
          unsigned int& nonExpandedBlockHeight,
          unsigned int& expandedBlockWidth,
          unsigned int& expandedBlockHeight ) const;

        /*!
          \brief Segmenter thread entry.
          \param paramsPtr A pointer to the segmenter thread parameters.
        */
        static void segmenterThreadEntry( SegmenterThreadEntryParams* paramsPtr );

        /*!
          \brief Generate the horizontal cutOff prifles for the entire image..
          \param profileCenter The profile center line.
          \param inRaster The input raster.
          \param inRasterBands The input raster bands.
          \param pixelNeighborhoodSize The pixel neighborhood size over the line transverse to each tile line.
          \param tileNeighborhoodSize The buffer size around each tile.
          \param profileAntiSmoothingFactor A positive profile anti-smoothing factor.
          \param profile The generated profile (each element is a inRaster line index ).
          \return true if OK, false on errors.
        */
        bool genImageHCutOffProfile( const unsigned int profileCenter,
          const te::rst::Raster& inRaster,
          const std::vector< unsigned int >& inRasterBands,
          const unsigned int pixelNeighborhoodSize,
          const unsigned int tileNeighborhoodSize,
          const unsigned int profileAntiSmoothingFactor,
          std::vector< unsigned int>& profile ) const;

        /*!
          \brief Generate the vertical cutOff prifles for the entire image..
          \param profileCenter The profile center column.
          \param inRaster The input raster.
          \param inRasterBands The input raster bands.
          \param pixelNeighborhoodSize The pixel neighborhood size over the line transverse to each tile line.
          \param tileNeighborhoodSize The buffer size around each tile.
          \param profileAntiSmoothingFactor A positive profile anti-smoothing factor.
          \param profile The generated profile (each element is a inRaster column index )
          \return true if OK, false on errors.
        */
        bool genImageVCutOffProfile( const unsigned int profileCenter,
          const te::rst::Raster& inRaster,
          const std::vector< unsigned int >& inRasterBands,
          const unsigned int pixelNeighborhoodSize,
          const unsigned int tileNeighborhoodSize,
          const unsigned int profileAntiSmoothingFactor,
          std::vector< unsigned int>& profile ) const;

        /*!
          \brief Update the block cutOff profiles using the full image profiles.
          \param imageVerticalProfiles The generated horizontal profiles (empty profiles also will be accepted).
          \param horizontalProfiles The generated vertical profiles (empty profiles also will be accepted).
          \param segmentsBlock The block to update.
          \return true if OK, false on errors.
        */
        bool updateBlockCutOffProfiles(
          const std::vector< std::vector< unsigned int> >& imageHorizontalProfiles,
          const std::vector< std::vector< unsigned int> >& imageVerticalProfiles,
          te::rp::SegmenterSegmentsBlock& segmentsBlock ) const;

        bool createCutOffLinesTiff(
          const std::vector< std::vector< unsigned int> >& imageHorizontalProfiles,
          const std::vector< unsigned int >& imageHorizontalProfilesCenterLines,
          const std::vector< std::vector< unsigned int> >& imageVerticalProfiles,
          const std::vector< unsigned int >& imageVerticalProfilesCenterLines,
          const std::string& filename );
    };
  } // end namespace segmenter
} // end namespace teradar

#endif  // TERRARADAR_LIB_SEGM_INTERNAL_MULTILEVELSEGMENTER_HPP_