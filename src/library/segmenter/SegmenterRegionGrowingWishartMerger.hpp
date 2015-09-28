/*  Copyright (C) 2015 National Institute For Space Research (INPE) - Brazil.

This file is part of the TerraRadar - a library and application for radar data manipulation.

TerraRadar is under development.
*/

/*!
  \file terraradar/library/segmenter/SegmenterRegionGrowingWishartMerger.h
  \brief Segmenter Wishart merger used in region growing process.
  */

#ifndef TERRARADAR_LIB_SEGM_INTERNAL_SEGMENTERREGIONGROWINGWISHARTMERGER_HPP_
#define TERRARADAR_LIB_SEGM_INTERNAL_SEGMENTERREGIONGROWINGWISHARTMERGER_HPP_

// TerraRadar includes
#include "config.hpp"

// Terralib includes
#include <terralib/rp/SegmenterRegionGrowingFunctions.h>
#include <terralib/rp/SegmenterRegionGrowingMerger.h>

namespace teradar {
  namespace segmenter {
    /*!
      \class SegmenterRegionGrowingWishartMerger
      \brief Segments merger based on Wishart method.
      */
    class TERADARSEGMEXPORT SegmenterRegionGrowingWishartMerger : 
      public te::rp::SegmenterRegionGrowingMerger < te::rp::rg::WishartFeatureType >
    {
      public:
        /*!
          \brief Constructor.
          \param featuresNumber Number of features (elements in the covariance matrix).
          \param numberOfLooks Number of looks.
        */
        SegmenterRegionGrowingWishartMerger( const unsigned int featuresNumber,
          const double& numberOfLooks );

        /*!
          \brief Destructor.
        */
        ~SegmenterRegionGrowingWishartMerger();

        //overload        
        te::rp::DissimilarityTypeT
          getDissimilarity( te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType > const * const segment1Ptr,
          te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType > const * const segment2Ptr,
          te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType > * const mergePreviewSegPtr ) const;

        //overload                
        void mergeFeatures( te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType > * const segment1Ptr,
          te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType > const * const segment2Ptr,
          te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType > const * const mergePreviewSegPtr ) const;

        //overload
        void update( te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType >* const actSegsListHeadPtr );

        //overload
        inline unsigned int getSegmentFeaturesSize() const {
          return m_featuresNumber;
        };

      protected:
        unsigned int m_featuresNumber; //!< The number of elements in the covariance matrix.
        double m_numberOfLooks; //!< Number of looks.

        // variables used by the method getDissimilarity
        mutable te::rp::DissimilarityTypeT m_getDissimilarity_dissValue;
        mutable double m_getDissimilarity_noise;
        mutable unsigned int m_getDissimilarity_sizeUnionD;
        mutable unsigned int m_getDissimilarity_sizeSeg1D;
        mutable unsigned int m_getDissimilarity_sizeSeg2D;
        mutable unsigned int m_getDissimilarity_elemXIdx;
        mutable unsigned int m_getDissimilarity_elemYIdx;
    };
  } // end namespace segmenter
} // end namespace teradar

#endif  // TERRARADAR_LIB_SEGM_INTERNAL_SEGMENTERREGIONGROWINGWISHARTMERGER_HPP_