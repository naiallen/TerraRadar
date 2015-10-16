# Description: Build configuration for the TerraRadar Segmenter Module
#
#  Author: Etore Marcari Jr. <etore@dpi.inpe.br>
#          Raian Vargas Maretto <raian@dpi.inpe.br>
#

# Includes
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${TERRALIB_INCLUDE_DIR})
include_directories(${TERRALIB_INCLUDE_CONFIG_DIR})

#Set include directories location
include_directories(${TERRARADAR_SRC_DIR}/library/segmenter)

if(WIN32)
  add_definitions(-DTERADARSEGMDLL)
endif()

# Source and Header files
file(GLOB TERRARADAR_SEGM_SRCS ${TERRARADAR_SRC_DIR}/library/segmenter/*.cpp)
file(GLOB TERRARADAR_SEGM_HDRS ${TERRARADAR_SRC_DIR}/library/segmenter/*.hpp)

# Creating source groups for IDEs
source_group("Source Files"  FILES ${TERRARADAR_SEGM_SRCS})
source_group("Header Files"  FILES ${TERRARADAR_SEGM_HDRS})

# Setting Dependencies
set(TERRARADAR_SEGM_LIB_DEPENDENCIES #terraradar_mod_segmenter
                                 ${Boost_SYSTEM_LIBRARY}
								 ${Boost_THREAD_LIBRARY}
								 ${Boost_FILESYSTEM_LIBRARY}
                                 ${TERRALIB_RASTER_LIBRARY}
                                 ${TERRALIB_COMMON_LIBRARY}
                                 ${TERRALIB_RP_LIBRARY}
								 ${TERRALIB_MEMORY_LIBRARY}
)

add_library(terraradar_mod_segmenter SHARED ${TERRARADAR_SEGM_SRCS} ${TERRARADAR_SEGM_HDRS})

target_link_libraries(terraradar_mod_segmenter ${TERRARADAR_SEGM_LIB_DEPENDENCIES})

if(TERRARADAR_SOURCE_VERIFY_ENABLED)
    add_style_check_target(terraradar_mod_segmenter "${TERRARADAR_SEGM_SRCS};${TERRARADAR_SEGM_HDRS}")
endif()

set_target_properties(terraradar_mod_segmenter
                      PROPERTIES VERSION ${TERRARADAR_VERSION_MAJOR}.${TERRARADAR_VERSION_MINOR}
                                 SOVERSION ${TERRARADAR_VERSION_MAJOR}.${TERRARADAR_VERSION_MINOR}
                                 INSTALL_NAME_DIR "@executable_path/../library")
