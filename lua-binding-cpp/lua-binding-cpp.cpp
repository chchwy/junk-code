// lua-binding-cpp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int main()
{
    lua_State* state = luaL_newstate();

    lua_pushboolean(state, true);
    lua_pushboolean(state, false);
    lua_pushboolean(state, true);
    lua_pushinteger(state, 3);

    printf("%d\n", lua_isboolean(state, 1));
    printf("%d\n", lua_isboolean(state, 2));
    printf("%d\n", lua_isboolean(state, 3));
    printf("%d\n", lua_isboolean(state, 4));

    int ok = luaL_dofile(state, "hello.lua");
    if (ok != 0)
    {
        printf("Bad file!\n");
    }

    lua_getglobal(state, "width");
    double width = lua_tonumber(state, -1);
    printf("width is number: %d\n", lua_isnumber(state, -1));
    printf("Width = %lf\n", width);

    lua_close(state);

    return 0;
}

