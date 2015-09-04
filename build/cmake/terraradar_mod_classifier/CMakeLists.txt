# Description: Build configuration for the TerraRadar Classifier Module
#
#  Author: Etore Marcari Jr. <etore@dpi.inpe.br>
#          Raian Vargas Maretto <raian@dpi.inpe.br>
#

# Includes
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${TERRALIB_INCLUDE_DIR})
include_directories(${TERRALIB_INCLUDE_CONFIG_DIR})

#Set include directories location
include_directories(${TERRARADAR_SRC_DIR}/library/classifier)

if(WIN32)
  add_definitions(-DTRCLASSDLL)
endif()

# Source and Header files
file(GLOB TERRARADAR_CLASS_SRCS ${TERRARADAR_SRC_DIR}/library/classifier/*.cpp)
file(GLOB TERRARADAR_CLASS_HDRS ${TERRARADAR_SRC_DIR}/library/classifier/*.hpp)

# Creating source groups for IDEs
source_group("Source Files"  FILES ${TERRARADAR_CLASS_SRCS})
source_group("Header Files"  FILES ${TERRARADAR_CLASS_HDRS})

# Setting Dependencies
set(TERRARADAR_CLASS_LIB_DEPENDENCIES #terraradar_mod_classifier
                                 ${Boost_SYSTEM_LIBRARY}
                                 ${TERRALIB_RASTER_LIBRARY}
                                 ${TERRALIB_COMMON_LIBRARY}
                                 ${TERRALIB_RP_LIBRARY}
)

add_library(terraradar_mod_classifier SHARED ${TERRARADAR_CLASS_SRCS} ${TERRARADAR_CLASS_HDRS})

target_link_libraries(terraradar_mod_classifier ${TERRARADAR_CLASS_LIB_DEPENDENCIES})

if(TERRARADAR_SOURCE_VERIFY_ENABLED)
    add_style_check_target(terraradar_mod_classifier "${TERRARADAR_CLASS_SRCS};${TERRARADAR_CLASS_HDRS}")
endif()

set_target_properties(terraradar_mod_classifier
                      PROPERTIES VERSION ${TERRARADAR_VERSION_MAJOR}.${TERRARADAR_VERSION_MINOR}
                                 SOVERSION ${TERRARADAR_VERSION_MAJOR}.${TERRARADAR_VERSION_MINOR}
                                 INSTALL_NAME_DIR "@executable_path/../library")
