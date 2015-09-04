/*  Copyright (C) 2015 National Institute For Space Research (INPE) - Brazil.

This file is part of the TerraRadar - a library and application for radar data manipulation.

TerraRadar is under development.
*/

/*!
\file terraradar/library/segmenter/config.h
\brief Configuration flags for TerraRadar Classifier Runtime module.
*/

#ifndef TERRARADAR_LIB_SEGM_INTERNAL_CONFIG_HPP_
#define TERRARADAR_LIB_SEGM_INTERNAL_CONFIG_HPP_

#define TERADAR_SEGM_MODULE_NAME "terraradar.segm"

/** @name DLL/LIB Module
*  Flags for building TerraRadar as a DLL or as a Static Library
*/
//@{

/*!
\def TERADARSEGMEXPORT

\brief You can use this macro in order to export/import classes and functions from this module.

\note If you want to compile TerraRadar as DLL in Windows, remember to insert TERADARSEGMDLL into the
project's list of defines.

\note If you want to compile TerraRadar Segmenter Library as an Static Library under Windows,
remember to insert the TERADARSEGMSTATIC flag into the project list of defines.
*/
#ifdef WIN32

#ifdef TERADARSEGMSTATIC
#define TERADARSEGMEXPORT                      // Don't need to export/import... it is a static
// library
#elif TERADARSEGMDLL
#define TERADARSEGMDLL  __declspec(dllexport)   // export DLL information
#else
#define TERADARSEGMDLL  __declspec(dllimport)   // import DLL information
#endif 
#else
#define TERADARSEGMDLL
#endif

//@}

#endif  // TERRARADAR_LIB_SEGM_INTERNAL_CONFIG_HPP_