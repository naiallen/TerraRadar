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
file(GLOB TERRARADAR_EXEC_HDRS ${TERRARADAR_SRC_DIR}/binary/*.hpp)

# Creating source groups for IDEs
source_group("Source Files"  FILES ${TERRARADAR_EXEC_SRCS})
source_group("Header Files"  FILES ${TERRARADAR_EXEC_HDRS})

# Setting Dependencies
set(TERRARADAR_EXEC_DEPENDENCIES terraradar_mod_common
				 terraradar_mod_segmenter
				 terraradar_mod_classifier
				 terraradar_mod_filter
)

# Setting Dependencies
set(TERRARADAR_EXEC_LIB_DEPENDENCIES ${Boost_SYSTEM_LIBRARY}
                                     ${TERRALIB_COMMON_LIBRARY}
                                     ${TERRALIB_PLUGIN_LIBRARY}
                                     ${TERRALIB_RP_LIBRARY}
                                     ${TERRALIB_RASTER_LIBRARY}
                                     ${TERRALIB_QT_WIDGETS_LIBRARY}
                                     ${TERRALIB_QT_APF_LIBRARY}
)

set(TERRARADAR_EXEC_QT_DEPENDENCIES Qt5::Widgets Qt5::Gui Qt5::Core)

add_executable(terraradar ${TERRARADAR_EXEC_HDRS} ${TERRARADAR_EXEC_SRCS})

target_link_libraries(terraradar ${TERRARADAR_EXEC_DEPENDENCIES} 
	                             ${TERRARADAR_EXEC_QT_DEPENDENCIES}
	                             ${TERRARADAR_EXEC_LIB_DEPENDENCIES}
	                             ${QT_LIBRARIES})

if(TERRARADAR_SOURCE_VERIFY_ENABLED)
    add_style_check_target(terraradar "${TERRARADAR_EXEC_SRCS}")
endif()

# set_target_properties(terraradar
#                       PROPERTIES VERSION ${TERRARADAR_VERSION_MAJOR}.${TERRARADAR_VERSION_MINOR}
#                                  SOVERSION ${TERRARADAR_VERSION_MAJOR}.${TERRARADAR_VERSION_MINOR}
#                                  INSTALL_NAME_DIR "@executable_path/../library")
