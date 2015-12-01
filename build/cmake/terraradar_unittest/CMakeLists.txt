# Description: Build configuration for the Unit tests of TerraRadar library and application.
#
#  Author: Etore Marcari Jr. <etore@dpi.inpe.br>
#          Raian Vargas Maretto <raian@dpi.inpe.br>
#

# Includes
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${terralib_DIR})
include_directories(${terralib_INCLUDE_DIRS})
include_directories(${GTEST_INCLUDE_DIRS})

#Set include directories location
include_directories(${TERRARADAR_SRC_DIR}/library/segmenter
		    ${TERRARADAR_SRC_DIR}/library/classifier
		    ${TERRARADAR_SRC_DIR}/library/filter
            ${TERRARADAR_SRC_DIR}/library/common
)

# Source files
file(GLOB TERRARADAR_UNITTEST_COMMON_SRCS ${TERRARADAR_TESTS_DIR}/unittest/common/*.cpp)
file(GLOB TERRARADAR_UNITTEST_SEGM_SRCS ${TERRARADAR_TESTS_DIR}/unittest/segmenter/*.cpp)
file(GLOB TERRARADAR_UNITTEST_CLASS_SRCS ${TERRARADAR_TESTS_DIR}/unittest/classifier/*.cpp)
file(GLOB TERRARADAR_UNITTEST_FILT_SRCS ${TERRARADAR_TESTS_DIR}/unittest/filter/*.cpp)

# Creating source groups for IDEs
# source_group("Source Files")
source_group("Source Files\\common"  FILES ${TERRARADAR_UNITTEST_COMMON_SRCS})
source_group("Source Files\\segmenter"  FILES ${TERRARADAR_UNITTEST_SEGM_SRCS})
source_group("Source Files\\classifier"  FILES ${TERRARADAR_UNITTEST_CLASS_SRCS})
source_group("Source Files\\filter"  FILES ${TERRARADAR_UNITTEST_FILT_SRCS})

# Setting Dependencies
set(TERRARADAR_UNITTEST_LIBS_DEPENDENCIES terraradar_mod_common
				                          terraradar_mod_segmenter
					                      terraradar_mod_classifier
					                      terraradar_mod_filter
					                      ${GTEST_BOTH_LIBRARIES}
)

add_executable(terraradar_unittest ${TERRARADAR_UNITTEST_COMMON_SRCS} ${TERRARADAR_UNITTEST_SEGM_SRCS} 
					 ${TERRARADAR_UNITTEST_CLASS_SRCS} ${TERRARADAR_UNITTEST_FILT_SRCS})

target_link_libraries(terraradar_unittest ${TERRARADAR_UNITTEST_LIBS_DEPENDENCIES})


if(TERRARADAR_SOURCE_VERIFY_ENABLED)
    add_style_check_target(terraradar_unittest "${TERRARADAR_UNITTEST_COMMON_SRCS} ${TERRARADAR_UNITTEST_SEGM_SRCS} 
					 ${TERRARADAR_UNITTEST_CLASS_SRCS} ${TERRARADAR_UNITTEST_FILT_SRCS}")
endif()
