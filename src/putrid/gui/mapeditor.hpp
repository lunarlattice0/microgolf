#pragma once

// Probably fine to inline this...used in literally one place anyway.
#include "imgui.h"
#include <raylib.h>

inline void MapEditorGUI() {
    ImGui::Begin("Map Editor", NULL, ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_AlwaysAutoResize);

    // Disable imgui config
    ImGuiIO* io = &ImGui::GetIO();
    io->IniFilename = NULL;
    io->LogFilename = NULL;

    if (ImGui::BeginTabBar("MapEditorTabBar")) {
        if (ImGui::BeginTabItem("Welcome")) {
            ImGui::Text("Welcome to the Map Editor!\n"
            "Press / to unlock or lock the mouse.\n"
            "Space/Control to ascend or descend.\n"
            "MWheelUp/Down to zoom in and zoom out.");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
};
