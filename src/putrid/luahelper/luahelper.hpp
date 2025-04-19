// Interface with luau helper functions

#pragma once
#include "lua.h"
#include "lualib.h"
#include <iostream>
#include <memory>
#include <raylib.h>
#include <sstream>
#include <unordered_map>
#include <vector>
//#include "imgui_lua.hpp"

// Override the print() in lua so that it is nice with raylib
// Totally not a ripoff of RunService...

class RenderSvc {
    private:
        std::unordered_map<const char *, int> activeCallbacks;
        //int lua_reg_idx;
    public:
        //int GetLuaRegIdx();
        //RenderSvc(lua_State *L);
        void AddTask(int ref, const char * fname);
        void RemoveTask(const char * fname);
        int GetTask(const char * fname);
        void CallRenderCallbacks(lua_State *L);
};


class LuauHelper {
    public:
        LuauHelper();
        bool CompileAndRun(const char * name, const char * source, int sourceLen);
        //static int CompileAndRun(lua_State *L);
        ~LuauHelper();
        void CallServiceCallbacks(); // Aggregate func for handling all service calls.
    private:
        lua_State *L;
        // Stores a pointer to the RenderSvc held by lua.
        // Don't delete this, lua handles the dtor!!!
        RenderSvc * renderSvc;
};
