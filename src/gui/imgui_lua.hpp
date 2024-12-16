#pragma once
// As a stopgap, the engine will provide certain bindings for imgui to lua, HL1 style
// Of course, feel free to implement certain imagebuttons / more complex gui elements in raylib.

// Modify style.hpp to change style.

// Include the constructor WITHIN a raylib loop, and remember you are in charge for CLEANING UP after yourself.
// Make sure ImGui is initialized BEFORE luau!

#include "luacode.h"
#include "lualib.h"
#include "lua.h"
#include "imgui.h"

class ImGuiLuaBridge {
    public:
        ImGuiLuaBridge(lua_State *L);
    private:
//        static int Test(lua_State *L);
        static int Begin(lua_State *L);
        static int End(lua_State *L);
        static int Text(lua_State *L);
        static int Button(lua_State *L);
        static int InputTextMultiline(lua_State *L);

        constexpr static const luaL_Reg LuaFuncs[] =
        {
          {"Begin", Begin},
          {"End", End},
          {"Text", Text},
          {"Button", Button},
          {"InputTextMultiline", InputTextMultiline},
          {NULL, NULL}
        };
};
