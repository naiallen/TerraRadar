#
#  Description: Main CMake script for the TerraRadar.
#
#  Author: Etore Marcari Junior <etore@dpi.inpe.br>
#

cmake_minimum_required(VERSION 3.1.3)

project(terraradar)

#
# Main Variables for the build script
#

# Variables that control the system versioning
set(TERRARADAR_VERSION_MAJOR 0)
set(TERRARADAR_VERSION_MINOR 0)
set(TERRARADAR_VERSION_PATCH 0)
set(TERRARADAR_VERSION_STATUS "develop")
set(TERRARADAR_VERSION_STRING "${TERRARADAR_VERSION_MAJOR}.${TERRARADAR_VERSION_MINOR}.${TERRARADAR_VERSION_PATCH}-${TERRARADAR_VERSION_STATUS}")

# Variables that contain project directories
set(TERRARADAR_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(TERRARADAR_SRC_DIR ${TERRARADAR_ROOT_DIR}/src)
set(TERRARADAR_DOC_DIR ${TERRARADAR_ROOT_DIR}/doc)
set(TERRARADAR_SHARE_DIR ${TERRARADAR_ROOT_DIR}/share)
set(TERRARADAR_ICONS_DIR ${TERRARADAR_SHARE_DIR}/icons)
set(TERRARADAR_TESTS_DIR ${TERRARADAR_ROOT_DIR}/tests)

# variables storing absolute paths
set(TERRARADAR_ABSOLUTE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)

if(NOT DEFINED TERRARADAR_DATA_DIR)
  set(TERRARADAR_DATA_DIR "${TERRARADAR_ABSOLUTE_ROOT_DIR}/data" CACHE PATH "Path to a directory containing the data used in the examples and/or in\
 the unit test")
endif()

#
# global configurations
#
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/code_style")

add_definitions(-DBOOST_ALL_NO_LIB -DBOOST_FILESYSTEM_VERSION=3 -DBOOST_UBLAS_TYPE_CHECK=0)

#
# Where terralib project is located
#
IF(NOT DEFINED TERRALIB_DIR)
    set(TERRALIB_DIR "C:/terralib/codebase"
       	CACHE PATH "Path to the directory where TerraLib 5 is installed.")
endif()

# Defining the directory where gtest is installed
if(NOT DEFINED GTEST_ROOT)
    set(GTEST_ROOT "C:/develop/gtest-1.7.0"
        CACHE PATH "Path to the directory where GTest is installed.")
endif()

#
# including CMake's auxiliary files
#
include(CMakeDependentOption)

#
# Search for main dependencies
#
# TerraLib
find_package(TerraLib REQUIRED)
if(NOT TERRALIB_FOUND)
    message(FATAL_ERROR "Could not find required TerraLib libraries!")
endif()

# Boost
find_package(Boost 1.57 REQUIRED system date_time chrono timer filesystem thread)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find required Boost libraries!")
endif()

# Google Test
find_package(GTest)
if(NOT GTest_FOUND)
    message(WARNING "Could not find required Google Test libraries!")
endif()

# Doxygen
find_package(Doxygen QUIET)
if(NOT DOXYGEN_FOUND)
    message(WARNING "Could not find required Doxygen libraries!")
endif()

#
# Build options (what is built)
#
# Warning: please, do not change the order below!
#
if(DOXYGEN_FOUND)
  option(TERRARADAR_DOXYGEN_ENABLED  "Enable source code documentation build?" ON)
endif()

option(TERRARADAR_MOD_COMMON_ENABLED "Build Common functions module?" ON)

option(TERRARADAR_MOD_SEGMENTER_ENABLED "Build Segmenter functions module?" ON)

option(TERRARADAR_MOD_CLASSIFIER_ENABLED "Build Classifier functions module?" ON)

option(TERRARADAR_MOD_FILTER_ENABLED "Build Filter functions module?" ON)

# Application bin
cmake_dependent_option(TERRARADAR_BIN_ENABLED "Build TerraRadar Application?" ON
                       "TERRARADAR_MOD_COMMON_ENABLED;TERRARADAR_MOD_SEGMENTER_ENABLED;TERRARADAR_MOD_CLASSIFIER_ENABLED;TERRARADAR_MOD_FILTER_ENABLED"
                       OFF)

# cpplint
option(TERRARADAR_SOURCE_VERIFY_ENABLED
        "Enable the automatic verification of the source code through cpplint.py." OFF)

# GTest
if(GTEST_FOUND)
    option(TERRARADAR_UNITTEST_ENABLED "Build Unit tests for the Attributes Extraction module" ON)
endif()

# For Mac Users
if(APPLE)
    cmake_dependent_option(BUILD_TERRARADAR_AS_BUNDLE "If on, tells that the build will generate a bundle" OFF
                           "TERRARADAR_BIN_ENABLED" OFF)
endif()

#
# Setting Destination of built files
#
if(APPLE AND BUILD_TERRARADAR_AS_BUNDLE AND NOT DEFINED TERRARADAR_BASE_DESTINATION_DIR)
    set(TERRARADAR_BASE_DESTINATION_DIR "terraradar.app/Contents/")
elseif(NOT DEFINED TERRARADAR_BASE_DESTINATION_DIR)
    set(TERRARADAR_BASE_DESTINATION_DIR "")
endif()

if(NOT DEFINED TERRARADAR_DESTINATION_LIBRARY)
    set(TERRARADAR_DESTINATION_LIBRARY "${TERRARADAR_BASE_DESTINATION_DIR}lib")
endif()

if(NOT DEFINED TERRARADAR_DESTINATION_RUNTIME)
    set(TERRARADAR_DESTINATION_RUNTIME "${TERRARADAR_BASE_DESTINATION_DIR}bin")
endif()

if(NOT DEFINED TERRARADAR_DESTINATION_ARCHIVE)
    set(TERRARADAR_DESTINATION_ARCHIVE "${TERRARADAR_BASE_DESTINATION_DIR}lib")
endif()

if(NOT DEFINED TERRARADAR_DESTINATION_HEADERS)
    set(TERRARADAR_DESTINATION_HEADERS "${TERRARADAR_BASE_DESTINATION_DIR}include")
endif()

if(NOT DEFINED TERRARADAR_DESTINATION_SHARE)
    set(TERRARADAR_DESTINATION_SHARE "${TERRARADAR_BASE_DESTINATION_DIR}share")
endif()

#
# Settings to build files into a single folder
#
if((CMAKE_GENERATOR STREQUAL Xcode) OR MSVC)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
else()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

#
# global definitions and includes
#
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()

if(WIN32)
	if(MINGW)
		add_definitions(-DTE_MINGW -D_WIN32_WINNT=0x601 -fpermissive)
	endif()
elseif(APPLE)
	add_definitions(-ftemplate-depth-1024)
endif()

# Turn on multi process compilation
if(MSVC)
  add_definitions(/MP)
endif()

#
# Searching for CPP lint check dependencies
#
if(TERRARADAR_SOURCE_VERIFY_ENABLED)
    message("    -> Code style verification enabled!")
    find_package(PythonInterp REQUIRED)
    if(PYTHONINTERP_FOUND)
        include(CodeStyleCheck)
    else()
        message(FATAL_ERROR "Could not find Python interpreter. It is required to enable the CppLint check.")
    endif()
endif()

#
# Process TERRARADAR configuration files
#
configure_file(${TERRARADAR_ABSOLUTE_ROOT_DIR}/src/library/BuildConfig.hpp.in
               ${CMAKE_BINARY_DIR}/terraradar_generated/terraradar_buildconfig.hpp @ONLY)

include_directories(${CMAKE_BINARY_DIR}/terraradar_generated/)

#
# Build Modules
#
message("-- Checking Modules!")

# TerraRadar Common Module
if(TERRARADAR_MOD_COMMON_ENABLED)
    message("    -> Common module enabled!")
    add_subdirectory(terraradar_mod_common)
endif()

# TerraRadar Segmenter Module
if(TERRARADAR_MOD_SEGMENTER_ENABLED)
    message("    -> Segmenter module enabled!")
    add_subdirectory(terraradar_mod_segmenter)
endif()

# TerraRadar Classifier Module
if(TERRARADAR_MOD_CLASSIFIER_ENABLED)
    message("    -> Classifier module enabled!")
    add_subdirectory(terraradar_mod_classifier)
endif()

# TerraRadar Filter Module
if(TERRARADAR_MOD_FILTER_ENABLED)
    message("    -> Filter module enabled!")
    add_subdirectory(terraradar_mod_filter)
endif()

# TerraRadar Binary
if(TERRARADAR_BIN_EXECUTABLE)
    message("    -> TerraRadar Binary enabled")
    add_subdirectory(terraradar_bin)
endif()

# API Documentation generation with Doxygen
if(TERRARADAR_DOXYGEN_ENABLED)
    message("    -> Doxygen Documentation enabled")
    find_package(Doxygen REQUIRED)

    if(DOXYGEN_FOUND)
        add_subdirectory(terraradar_doxygen)
    else()
        message(FATAL_ERROR "Could not find Doxygen. It is needed to build the documentation.")
    endif()
endif()

#
# Build Tests
#
if(TERRARADAR_UNITTEST_ENABLED)
    message("    -> Unit Tests enabled")

    if(GTEST_FOUND)
        add_subdirectory(terraradar_unittest)
    else()
        message(FATAL_ERROR "Could not find Google Test. It is needed to build the tests.")
    endif()
endif()
