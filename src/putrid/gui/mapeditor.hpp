#pragma once
// TODO: When in actual gameplay, use a separate lua_state thread...


// Probably fine to inline this...used in literally one place anyway.
#include "imgui.h"
#include "mapeditor/mapeditor.hpp"
#include <filesystem>
#include <memory>
#include <raylib.h>
#include <stdexcept>
#include "vendor/imgui/misc/cpp/imgui_stdlib.h"

inline void MapEditorGUI(std::shared_ptr<MapEditor> me) {
    ImGui::Begin("Map Editor", NULL, ImGuiWindowFlags_NoSavedSettings
    );

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
        if (ImGui::BeginTabItem("Lua Script")) {
            ImGui::Text("Game Script");
            static std::string luaScript;
            static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
            ImGui::InputTextMultiline("LuaScriptEntry", &luaScript, ImVec2(ImGui::GetWindowSize().x - 16, 256), flags, NULL, NULL);

            if (ImGui::Button ("Run")) {
                me->RunScript(luaScript);
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Project Management")) {
            static std::string pmpath;
            ImGui::InputTextWithHint("Project Path", "Existing / New Path for Project", &pmpath);
            ImGui::SameLine();
            if (ImGui::Button("Load / Create")) {
                try {
                    me->LoadMap(pmpath.c_str());
                } catch (const std::runtime_error& e) {
                    TraceLog(LOG_ERROR, "Couldn't open the map file!");
                }
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
};
