/*  Copyright (C) 2015 National Institute For Space Research (INPE) - Brazil.

This file is part of the TerraRadar - a library and application for radar data manipulation.

TerraRadar is under development.
*/

/*!
  \file terraradar/library/common/MultiResolution.hpp
  \brief Multiresolution class that handles complex data.
  */

#ifndef TERRARADAR_LIB_COMMON_INTERNAL_MULTIRESOLUTION_HPP_
#define TERRARADAR_LIB_COMMON_INTERNAL_MULTIRESOLUTION_HPP_

// TerraRadar includes
#include "config.hpp"

// TerraLib includes
#include <terralib/Raster.h>

namespace teradar {
  namespace common {
    /*!
      \class MultiResolution
      \brief MultiResolution facility class.

      \details This multiresolution class is necessary because te::raster uses
      gdal, that works differently from desired behaviour in subsampling process.
    */
    class TERADARCOMMONEXPORT MultiResolution
    {
      public:
        /*!
          \brief Given the number of @a lines and @a columns, computes the max
          compression level.
          \param lines Number of lines.
          \param columns Number of columns.
          \return The max compression level.        
        */
        static unsigned int computeMaxCompressionLevel( unsigned int lines, unsigned int columns );
        
        /*!
          \brief Constructor.
          \param inputRaster Input raster.
          \param levels Number of levels to be created in the multi resolution,
          plus the level 0.
          \param enableProgressInterface Enable/disable the use of a progress.
        */
        MultiResolution( const te::rst::Raster& inputRaster, size_t levels,
          const bool enableProgressInterface = false );

        /*!
          \brief Constructor.
          \param inputRaster Input raster.
          \param levels Number of levels in the multi resolution.
          plus the level 0.
          \param bandsNumbers Numbers of bands to be used when computing stats.
          \param enableProgressInterface Enable/disable the use of a progress.
        */
        MultiResolution( const te::rst::Raster& inputRaster, size_t levels,
          const std::vector<size_t>& bandsNumbers,
          const bool enableProgressInterface = false );

        /// Descructor.
        ~MultiResolution();

        /*!
          \brief Return the number of multi resolution levels.
          \return Number of multi resolution levels.
        */
        size_t getNumberOfLevels() const;

        /*!
          \brief Return the desired multi resolution @a level.
          \param level Desired multi resolution level.
          \return The raster of the given level.
        */
        te::rst::Raster* getLevel( size_t level ) const;

        /*!
          \!brief Remove all multi resolution stored rasters.
        */
        void remove();

        /*!
          \brief Given a level, returns the number of lines and columns associated to it.
          \param level MultiResolution level.
          \param lines Number of lines from given level.
          \param cols Number of columns from given level.
          \return True if lines and columns were taken without errors. False otherwise.
        */
        bool getNumberOfLinesAndColumns( size_t level, size_t& lines, size_t& cols ) const;

      protected:
        /*!
          \brief Create the multi resolution levels.
          */
        void createLevels();

        /*!
          \brief Create a new multi resolution level based on the original one.
          The new level must have a half number of lines and columns when compared to
          the original one.

          \param srcRaster Source raster, to read information from.
          \param dstRaster Destination raster, to write information into.
        */
        void createLevel( const te::rst::Raster& srcRaster, te::rst::Raster& dstRaster );

      private:
        std::vector<size_t> m_bandsNumbers; //!< Bands used in the multi resolution creation.
        std::vector<te::rst::Raster*> m_levels; //!< Internal levels.
        bool m_enableProgress; //!< Enable/Disable the progress interface.
    };
  } // end namespace common
} // end namespace teradar

#endif // TERRARADAR_LIB_COMMON_INTERNAL_MULTIRESOLUTION_HPP_
