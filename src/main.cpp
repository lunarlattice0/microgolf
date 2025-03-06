#include "putrid/putrid.hpp"
#include "src/vendor/rlImGui/rlImGui.h"
#include "common.hpp"
#include "style.hpp"
#include <memory>
#include <raylib.h>

int main() {
    // TODO: Figure out a fancy pants menu system.
    // TODO: Figure out a fancy pants config menu

    SetExitKey(0);

    // Load config from file
    std::shared_ptr<AssetManager> asMgr = std::make_unique<AssetManager>();
    std::shared_ptr<ConfigManager> cfgMgr = std::make_unique<ConfigManager>();
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
        /*
        Layer 0: Gameplay
        Layer 1: Gameplay GUIs
        Layer 2: PauseMenu Texture
        Layer 3: PauseMenu GUIs
        */

        // Draw GUI/2D elements
        ClearBackground(RAYWHITE);
        BeginDrawing();

        // Layer 1
        // End Layer 1

        // Layer 2
        DrawTexturePro(mainmenubg, {0,0,1920,1080}, {0,0,static_cast<float>(GetScreenWidth()),static_cast<float>(GetScreenHeight())}, {0,0}, 0, WHITE);
        // End Layer 2

        // Layer 3
        {
            rlImGuiBegin();
            SetupGuiStyle();
            {
                SettingsGUI(displayConfig, asMgr, cfgMgr);
            }
            rlImGuiEnd();
        }
        EndDrawing();
        // End Layer 3

    }

    // Cleanup
    rlImGuiShutdown();
    UnloadTexture(mainmenubg);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
