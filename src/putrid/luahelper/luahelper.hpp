// Interface with luau helper functions

#pragma once
#include "lua.h"
#include "lualib.h"
#include <iostream>
#include <raylib.h>
#include <sstream>
//#include "imgui_lua.hpp"

// Override the print() in lua so that it is nice with raylib
static int print(lua_State *L) {
    int nargs = lua_gettop(L);
    std::stringstream luaPrintStr;
    for (int i = 1; i <= nargs; ++i) {
        luaPrintStr << luaL_tolstring(L, i, nullptr);
        lua_pop(L, 1);
        if (i <= nargs - 1) {
            luaPrintStr << "\t";
        }
    }

    std::string concat = "Lua: " + luaPrintStr.str();
    TraceLog(LOG_INFO, concat.c_str());
    return 0;
}

class LuauHelper {
    public:
        LuauHelper();
        bool CompileAndRun(const char * name, const char * source, int sourceLen);
        //static int CompileAndRun(lua_State *L);
        ~LuauHelper();
    private:
        lua_State *L;
//        ImGuiLuaBridge * lb;
};
