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

    ImGui::Text("Welcome to the Map Editor!\nPress / to unlock or lock the mouse.");

    ImGui::End();
};
