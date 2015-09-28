/*  Copyright (C) 2015 National Institute For Space Research (INPE) - Brazil.

This file is part of the TerraRadar - a library and application for radar data manipulation.

TerraRadar is under development.
*/

/*!
  \file terraradar/library/segmenter/SegmenterRegionGrowingWishartMerger.cpp
  \brief Segmenter Wishart merger used in region growing process.
  */

// TerraRadar includes
#include "SegmenterRegionGrowingWishartMerger.hpp"

// TerraLib includes
#include <terralib/common/MatrixUtils.h>

// Boost includes
#include <boost/math/distributions/chi_squared.hpp>

namespace teradar {
  namespace segmenter {
    SegmenterRegionGrowingWishartMerger::SegmenterRegionGrowingWishartMerger( const unsigned int featuresNumber,
      const double& numberOfLooks )
    {
      m_featuresNumber = featuresNumber;
      m_numberOfLooks = numberOfLooks;
      m_getDissimilarity_noise = 1e-8;
    }

    SegmenterRegionGrowingWishartMerger::~SegmenterRegionGrowingWishartMerger()
    {
    }

    te::rp::DissimilarityTypeT
      SegmenterRegionGrowingWishartMerger::getDissimilarity( te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType > const * const segment1Ptr,
      te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType > const * const segment2Ptr,
      te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType > * const mergePreviewSegPtr ) const
    {
      assert( segment1Ptr );
      assert( segment1Ptr->m_features );
        assert( segment2Ptr );
        assert( segment2Ptr->m_features );
        assert( mergePreviewSegPtr );

        // globals
        m_getDissimilarity_sizeSeg1D = segment1Ptr->m_size;
        m_getDissimilarity_sizeSeg2D = segment2Ptr->m_size;
        m_getDissimilarity_sizeUnionD = m_getDissimilarity_sizeSeg1D + m_getDissimilarity_sizeSeg2D;

        TERP_DEBUG_TRUE_OR_THROW( m_getDissimilarity_sizeUnionD != 0., "Internal error" );
        mergePreviewSegPtr->m_size = (unsigned int)m_getDissimilarity_sizeUnionD;

        mergePreviewSegPtr->m_xStart = std::min( segment1Ptr->m_xStart, segment2Ptr->m_xStart );
        mergePreviewSegPtr->m_yStart = std::min( segment1Ptr->m_yStart, segment2Ptr->m_yStart );
        mergePreviewSegPtr->m_xBound = std::max( segment1Ptr->m_xBound, segment2Ptr->m_xBound );
        mergePreviewSegPtr->m_yBound = std::max( segment1Ptr->m_yBound, segment2Ptr->m_yBound );

        assert( mergePreviewSegPtr->m_xBound > mergePreviewSegPtr->m_xStart );
        assert( mergePreviewSegPtr->m_yBound > mergePreviewSegPtr->m_yStart );

        // fill the covariance matrix
        unsigned int covMatrixOrder = (unsigned int)std::sqrt( m_featuresNumber );

        boost::numeric::ublas::matrix< te::rp::rg::WishartFeatureType > segment1Matrix( covMatrixOrder, covMatrixOrder );
        boost::numeric::ublas::matrix< te::rp::rg::WishartFeatureType > segment2Matrix( covMatrixOrder, covMatrixOrder );

        for( m_getDissimilarity_elemXIdx = 0; m_getDissimilarity_elemXIdx < covMatrixOrder; ++m_getDissimilarity_elemXIdx ) {
          for( m_getDissimilarity_elemYIdx = 0; m_getDissimilarity_elemYIdx < covMatrixOrder; ++m_getDissimilarity_elemYIdx ) {
            segment1Matrix( m_getDissimilarity_elemXIdx, m_getDissimilarity_elemYIdx ) = segment1Ptr->m_features[3 * m_getDissimilarity_elemYIdx + m_getDissimilarity_elemXIdx];
            segment2Matrix( m_getDissimilarity_elemXIdx, m_getDissimilarity_elemYIdx ) = segment2Ptr->m_features[3 * m_getDissimilarity_elemYIdx + m_getDissimilarity_elemXIdx];
          }
        }

        // Compute the union values.
        boost::numeric::ublas::matrix< te::rp::rg::WishartFeatureType > segmentUMatrix( covMatrixOrder, covMatrixOrder );
        boost::numeric::ublas::matrix< te::rp::rg::WishartFeatureType > segmentPUMatrix( covMatrixOrder, covMatrixOrder );

        segmentPUMatrix = (segment1Matrix * m_getDissimilarity_sizeSeg1D) + (segment2Matrix * m_getDissimilarity_sizeSeg2D);
        segmentPUMatrix /= m_getDissimilarity_sizeUnionD;

        segmentUMatrix = segment1Matrix + segment2Matrix;


        // Update mergePreviewSegment
        for( m_getDissimilarity_elemXIdx = 0; m_getDissimilarity_elemXIdx < covMatrixOrder; ++m_getDissimilarity_elemXIdx ) {
          for( m_getDissimilarity_elemYIdx = 0; m_getDissimilarity_elemYIdx < covMatrixOrder; ++m_getDissimilarity_elemYIdx ) {
            mergePreviewSegPtr->m_features[3 * m_getDissimilarity_elemYIdx + m_getDissimilarity_elemXIdx] =
              segmentPUMatrix( m_getDissimilarity_elemXIdx, m_getDissimilarity_elemYIdx );
          }
        }

        te::rp::rg::WishartFeatureType seg1Det = 0.;
        te::common::GetDeterminant< te::rp::rg::WishartFeatureType >( segment1Matrix, seg1Det );

        if( std::real( seg1Det ) == 0 ) {
          do {
            for( m_getDissimilarity_elemXIdx = 0; m_getDissimilarity_elemXIdx < covMatrixOrder; ++m_getDissimilarity_elemXIdx ) {
              for( m_getDissimilarity_elemYIdx = 0; m_getDissimilarity_elemYIdx < covMatrixOrder; ++m_getDissimilarity_elemYIdx ) {
                segment1Matrix( m_getDissimilarity_elemXIdx, m_getDissimilarity_elemYIdx ) =
                  segment1Matrix( m_getDissimilarity_elemXIdx, m_getDissimilarity_elemYIdx ) + m_getDissimilarity_noise;
              }
            }

            te::common::GetDeterminant< te::rp::rg::WishartFeatureType >( segment1Matrix, seg1Det );

          } while( std::real( seg1Det ) == 0 );
        }

        te::rp::rg::WishartFeatureType seg2Det = 0.;
        te::common::GetDeterminant< te::rp::rg::WishartFeatureType >( segment2Matrix, seg2Det );

        if( std::real( seg2Det ) == 0 ) {
          do {
            for( m_getDissimilarity_elemXIdx = 0; m_getDissimilarity_elemXIdx < covMatrixOrder; ++m_getDissimilarity_elemXIdx ) {
              for( m_getDissimilarity_elemYIdx = 0; m_getDissimilarity_elemYIdx < covMatrixOrder; ++m_getDissimilarity_elemYIdx ) {
                segment2Matrix( m_getDissimilarity_elemXIdx, m_getDissimilarity_elemYIdx ) =
                  segment2Matrix( m_getDissimilarity_elemXIdx, m_getDissimilarity_elemYIdx ) + m_getDissimilarity_noise;
              }
            }

            te::common::GetDeterminant< te::rp::rg::WishartFeatureType >( segment2Matrix, seg2Det );

          } while( std::real( seg2Det ) == 0 );
        }

        te::rp::rg::WishartFeatureType segUDet = 0.;
        te::common::GetDeterminant< te::rp::rg::WishartFeatureType >( segmentUMatrix, segUDet );

        if( std::real( segUDet ) == 0 ) {
          do {
            for( m_getDissimilarity_elemXIdx = 0; m_getDissimilarity_elemXIdx < covMatrixOrder; ++m_getDissimilarity_elemXIdx ) {
              for( m_getDissimilarity_elemYIdx = 0; m_getDissimilarity_elemYIdx < covMatrixOrder; ++m_getDissimilarity_elemYIdx ) {
                segmentUMatrix( m_getDissimilarity_elemXIdx, m_getDissimilarity_elemYIdx ) =
                  segmentUMatrix( m_getDissimilarity_elemXIdx, m_getDissimilarity_elemYIdx ) + 2 * m_getDissimilarity_noise;
              }
            }

            te::common::GetDeterminant< te::rp::rg::WishartFeatureType >( segmentUMatrix, segUDet );

          } while( std::real( segUDet ) == 0 );
        }

        // This values are defined according to Saldanha, 2014 - Um segmentador multinivel para imagens SAR polarimetricas baseado na distribuicao Wishart, pp. 21-23
        te::rp::DissimilarityTypeT n1 = m_getDissimilarity_sizeSeg1D * m_numberOfLooks;
        te::rp::DissimilarityTypeT n2 = m_getDissimilarity_sizeSeg2D * m_numberOfLooks;
        te::rp::DissimilarityTypeT nu = n1 + n2;

        // Get the real part of complex determinants.
        double seg1DetAbs = std::fabs( std::real( seg1Det ) );
        double seg2DetAbs = std::fabs( std::real( seg2Det ) );
        double segUDetAbs = std::fabs( std::real( segUDet ) );

        te::rp::DissimilarityTypeT f = pow( covMatrixOrder, 2 ); // degree of freedom
        te::rp::DissimilarityTypeT ro = 1. - ((2. * f - 1.) / (6. * covMatrixOrder)) * ((1. / n1) + (1. / n2) - (1. / nu));
        te::rp::DissimilarityTypeT w2 = (-f / 4.) * pow( (1. - (1. / ro)), 2 ) + ((f * (f - 1.)) / 24.) * ((1. / pow( n2, 2 )) + (1. / pow( n1, 2 )) - (1. / pow( nu, 2 ))) * (1. / pow( ro, 2 ));
        te::rp::DissimilarityTypeT q1 = (covMatrixOrder * nu * log( nu )) - ((covMatrixOrder * n2 * log( n2 )) + (covMatrixOrder * n1 * log( n1 )));
        te::rp::DissimilarityTypeT Q = q1 + (n2 * log( seg1DetAbs )) + (n1 * log( seg2DetAbs )) - (nu * log( segUDetAbs ));
        te::rp::DissimilarityTypeT L = -2. * ro * Q;

        te::rp::DissimilarityTypeT seg1Prob = 0.;
        te::rp::DissimilarityTypeT seg2Prob = 0.;

        // Chi Squared is valid only if L >= 0
        if( L >= 0 ) {
          boost::math::chi_squared_distribution< te::rp::DissimilarityTypeT > seg1Distr( f );
          seg1Prob = cdf( seg1Distr, L );

          boost::math::chi_squared_distribution< te::rp::DissimilarityTypeT > seg2Distr( f + 4 );
          seg2Prob = cdf( seg2Distr, L );
        }

        m_getDissimilarity_dissValue = seg1Prob + w2 * (seg2Prob - seg1Prob);

        return m_getDissimilarity_dissValue;
      }

      void SegmenterRegionGrowingWishartMerger::mergeFeatures( te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType > * const segment1Ptr,
      te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType > const * const segment2Ptr,
      te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType > const * const mergePreviewSegPtr ) const
    {
      assert( segment1Ptr );
      assert( segment1Ptr->m_features );
      assert( segment2Ptr );
      assert( segment2Ptr->m_features );
      assert( mergePreviewSegPtr );

      // Merging basic features
      segment1Ptr->m_size = mergePreviewSegPtr->m_size;
      segment1Ptr->m_xStart = mergePreviewSegPtr->m_xStart;
      segment1Ptr->m_xBound = mergePreviewSegPtr->m_xBound;
      segment1Ptr->m_yStart = mergePreviewSegPtr->m_yStart;
      segment1Ptr->m_yBound = mergePreviewSegPtr->m_yBound;

      // Merging specific features   
      memcpy( segment1Ptr->m_features, mergePreviewSegPtr->m_features,
        sizeof(te::rp::rg::WishartFeatureType) * getSegmentFeaturesSize() );
    }

    void SegmenterRegionGrowingWishartMerger::update( te::rp::SegmenterRegionGrowingSegment< te::rp::rg::WishartFeatureType >* const actSegsListHeadPtr )
    {
    }

  } // end namespace segmenter
} // end namespace teradar