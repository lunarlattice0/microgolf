#include "putrid/putrid.hpp"
#include "putrid/luahelper/luahelper.hpp"
#include "stinky/stinky.hpp"
#include <map>
#include <raylib.h>
#include <unordered_map>
#include "../gui/style.hpp"
#include "../vendor/imgui/imgui.h"
#include "../rlImGui.h"


int main() {
    // TODO: Figure out a fancy pants menu system.
    // TODO: Figure out a fancy pants config menu

    // Load config from file
    CheckFilePaths();
    Config active_cfg = LoadConfigFromFile(MicrogolfFilePaths.at("config").c_str());

    // Calculate starting resolution
    int screenWidth = 640 * active_cfg.res.multiplier;
    int screenHeight = 360 * active_cfg.res.multiplier;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Microgolf");
    if (IsWindowFullscreen() != active_cfg.res.fullscreen) {
        ToggleFullscreen();
    }

    SetTargetFPS(60);
    InitAudioDevice();

    // Setup ImGui for configmenus
    rlImGuiSetup(true);
    ImGuiIO* io = &ImGui::GetIO();
    io->IniFilename = NULL;
    io->LogFilename = NULL;

    // Draw main menu
    Image mainmenuimg = LoadImage(MicrogolfFilePaths.at("menubg").c_str());
    ImageResize(&mainmenuimg, screenWidth, screenHeight);
    Texture2D mainmenubg = LoadTextureFromImage(mainmenuimg);
    Rectangle srcRect = {0,0,1920,1080}; // source image is 1920x1080

    bool displayConfig = false;
    // TODO: Check why imgui menus don't display on fullscreen?
    bool displayServerList = false;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTextureRec(mainmenubg, srcRect, {0,0}, WHITE);
            // ImGui tasks for drawing config windows
            {
                rlImGuiBegin();
                SetupGuiStyle();

                if (displayConfig) {
                    static Resolution current_res = active_cfg.res;

                    ImGui::Begin("Config");
                    ImGui::Text("Resolution");
                    if (current_res.fullscreen) {
                       ImGui::BeginDisabled();
                    }
                    ImGui::SliderInt("##", &current_res.multiplier, 1, 6);
                    ImGui::Text("Selected: %dx%d", current_res.multiplier * 640, current_res.multiplier * 360);
                    if (current_res.fullscreen) {
                        ImGui::EndDisabled();
                    }

                    ImGui::Checkbox("Fullscreen", &current_res.fullscreen);
                    if (ImGui::Button("Discard Changes")) {
                        displayConfig = false;
                        current_res = active_cfg.res;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Save and Close")) {
                        displayConfig = false;
                        active_cfg.res = current_res;
                        SaveConfigToFile(MicrogolfFilePaths.at("config").c_str(), active_cfg);

                        // Apply resolution changes
                        screenWidth = 640 * active_cfg.res.multiplier;
                        screenHeight =  360 * active_cfg.res.multiplier;
                        SetWindowSize(screenWidth,screenHeight);
                        if (current_res.fullscreen != IsWindowFullscreen()) {
                            ToggleFullscreen();
                        }
                    }
                    ImGui::End();
                }
                rlImGuiEnd();
            }
            // End of ImGui Section
        EndDrawing();
    }
    rlImGuiShutdown();
    UnloadTexture(mainmenubg);
    CloseAudioDevice();
    CloseWindow();
}
