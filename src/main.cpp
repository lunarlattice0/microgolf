// TODO: Scale RayGui Button fonts
// TODO: Track state somehow..

#include "putrid/putrid.hpp"
#include "src/vendor/rlImGui/rlImGui.h"
#include "src/putrid/gui/common.hpp"
#include "src/putrid/gui/style.hpp"
#include <memory>
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "src/vendor/raygui/src/raygui.h"
#include "src/putrid/mapeditor/mapeditor.hpp"

int main() {

    // Load config from file
    std::shared_ptr<AssetManager> asMgr = std::make_unique<AssetManager>();
    std::shared_ptr<ConfigManager> cfgMgr = std::make_unique<ConfigManager>();
    cfgMgr->SetActiveConfig(cfgMgr->LoadConfig(asMgr.get()));

    // Set up ImGui
    rlImGuiSetup(true);

    // State variables
    static bool displayPauseMenu = true;
    static bool displaySettings = false;
    // static bool displayServerList = false;

    // Background image for pause menu
    static Image mainmenuimg = LoadImage(asMgr->GetAssetPathByName("menubg").c_str());
    static Texture2D mainmenubg = LoadTextureFromImage(mainmenuimg);

    // State variables
    std::unique_ptr<MapEditor> me = nullptr;

    // Main loop
    while (!WindowShouldClose()) {
        /*
        Layer 0: Gameplay
        Layer 1: Gameplay GUIs
        Layer 2: PauseMenu Texture / Buttons
        Layer 3: PauseMenu Submenus
        */

        // Layer 0
        if (me != nullptr) {
            BeginMode3D(*me->GetCamera());
            me->Loop();
            EndMode3D();
        }

        // Draw GUI/2D elements
        BeginDrawing();

        // Layer 1
        // End Layer 1

        // Layer 2
        if (displayPauseMenu) {
            DrawTexturePro(mainmenubg, {0,0,1920,1080}, {0,0,static_cast<float>(GetScreenWidth()),static_cast<float>(GetScreenHeight())}, {0,0}, 0, WHITE);

            // Draw the main panel in the lower quarter of the screen, with a margin
            Rectangle quarterPanelButton = {
                // X, Y, Width, Height
                (GetScreenWidth() * 0.05f),
                (GetScreenHeight() * 0.6f),
                GetScreenWidth() * 0.2f,
                GetScreenHeight() * 0.1f,
            };
            if (GuiButton(quarterPanelButton, "Map Editor")) {
                displayPauseMenu = false;
                me = std::make_unique<MapEditor>();
            }

            quarterPanelButton.y = quarterPanelButton.y + quarterPanelButton.height + 5;
            if (GuiButton(quarterPanelButton, "Settings")) {
                displaySettings = true;
            }
        }
        // End Layer 2

        // Layer 3
        {
            rlImGuiBegin();
            SetupGuiStyle();
            {
                SettingsGUI(&displaySettings, asMgr, cfgMgr);
            }
            rlImGuiEnd();
        }

        // End Layer 3

        EndDrawing();
    }

    // Cleanup
    rlImGuiShutdown();
    UnloadTexture(mainmenubg);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
