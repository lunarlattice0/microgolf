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

namespace ImGuiLuaBridge {
    int Begin(lua_State *L);
    int Text(lua_State *L);
    int End(lua_State *L);
}
