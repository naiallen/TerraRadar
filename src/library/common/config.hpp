/*  Copyright (C) 2015 National Institute For Space Research (INPE) - Brazil.

This file is part of the TerraRadar - a library and application for radar data manipulation.

TerraRadar is under development.
*/

/*!
\file terraradar/library/common/config.h
\brief Configuration flags for TerraRadar Common Runtime module.
*/

#ifndef TERRARADAR_LIB_COMMON_INTERNAL_CONFIG_HPP_
#define TERRARADAR_LIB_COMMON_INTERNAL_CONFIG_HPP_

#define TERADAR_COMMON_MODULE_NAME "terraradar.common"

/** @name DLL/LIB Module
*  Flags for building TerraRadar as a DLL or as a Static Library
*/
//@{

/*!
\def TERADARCOMMONEXPORT

\brief You can use this macro in order to export/import classes and functions from this module.

\note If you want to compile TerraRadar as DLL in Windows, remember to insert TERADARCOMMONDLL into the
project's list of defines.

\note If you want to compile TerraRadar Common Library as an Static Library under Windows,
remember to insert the TERADARCOMMONSTATIC flag into the project list of defines.
*/
#ifdef WIN32

#ifdef TERADARCOMMONSTATIC
#define TERADARCOMMONEXPORT                      // Don't need to export/import... it is a static
// library
#elif TERADARCOMMONDLL
#define TERADARCOMMONEXPORT  __declspec(dllexport)   // export DLL information
#else
#define TERADARCOMMONEXPORT  __declspec(dllimport)   // import DLL information
#endif 
#else
#define TERADARCOMMONEXPORT
#endif

//@}

#endif  // TERRARADAR_LIB_COMMON_INTERNAL_CONFIG_HPP_
