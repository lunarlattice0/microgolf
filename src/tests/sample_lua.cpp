// TEST FOR LUA

#include "luahelper/luahelper.hpp"
#include "luacode.h"
#include "lualib.h"
#include "lua.h"

#include <raylib.h>

#include "../vendor/imgui/imgui.h"
#include "../rlImGui.h"
#include "misc/cpp/imgui_stdlib.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "gui/imgui_lua.hpp"
#include "style.hpp"

int main(void) {
    const int screenWidth = 640;
    const int screenHeight = 480;
    // the holy resolution

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Test");
    SetTargetFPS(60);

    rlImGuiSetup(true);
    ImGuiIO* io = &ImGui::GetIO();
    io->IniFilename = NULL;
    io->LogFilename = NULL;

    // Set up Lua env
    LuauHelper * luau = new LuauHelper;
    std::ifstream preloadFile("../src/gamescripts/sample_lua/preload.lua");
    std::stringstream preload;
    preload << preloadFile.rdbuf();
    luau->CompileAndRun("preload", preload.str().c_str(), preload.str().length() + 1);

    std::ifstream loopFile("../src/gamescripts/sample_lua/loop.lua");
    std::stringstream loop;
    loop << loopFile.rdbuf();

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            rlImGuiBegin();
            ClearBackground(WHITE);

            SetupGuiStyle();
            luau->CompileAndRun("loop", loop.str().c_str(), loop.str().length() + 1);

            rlImGuiEnd();
            DrawText("LUA TEST", 0, 0, 50, BLACK);
        }
        EndDrawing();
    }
    delete(luau);
    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
