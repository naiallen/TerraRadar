/*  Copyright (C) 2015 National Institute For Space Research (INPE) - Brazil.

This file is part of the TerraRadar - a library and application for radar data manipulation.

TerraRadar is under development.
*/

/*!
\file terraradar/library/common/Utils.cpp
\brief Useful functions and procedures.
*/

// TerraLib Includes
#include <terralib/plugin.h>
#include <terralib/common/PlatformUtils.h>

// STL Includes
#include <string>

// TerraRadar Includes
#include "Utils.hpp"

// Load TerraLib Modules
void teradar::common::loadTerraLibDrivers() {
  te::plugin::PluginInfo* info;
  std::string plugins_path = te::common::FindInTerraLibPath("share/terralib/plugins");

  info = te::plugin::GetInstalledPlugin(plugins_path + "/te.da.gdal.teplg");
  te::plugin::PluginManager::getInstance().add(info);

  info = te::plugin::GetInstalledPlugin(plugins_path + "/te.da.ogr.teplg");
  te::plugin::PluginManager::getInstance().add(info);

  te::plugin::PluginManager::getInstance().loadAll();
}
