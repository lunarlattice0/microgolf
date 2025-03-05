#include "putrid/putrid.hpp"
#include "src/vendor/rlImGui/rlImGui.h"
#include "imgui.h"
#include "style.hpp"
#include <memory>
#include <raylib.h>

int main() {
    // TODO: Figure out a fancy pants menu system.
    // TODO: Figure out a fancy pants config menu

    SetExitKey(0);

    // Load config from file
    std::unique_ptr<AssetManager> asMgr = std::make_unique<AssetManager>();
    std::unique_ptr<ConfigManager> cfgMgr = std::make_unique<ConfigManager>();
    cfgMgr->SetActiveConfig(asMgr->LoadConfig());

    // Set up ImGui
    rlImGuiSetup(true);

    // Draw main menu
    Image mainmenuimg = LoadImage(asMgr->GetAssetPathByName("menubg").c_str());
    Texture2D mainmenubg = LoadTextureFromImage(mainmenuimg);

    bool displayConfig = true;
    // TODO: Check why imgui menus don't display on fullscreen?
    //bool displayServerList = false;

    while (!WindowShouldClose()) {
        ClearBackground(RAYWHITE);
        /*
        Layer 0: Gameplay
        Layer 1: Gameplay GUIs
        Layer 2: PauseMenu Texture
        Layer 3: PauseMenu GUIs
        */
        BeginDrawing();
        DrawTexturePro(mainmenubg, {0,0,1920,1080}, {0,0,static_cast<float>(GetScreenWidth()),static_cast<float>(GetScreenHeight())}, {0,0}, 0, WHITE);
        {
            rlImGuiBegin();
            SetupGuiStyle();

            if (displayConfig) {
                // Create a new draft cfg
                static Config draft_cfg = *cfgMgr->GetActiveConfig();

                // Window title
                ImGui::Begin("Config");

                ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(1.0f,0.0f,0.0f));
                ImGui::Text("Display Settings");
                ImGui::PopStyleColor();

                // Monitor selection
                ImGui::SliderInt("Monitor number", &draft_cfg.res.selectedMonitor, 0, GetMonitorCount() - 1);
                ImGui::Text("Selected Monitor: %s", GetMonitorName(draft_cfg.res.selectedMonitor));

                // Disable resolution settings if in fullscreen
                if (draft_cfg.res.fullscreen) {
                    ImGui::BeginDisabled();
                }

                // Screen Ratio Selection
                const char * ratios[RATIOCOUNTER] = {"4:3", "16:9", "16:10"};
                const char * selected_ratio = ratios[draft_cfg.res.selectedRatio]; // unsure if safe...?
                ImGui::NewLine();
                ImGui::SliderInt("Resolution Ratio", &draft_cfg.res.selectedRatio, 0, RATIOCOUNTER - 1, selected_ratio, ImGuiSliderFlags_NoInput);

                // Display Resolution Multiplier
                ImGui::SliderInt("##", &draft_cfg.res.multiplier, 1, 6);

                int baseResolutionX;
                int baseResolutionY;
                if (draft_cfg.res.selectedRatio == FOURBYTHREE) {
                    baseResolutionX = 640;
                    baseResolutionY = 480;
                } else if (draft_cfg.res.selectedRatio == SIXTEENBYNINE) {
                    baseResolutionX = 640;
                    baseResolutionY = 360;
                } else {
                    baseResolutionX = 1280;
                    baseResolutionY = 800;
                }

                draft_cfg.res.x = draft_cfg.res.multiplier * baseResolutionX;
                draft_cfg.res.y = draft_cfg.res.multiplier * baseResolutionY;

                ImGui::Text("Selected: %dx%d", draft_cfg.res.x, draft_cfg.res.y);

                // Disable and reset resolution controls if fullscreen is enabled. otherwise, the window breaks.
                if (draft_cfg.res.fullscreen) {
                    ImGui::EndDisabled();
                    draft_cfg.res.x = GetMonitorWidth(draft_cfg.res.selectedMonitor);
                    draft_cfg.res.y = GetMonitorHeight(draft_cfg.res.selectedMonitor);
                }

                // Framerate selection
                ImGui::InputInt("Target FPS", &draft_cfg.res.targetFPS);

                // Fullscreen setting
                ImGui::Checkbox("Fullscreen", &draft_cfg.res.fullscreen);
                if (ImGui::Button("Discard Changes")) {
                    displayConfig = false;
                    draft_cfg = *cfgMgr->GetActiveConfig();
                }

                // Closing buttons
                ImGui::SameLine();
                if (ImGui::Button("Save and Close")) {
                    displayConfig = false;
                    cfgMgr->SetActiveConfig(draft_cfg);
                    asMgr->SaveConfig(*cfgMgr->GetActiveConfig());
                }
                ImGui::SameLine();
                if (ImGui::Button("Apply")) { // consider enabling autorevert
                    cfgMgr->SetActiveConfig(draft_cfg);
                    asMgr->SaveConfig(*cfgMgr->GetActiveConfig());
                }

                // TODO: Consider adding resiziable window...
                ImGui::End();
            }
            rlImGuiEnd();
        }
        EndDrawing();

    }
    rlImGuiShutdown();
    UnloadTexture(mainmenubg);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
