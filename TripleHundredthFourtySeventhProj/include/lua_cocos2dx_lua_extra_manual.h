#ifndef __LUA_EXTRA_MANUAL_H_
#define __LUA_EXTRA_MANUAL_H_

extern "C" {
#include "lua.h"
#include "tolua++.h"
}
#include "lua_cocos2dx_tolua_fix.h"
// #if LUA_VERSION_NUM < 502
// #define luaL_newlib(L,l) (lua_newtable(L), luaL_register(L,NULL,l))
// #endif
#if (LUA_VERSION_NUM == 501)
#define luaL_setfuncs(L,f,n) luaL_register(L,NULL,f)
#define luaL_newlib(L,f) luaL_register(L,"lpeg",f)
#endif
#if LUA_VERSION_NUM >= 503
    #define lua_tointegerx lua_tointegerx
#else
    #define lua_tointegerx(L,idx,isnum) \
        (*(isnum) = lua_isnumber(L,idx), lua_tointeger(L,idx))
#endif

TOLUA_API int register_extra_manual_all(lua_State* tolua_S);

#endif // __LUA_EXTRA_MANUAL_H_
