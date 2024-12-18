#include "luahelper.hpp"
#include "luacode.h"
#include "lualib.h"
#include "lua.h"
#include <cstdlib>
#include <raylib.h>
#include <iostream>

// Initialize a LuaU VM.
LuauHelper::LuauHelper() {
    this->L = luaL_newstate();
    luaL_openlibs(this->L);
    this->lb = new ImGuiLuaBridge(L);
    lua_pushcfunction(L, CompileAndRun, "CRS");
    lua_setglobal(L, "CompileAndRun");
}

// Compile a lua file into bytecode and run it.
// Returns true upon success, and false upon failure.
bool LuauHelper::CompileAndRun(const char * name, const char * source, int sourceLen) {
    size_t bytecodeSize = 0;
    char * bytecode = luau_compile(source, sourceLen, NULL, &bytecodeSize);
    int runResult = 0;
    runResult = luau_load(L, name, bytecode, bytecodeSize, 0);
    free(bytecode);
    if (runResult == 0) {
        lua_pcall(L, 0, 0, 0);
        return true;
    } else {
        TraceLog(LOG_INFO, lua_tostring(L, -1));
        lua_pop(L,1);
        return false;
    }
}

// TODO: unsure of how safe this is...
int LuauHelper::CompileAndRun(lua_State *L) {
    LuauHelper * tempHelper = new LuauHelper;
    auto code = std::string(luaL_checkstring(L, -1));
    tempHelper->CompileAndRun("user", code.c_str(), code.length() + 1);
    delete(tempHelper);
    return 0;
}

LuauHelper::~LuauHelper() {
    delete(lb);
    lua_close(this->L);
}
