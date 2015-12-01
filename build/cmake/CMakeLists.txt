#
#  Description: Main CMake script for the TerraRadar.
#
#  Author: Etore Marcari Junior <etore@dpi.inpe.br>
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

PROJECT(terraradar)

#
# Main Variables for the build script
#

# Variables that control the system versioning
SET(TERRARADAR_VERSION_MAJOR 0)
SET(TERRARADAR_VERSION_MINOR 0)
SET(TERRARADAR_VERSION_PATCH 0)
SET(TERRARADAR_VERSION_STATUS "develop")
SET(TERRARADAR_VERSION_STRING "${TERRARADAR_VERSION_MAJOR}.${TERRARADAR_VERSION_MINOR}.${TERRARADAR_VERSION_PATCH}-${TERRARADAR_VERSION_STATUS}")

# Variables that contain project directories
SET(TERRARADAR_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
SET(BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)
SET(TERRARADAR_SRC_DIR ${TERRARADAR_ROOT_DIR}/src)
SET(TERRARADAR_DOC_DIR ${TERRARADAR_ROOT_DIR}/doc)
SET(TERRARADAR_SHARE_DIR ${TERRARADAR_ROOT_DIR}/share)
SET(TERRARADAR_ICONS_DIR ${TERRARADAR_SHARE_DIR}/icons)
SET(TERRARADAR_TESTS_DIR ${TERRARADAR_ROOT_DIR}/tests)

# variables storing absolute paths
SET(TERRARADAR_ABSOLUTE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)

IF(NOT DEFINED TERRARADAR_DATA_DIR)
  SET(TERRARADAR_DATA_DIR "${TERRARADAR_ABSOLUTE_ROOT_DIR}/data" CACHE PATH "Path to a directory containing the data used in the examples and/or in\
 the unit test")
ENDIF()

#
# global configurations
#
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules")
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/code_style")

#
# including CMake's auxiliary files
#
INCLUDE(CMakeDependentOption)

#
# Where terralib project is located
#
IF(NOT DEFINED terralib_DIR)
    IF(WIN32)
        SET(terralib_DIR "C:/terralib/build-win64"
          CACHE PATH "Path to the directory where TerraLib 5 is installed.")
    ELSE()
        SET(terralib_DIR "/usr/local/lib/terralib"
          CACHE PATH "Path to the directory where TerraLib 5 is installed.")
    ENDIF()
ENDIF()

# Defining the directory where gtest is installed
IF(NOT DEFINED GTEST_ROOT)
    IF(WIN32)
        SET(GTEST_ROOT "C:/develop/gtest-1.7.0"
            CACHE PATH "Path to the directory where GTest is installed.")
    ELSE()
        SET(GTEST_ROOT "/usr/local/gtest"
            CACHE PATH "Path to the directory where GTest is installed.")
    ENDIF()
ENDIF()

#
# Search for main dependencies
#
# TerraLib
FIND_PACKAGE(terralib REQUIRED)
IF(NOT terralib_FOUND)
    MESSAGE(FATAL_ERROR "Could not find required TerraLib libraries!")
ELSE()
    MESSAGE(STATUS "Found TerraLib library in ${terralib_DIR}")
ENDIF()

# Boost
FIND_PACKAGE(Boost 1.57 REQUIRED system date_time chrono timer filesystem thread)
IF(NOT Boost_FOUND)
    MESSAGE(FATAL_ERROR "Could not find required Boost libraries!")
ENDIF()

# Google Test
FIND_PACKAGE(GTest)
IF(NOT GTest_FOUND)
    MESSAGE(WARNING "Could not find required Google Test libraries!")
ENDIF()

# Qt
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

FIND_PACKAGE(Qt5Widgets)
IF(Qt5Widgets_FOUND)
	MESSAGE(STATUS "Found Qt5 Widgets libraries!")
    INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDE_DIRS})
    ADD_DEFINITIONS(${Qt5Widgets_DEFINITIONS})
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
ELSE()
	MESSAGE(FATAL_ERROR "Could not find required Qt Widgets libraries!")
ENDIF()

FIND_PACKAGE(Qt5Core)
IF(Qt5Core_FOUND)
	MESSAGE(STATUS "Found Qt5 Core libraries!")
    INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS})
    ADD_DEFINITIONS(${Qt5Core_DEFINITIONS})
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
ELSE()
	MESSAGE(FATAL_ERROR "Could not find required Qt Core libraries!")
ENDIF()

FIND_PACKAGE(Qt5Gui)
IF(Qt5Gui_FOUND)
	MESSAGE(STATUS "Found Qt5 Gui libraries!")
    INCLUDE_DIRECTORIES(${Qt5Gui_INCLUDE_DIRS})
    ADD_DEFINITIONS(${Qt5Gui_DEFINITIONS})
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Gui_EXECUTABLE_COMPILE_FLAGS}")
ELSE()
	MESSAGE(FATAL_ERROR "Could not find required Qt Gui libraries!")
ENDIF()

IF(NOT QT_QCOLLECTIONGENERATOR_EXECUTABLE)
  FIND_PROGRAM(QT_QCOLLECTIONGENERATOR_EXECUTABLE
               NAME qcollectiongenerator
               PATHS /usr
                     /usr/local
                     ${Qt5_DIR}/../../..
               PATH_SUFFIXES bin)
ENDIF()

#
# Build options (what is built)
#
# Warning: please, do not change the order below!
#
OPTION(TERRARADAR_MOD_COMMON_ENABLED "Build Common functions module?" ON)

OPTION(TERRARADAR_MOD_SEGMENTER_ENABLED "Build Segmenter functions module?" ON)

OPTION(TERRARADAR_MOD_CLASSIFIER_ENABLED "Build Classifier functions module?" ON)

OPTION(TERRARADAR_MOD_FILTER_ENABLED "Build Filter functions module?" ON)

# Doxygen
OPTION(TERRARADAR_DOXYGEN_ENABLED  "Enable source code documentation build?" OFF)

# Application bin
CMAKE_DEPENDENT_OPTION(TERRARADAR_BIN_ENABLED "Build TerraRadar Application?" ON
                       "TERRARADAR_MOD_COMMON_ENABLED;TERRARADAR_MOD_SEGMENTER_ENABLED;TERRARADAR_MOD_CLASSIFIER_ENABLED;TERRARADAR_MOD_FILTER_ENABLED"
                       OFF)

# cpplint
OPTION(TERRARADAR_SOURCE_VERIFY_ENABLED
        "Enable the automatic verification of the source code through cpplint.py." OFF)

# GTest
IF(GTEST_FOUND)
    OPTION(TERRARADAR_UNITTEST_ENABLED "Build Unit tests for the Attributes Extraction module" ON)
ENDIF()

# For Mac Users
IF(APPLE)
    cmake_dependent_OPTION(BUILD_TERRARADAR_AS_BUNDLE "If on, tells that the build will generate a bundle" OFF
                           "TERRARADAR_BIN_ENABLED" OFF)
ENDIF()

#
# Setting Destination of built files
#
IF(APPLE AND BUILD_TERRARADAR_AS_BUNDLE AND NOT DEFINED TERRARADAR_BASE_DESTINATION_DIR)
    SET(TERRARADAR_BASE_DESTINATION_DIR "terraradar.app/Contents/")
elseIF(NOT DEFINED TERRARADAR_BASE_DESTINATION_DIR)
    SET(TERRARADAR_BASE_DESTINATION_DIR "")
ENDIF()

IF(NOT DEFINED TERRARADAR_DESTINATION_LIBRARY)
    SET(TERRARADAR_DESTINATION_LIBRARY "${TERRARADAR_BASE_DESTINATION_DIR}lib")
ENDIF()

IF(NOT DEFINED TERRARADAR_DESTINATION_RUNTIME)
    SET(TERRARADAR_DESTINATION_RUNTIME "${TERRARADAR_BASE_DESTINATION_DIR}bin")
ENDIF()

IF(NOT DEFINED TERRARADAR_DESTINATION_ARCHIVE)
    SET(TERRARADAR_DESTINATION_ARCHIVE "${TERRARADAR_BASE_DESTINATION_DIR}lib")
ENDIF()

IF(NOT DEFINED TERRARADAR_DESTINATION_HEADERS)
    SET(TERRARADAR_DESTINATION_HEADERS "${TERRARADAR_BASE_DESTINATION_DIR}include")
ENDIF()

IF(NOT DEFINED TERRARADAR_DESTINATION_SHARE)
    SET(TERRARADAR_DESTINATION_SHARE "${TERRARADAR_BASE_DESTINATION_DIR}share")
ENDIF()

#
# Settings to build files into a single folder
#
IF((CMAKE_GENERATOR STREQUAL Xcode) OR MSVC)
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
ELSE()
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
ENDIF()

#
# global definitions and includes
#
ADD_DEFINITIONS(-DBOOST_ALL_NO_LIB -DBOOST_FILESYSTEM_VERSION=3 -DBOOST_UBLAS_TYPE_CHECK=0)

IF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
ENDIF()

IF(WIN32)
    IF(MINGW)
        ADD_DEFINITIONS(-DTE_MINGW -D_WIN32_WINNT=0x601 -fpermissive)
    ENDIF()
elseIF(APPLE)
    ADD_DEFINITIONS(-ftemplate-depth-1024)
ENDIF()

# Turn on multi process compilation
IF(MSVC)
    ADD_DEFINITIONS(/MP)
ENDIF()

#
# Searching for CPP lint check dependencies
#
IF(TERRARADAR_SOURCE_VERIFY_ENABLED)
    MESSAGE("    -> Code style verification enabled!")
    FIND_PACKAGE(PythonInterp REQUIRED)
    IF(PYTHONINTERP_FOUND)
        INCLUDE(CodeStyleCheck)
    ELSE()
        MESSAGE(FATAL_ERROR "Could not find Python interpreter. It is required to enable the CppLint check.")
    ENDIF()
ENDIF()

#
# Process TERRARADAR configuration files
#
CONFIGURE_FILE(${TERRARADAR_ABSOLUTE_ROOT_DIR}/src/library/BuildConfig.hpp.in
               ${CMAKE_BINARY_DIR}/terraradar_generated/terraradar_buildconfig.hpp @ONLY)

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/terraradar_generated/)

#
# Build Modules
#
MESSAGE("-- Checking Modules!")

# TerraRadar Common Module
IF(TERRARADAR_MOD_COMMON_ENABLED)
    MESSAGE("    -> Common module enabled!")
    ADD_SUBDIRECTORY(terraradar_mod_common)
ENDIF()

# TerraRadar Segmenter Module
IF(TERRARADAR_MOD_SEGMENTER_ENABLED)
    MESSAGE("    -> Segmenter module enabled!")
    ADD_SUBDIRECTORY(terraradar_mod_segmenter)
ENDIF()

# TerraRadar Classifier Module
IF(TERRARADAR_MOD_CLASSIFIER_ENABLED)
    MESSAGE("    -> Classifier module enabled!")
    ADD_SUBDIRECTORY(terraradar_mod_classifier)
ENDIF()

# TerraRadar Filter Module
IF(TERRARADAR_MOD_FILTER_ENABLED)
    MESSAGE("    -> Filter module enabled!")
    ADD_SUBDIRECTORY(terraradar_mod_filter)
ENDIF()

# TerraRadar Binary
IF(TERRARADAR_BIN_ENABLED)
    MESSAGE("    -> TerraRadar Binary enabled")
    ADD_SUBDIRECTORY(terraradar_binary)
ENDIF()

# Doxygen
IF(TERRARADAR_DOXYGEN_ENABLED)
    MESSAGE("    -> Doxygen Documentation enabled")
    FIND_PACKAGE(Doxygen REQUIRED)
    IF(DOXYGEN_FOUND)
        ADD_SUBDIRECTORY(terraradar_doxygen)
    ELSE()
        MESSAGE(FATAL_ERROR "Could not find Doxygen. It is needed to build the documentation.")
    ENDIF()
ENDIF()

#
# Build Tests
#
IF(TERRARADAR_UNITTEST_ENABLED)
    MESSAGE("    -> Unit Tests enabled!")
    IF(GTEST_FOUND)
        ADD_SUBDIRECTORY(terraradar_unittest)
    ELSE()
        MESSAGE(FATAL_ERROR "Could not find Google Test. It is needed to build the tests.")
    ENDIF()
ENDIF()
