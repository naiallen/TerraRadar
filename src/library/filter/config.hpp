/*  Copyright (C) 2015 National Institute For Space Research (INPE) - Brazil.

This file is part of the TerraRadar - a library and application for radar data manipulation.

TerraRadar is under development.
*/

/*!
\file terraradar/library/filter/config.h
\brief Configuration flags for TerraRadar Classifier Runtime module.
*/

#ifndef TERRARADAR_LIB_FILT_INTERNAL_CONFIG_HPP_
#define TERRARADAR_LIB_FILT_INTERNAL_CONFIG_HPP_

#define TERADAR_FILT_MODULE_NAME "terraradar.filt"

/** @name DLL/LIB Module
*  Flags for building TerraRadar as a DLL or as a Static Library
*/
//@{

/*!
\def TERADARFILTEXPORT

\brief You can use this macro in order to export/import classes and functions from this module.

\note If you want to compile TerraRadar as DLL in Windows, remember to insert TERADARFILTDLL into the
project's list of defines.

\note If you want to compile TerraRadar Filter Library as an Static Library under Windows,
remember to insert the TERADARFILTSTATIC flag into the project list of defines.
*/
#ifdef WIN32

#ifdef TERADARFILTSTATIC
#define TERADARFILTEXPORT                      // Don't need to export/import... it is a static
// library
#elif TERADARFILTDLL
#define TERADARFILTDLL  __declspec(dllexport)   // export DLL information
#else
#define TERADARFILTDLL  __declspec(dllimport)   // import DLL information
#endif 
#else
#define TERADARFILTDLL
#endif

//@}

#endif  // TERRARADAR_LIB_FILT_INTERNAL_CONFIG_HPP_