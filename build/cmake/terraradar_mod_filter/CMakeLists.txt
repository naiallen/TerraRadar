# Description: Build configuration for the TerraRadar Filter Module
#
#  Author: Etore Marcari Jr. <etore@dpi.inpe.br>
#          Raian Vargas Maretto <raian@dpi.inpe.br>
#

# Includes
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${TERRALIB_INCLUDE_DIR})
include_directories(${TERRALIB_INCLUDE_CONFIG_DIR})

#Set include directories location
include_directories(${TERRARADAR_SRC_DIR}/library/filter)

if(WIN32)
  add_definitions(-DTRFILTDLL)
endif()

# Source and Header files
file(GLOB TERRARADAR_FILT_SRCS ${TERRARADAR_SRC_DIR}/library/filter/*.cpp)
file(GLOB TERRARADAR_FILT_HDRS ${TERRARADAR_SRC_DIR}/library/filter/*.hpp)

# Creating source groups for IDEs
source_group("Source Files"  FILES ${TERRARADAR_FILT_SRCS})
source_group("Header Files"  FILES ${TERRARADAR_FILT_HDRS})

# Setting Dependencies
set(TERRARADAR_FILT_LIB_DEPENDENCIES #terraradar_mod_filter
                                 ${Boost_SYSTEM_LIBRARY}
                                 ${TERRALIB_RASTER_LIBRARY}
                                 ${TERRALIB_COMMON_LIBRARY}
                                 ${TERRALIB_RP_LIBRARY}
)

add_library(terraradar_mod_filter SHARED ${TERRARADAR_FILT_SRCS} ${TERRARADAR_FILT_HDRS})

target_link_libraries(terraradar_mod_filter ${TERRARADAR_FILT_LIB_DEPENDENCIES})

if(TERRARADAR_SOURCE_VERIFY_ENABLED)
    add_style_check_target(terraradar_mod_filter "${TERRARADAR_FILT_SRCS};${TERRARADAR_FILT_HDRS}")
endif()

set_target_properties(terraradar_mod_filter
                      PROPERTIES VERSION ${TERRARADAR_VERSION_MAJOR}.${TERRARADAR_VERSION_MINOR}
                                 SOVERSION ${TERRARADAR_VERSION_MAJOR}.${TERRARADAR_VERSION_MINOR}
                                 INSTALL_NAME_DIR "@executable_path/../library")
