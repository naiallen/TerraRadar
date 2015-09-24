# Description: This module search for TerraLib Library
#
#  Author: Etore Marcari Jr. <etore@dpi.inpe.br>
#          Raian Vargas Maretto <raian@dpi.inpe.br>
#

cmake_minimum_required(VERSION 3.1.3)

# Search for header files
find_path(TERRALIB_INCLUDE_DIR terralib.h
   HINTS  ${TERRALIB_DIR}
          ${TERRALIB_DIR}/src
   PATH_SUFFIXES include
)

# Search for include config files
find_path(TERRALIB_INCLUDE_CONFIG_DIR terralib_config.h
   HINTS  ${TERRALIB_DIR}
          ${TERRALIB_DIR}/build-files
          ${TERRALIB_DIR}/build-makefile
          ${TERRALIB_DIR}/built
          ${TERRALIB_DIR}/../build-win64
   PATH_SUFFIXES include/terralib
)

# Find common module library - - tries to find *.a,*.so,*.dylib in paths hard-coded by the script
find_library(TERRALIB_COMMON_LIBRARY
   NAMES terralib_mod_common
   PATHS ${TERRALIB_DIR}
         ${TERRALIB_DIR}/build-files/Release
         ${TERRALIB_DIR}/build-makefile/bin
   PATH_SUFFIXES lib
)

# Find Plugin Module library - - tries to find *.a,*.so,*.dylib in paths hard-coded by the script
find_library(TERRALIB_PLUGIN_LIBRARY
   NAMES terralib_mod_plugin
   PATHS ${TERRALIB_DIR}
         ${TERRALIB_DIR}/build-files/Release
         ${TERRALIB_DIR}/build-makefile/bin
   PATH_SUFFIXES lib
)

# Find raster module library - - tries to find *.a,*.so,*.dylib in paths hard-coded by the script
find_library(TERRALIB_RASTER_LIBRARY
   NAMES terralib_mod_raster
   PATHS ${TERRALIB_DIR}
         ${TERRALIB_DIR}/build-files/Release
         ${TERRALIB_DIR}/build-makefile/bin
   PATH_SUFFIXES lib
)

# Find Raster Processing Module library - - tries to find *.a,*.so,*.dylib in paths hard-coded by the script
find_library(TERRALIB_RP_LIBRARY
   NAMES terralib_mod_rp
   PATHS ${TERRALIB_DIR}
         ${TERRALIB_DIR}/build-files/Release
         ${TERRALIB_DIR}/build-makefile/bin
   PATH_SUFFIXES lib
)

# Find Qt Widgets Module library - - tries to find *.a,*.so,*.dylib in paths hard-coded by the script
find_library(TERRALIB_QT_WIDGETS_LIBRARY
   NAMES terralib_mod_qt_widgets
   PATHS ${TERRALIB_DIR}
         ${TERRALIB_DIR}/build-files/Release
         ${TERRALIB_DIR}/build-makefile/bin
   PATH_SUFFIXES lib
)

# Find Qt Application Framework Module library - - tries to find *.a,*.so,*.dylib in paths hard-coded by the script
find_library(TERRALIB_QT_APF_LIBRARY
   NAMES terralib_mod_qt_apf
   PATHS ${TERRALIB_DIR}
         ${TERRALIB_DIR}/build-files/Release
         ${TERRALIB_DIR}/build-makefile/bin
   PATH_SUFFIXES lib
)

# Verify if all terralib libraries were found
message("-- Looking for TerraLib Library.")
if(TERRALIB_INCLUDE_DIR)
	message("   Found include dir: ${TERRALIB_INCLUDE_DIR}")
else()
	message("   Could not find include dir")
endif()

if(TERRALIB_INCLUDE_CONFIG_DIR)
	message("   Found include config dir: ${TERRALIB_INCLUDE_CONFIG_DIR}")
else()
	message("   Could not find include config dir")
endif()

if(TERRALIB_COMMON_LIBRARY)
	message("   Found Common Module library: ${TERRALIB_COMMON_LIBRARY}")
else()
	message("   Could not find Common Module library")
endif()

if(TERRALIB_PLUGIN_LIBRARY)
	message("   Found Plugin Module library: ${TERRALIB_PLUGIN_LIBRARY}")
else()
	message("   Could not find Plugin Module library")
endif()

if(TERRALIB_RASTER_LIBRARY)
	message("   Found Raster Module library: ${TERRALIB_RASTER_LIBRARY}")
else()
	message("   Could not find Raster Module library")
endif()

if(TERRALIB_RP_LIBRARY)
	message("   Found Raster Processing Module library: ${TERRALIB_RP_LIBRARY}")
else()
	message("   Could not find Raster Processing Module library")
endif()

if(TERRALIB_QT_WIDGETS_LIBRARY)
  message("   Found Qt Widgets Module library: ${TERRALIB_QT_WIDGETS_LIBRARY}")
else()
  message("   Could not find Qt Widgets Module library")
endif()

if(TERRALIB_QT_APF_LIBRARY)
  message("   Found Qt Application Framework Module library: ${TERRALIB_QT_APF_LIBRARY}")
else()
  message("   Could not find Qt Application Framework Module library")
endif()

# Export include and library path for linking with other libraries
if(TERRALIB_INCLUDE_DIR AND TERRALIB_INCLUDE_CONFIG_DIR AND TERRALIB_COMMON_LIBRARY AND 
	TERRALIB_PLUGIN_LIBRARY AND TERRALIB_RASTER_LIBRARY AND TERRALIB_RP_LIBRARY AND TERRALIB_QT_APF_LIBRARY
  AND TERRALIB_QT_WIDGETS_LIBRARY)
	set(TERRALIB_FOUND TRUE)
else()
	set(TERRALIB_FOUND FALSE)
endif()

mark_as_advanced( TERRALIB_INCLUDE_DIR  TERRALIB_INCLUDE_CONFIG_DIR  TERRALIB_COMMON_LIBRARY  
				  TERRALIB_PLUGIN_LIBRARY  TERRALIB_RASTER_LIBRARY  TERRALIB_RP_LIBRARY  TERRALIB_QT_APF_LIBRARY
          TERRALIB_QT_WIDGETS_LIBRARY)
