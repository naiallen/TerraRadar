# Description: Build configuration for the TerraRadar Common Module
#
#  Author: Etore Marcari Jr. <etore@dpi.inpe.br>
#          Raian Vargas Maretto <raian@dpi.inpe.br>
#

# Includes
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${terralib_INCLUDE_DIRS})
include_directories(${terralib_DIR})

#Set include directories location
include_directories(${TERRARADAR_SRC_DIR}/library/common)

if(WIN32)
  add_definitions(-DTERADARCOMMONDLL)
endif()

# Source and Header files
file(GLOB TERRARADAR_COMMON_SRCS ${TERRARADAR_SRC_DIR}/library/common/*.cpp)
file(GLOB TERRARADAR_COMMON_HDRS ${TERRARADAR_SRC_DIR}/library/common/*.hpp)

# Creating source groups for IDEs
source_group("Source Files"  FILES ${TERRARADAR_COMMON_SRCS})
source_group("Header Files"  FILES ${TERRARADAR_COMMON_HDRS})

# Setting Dependencies
set(TERRARADAR_COMMON_LIB_DEPENDENCIES ${Boost_SYSTEM_LIBRARY}
									                     ${Boost_FILESYSTEM_LIBRARY}
                                       terralib_mod_common
                                       terralib_mod_plugin
                                       terralib_mod_rp
                                       terralib_mod_raster
)

add_library(terraradar_mod_common SHARED ${TERRARADAR_COMMON_SRCS} ${TERRARADAR_COMMON_HDRS})

target_link_libraries(terraradar_mod_common ${TERRARADAR_COMMON_LIB_DEPENDENCIES})

if(TERRARADAR_SOURCE_VERIFY_ENABLED)
    add_style_check_target(terraradar_mod_common "${TERRARADAR_COMMON_SRCS};${TERRARADAR_COMMON_HDRS}")
endif()

set_target_properties(terraradar_mod_common
                      PROPERTIES VERSION ${TERRARADAR_VERSION_MAJOR}.${TERRARADAR_VERSION_MINOR}
                                 SOVERSION ${TERRARADAR_VERSION_MAJOR}.${TERRARADAR_VERSION_MINOR}
                                 INSTALL_NAME_DIR "@executable_path/../library")
