// Common GUI interfaces (settings, etc. are here)
// Inherit from ImGuiInterface to create a GUI.
#pragma once
#include "putrid.hpp"

void SettingsGUI(bool run, std::shared_ptr<AssetManager> asMgr, std::shared_ptr<ConfigManager> cfgMgr);
