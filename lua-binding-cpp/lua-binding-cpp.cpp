// lua-binding-cpp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

void play_with_lua()
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

    printf("Get Top: %d\n", lua_gettop(state));

    int ok = luaL_dofile(state, "hello.lua");
    if (ok != 0)
    {
        printf("Bad file! %s\n", lua_tostring(state, -1));
    }

    lua_getglobal(state, "width");
    double width = lua_tonumber(state, -1);
    printf("width is number: %d\n", lua_isnumber(state, -1));
    printf("Width = %lf\n", width);

    lua_close(state);
}

class Kitten
{
public:
    std::string m_name;

    Kitten() { std::cout << "Kitten born!"; }
    ~Kitten() { std::cout << "Kitten died."; }

    void setName(std::string name)
    {
        std::cout << "setName: " << name << std::endl;
        m_name = name;
    }

    std::string getName() const
    {
        std::cout << "getName: " << m_name << std::endl;
        return m_name;
    }
};

#define KITTEN_META "kitten_meta"

int kitten_create(lua_State* L)
{
    //const char* name = luaL_checkstring(L, 1);
    Kitten** udata = (Kitten**)lua_newuserdata(L, sizeof(Kitten*));
    *udata = new Kitten;

    luaL_setmetatable(L, KITTEN_META);
    return 1;
}

int kitten_destroy(lua_State* L)
{
    auto me = (Kitten**)lua_touserdata(L, 1);
    delete *me;
    return 0;
}

int kitten_set_name(lua_State* L)
{
    auto k = (Kitten**)luaL_checkudata(L, 1, KITTEN_META);
    std::string name = luaL_checkstring(L, 2);
    (*k)->setName(name);
    return 0;
}

int kitten_get_name(lua_State* L)
{
    auto k = (Kitten**)luaL_checkudata(L, 1, KITTEN_META);
    std::string name = (*k)->getName();
    lua_pushstring(L, name.c_str());
    return 1;
}

void register_kitten(lua_State* L)
{
    lua_register(L, KITTEN_META, kitten_create);
    luaL_newmetatable(L, KITTEN_META);
    lua_pushcfunction(L, kitten_destroy);
    lua_setfield(L, -2, "__gc");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, kitten_set_name);
    lua_setfield(L, -2, "set");
    lua_pushcfunction(L, kitten_get_name);
    lua_setfield(L, -2, "get");
    lua_pop(L, 1);
}

void play_with_luna()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    register_kitten(L);

    int err = luaL_dofile(L, "hello.lua");
    if (err)
        std::cout << "Lua Error: " << luaL_checkstring(L, -1) << std::endl;

    lua_close(L);
}

int main()
{
    //play_with_lua();
    play_with_luna();
    return 0;
}

