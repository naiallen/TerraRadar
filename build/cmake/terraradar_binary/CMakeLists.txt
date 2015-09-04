#
# Description: Build configuration for the TerraRadar Executable
#
#  Author: Etore Marcari Jr. <etore@dpi.inpe.br>
#

# Includes
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${TERRALIB_INCLUDE_DIR})
include_directories(${TERRALIB_INCLUDE_CONFIG_DIR})

#Set include directories location
include_directories(${TERRARADAR_SRC_DIR}/library/common
		    ${TERRARADAR_SRC_DIR}/library/segmenter
		    ${TERRARADAR_SRC_DIR}/library/classifier
		    ${TERRARADAR_SRC_DIR}/library/filter
)

file(GLOB TERRARADAR_EXEC_SRCS ${TERRARADAR_SRC_DIR}/binary/*.cpp)

# Setting Dependencies
set(TERRARADAR_EXEC_DEPENDENCIES terraradar_mod_common
				 terraradar_mod_segmenter
				 terraradar_mod_classifier
				 terraradar_mod_filter
                                 # ${Boost_SYSTEM_LIBRARY}
)

add_executable(terraradar ${TERRARADAR_EXEC_SRCS})

target_link_libraries(terraradar ${TERRARADAR_EXEC_DEPENDENCIES})

if(TERRARADAR_SOURCE_VERIFY_ENABLED)
    add_style_check_target(terraradar "${TERRARADAR_EXEC_SRCS}")
endif()

# set_target_properties(terraradar
#                       PROPERTIES VERSION ${TERRARADAR_VERSION_MAJOR}.${TERRARADAR_VERSION_MINOR}
#                                  SOVERSION ${TERRARADAR_VERSION_MAJOR}.${TERRARADAR_VERSION_MINOR}
#                                  INSTALL_NAME_DIR "@executable_path/../library")
