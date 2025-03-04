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

                // Configure Resolution
                ImGui::Text("Resolution");

                if (draft_cfg.res.fullscreen) {
                    ImGui::BeginDisabled();
                }

                static int multiplier;
                if (draft_cfg.res.downscaling) {
                    multiplier = GetMonitorHeight(draft_cfg.res.selectedMonitor) / draft_cfg.res.y;
                } else {
                    multiplier = draft_cfg.res.y / GetMonitorHeight(draft_cfg.res.selectedMonitor);
                }

                ImGui::SliderInt("##", &multiplier, 1, 4);
                ImGui::Checkbox("Downscale Resolution", &draft_cfg.res.downscaling);
                if (draft_cfg.res.downscaling) {
                    draft_cfg.res.x = GetMonitorWidth(draft_cfg.res.selectedMonitor) / multiplier;
                    draft_cfg.res.y = GetMonitorHeight(draft_cfg.res.selectedMonitor) / multiplier;
                } else {
                    draft_cfg.res.x = multiplier * GetMonitorWidth(draft_cfg.res.selectedMonitor);
                    draft_cfg.res.y = multiplier * GetMonitorHeight(draft_cfg.res.selectedMonitor);
                }
                ImGui::Text("Selected: %dx%d", draft_cfg.res.x, draft_cfg.res.y);
                if (draft_cfg.res.fullscreen) {
                    ImGui::EndDisabled();
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
