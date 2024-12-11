// TEST FOR LUA

#include "luacode.h"
#include "lualib.h"
#include "lua.h"

#include <raylib.h>

#include "../vendor/imgui/imgui.h"
#include "../rlImGui.h"
#include "misc/cpp/imgui_stdlib.h"
#include <iostream>

static int test(lua_State *L) {
    std::cout << "Called from lua!" << std::endl;
    return 1;
}

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

    // oh dear god
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    lua_pushcfunction(L, test, "testCFunction");
    lua_setglobal(L, "testCFunction");
    lua_pop(L, 1);

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            rlImGuiBegin();
            ClearBackground(WHITE);

            static size_t bytecodeSize = 0;
            static std::string luaScript;
            static int runResult = 0;
            ImGui::Text("Lua Script:");
            static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
            ImGui::InputTextMultiline("##", &luaScript, ImVec2(ImGui::GetWindowSize().x - 16, 256), flags, NULL, NULL);
            if (ImGui::Button("Compile Bytecode")) {
                char* bytecode = luau_compile(luaScript.c_str(), luaScript.size() + 1, NULL, &bytecodeSize);
                runResult = luau_load(L, "test", bytecode, bytecodeSize, 0);
                if (runResult == 0) {
                    lua_pcall(L, 0, 0, 0);
                } else {
                    std::cerr << lua_tostring(L, -1) << std::endl;
                    lua_pop(L,1);
                }
                free(bytecode);
            }
            ImGui::Text("Check your console for output.");
            rlImGuiEnd();
            DrawText("LUA TEST", 0, 0, 50, BLACK);
        }
        EndDrawing();
    }
    rlImGuiShutdown();

    CloseWindow();
    lua_close(L);
    return 0;
}
