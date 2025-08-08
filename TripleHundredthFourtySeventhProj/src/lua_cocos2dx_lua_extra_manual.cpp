#include "lua_cocos2dx_CCLuaEngine.h"

#include "lua_cocos2dx_lua_extra_manual.h"
#include "lua_cocos2dx_lua_crypto_manual.h"

static int tolua_Cocos2d_Function_loadChunksFromZIP(lua_State* tolua_S)
{
    return cocos2d::LuaEngine::getInstance()->getLuaStack()->luaLoadChunksFromZIP(tolua_S);
}

static void cc_extend_functions(lua_State* tolua_S)
{
    tolua_module(tolua_S,"cc",0);
    tolua_beginmodule(tolua_S,"cc");
    tolua_function(tolua_S, "LuaLoadChunksFromZIP", tolua_Cocos2d_Function_loadChunksFromZIP);
    tolua_endmodule(tolua_S);
}

TOLUA_API int register_extra_manual_all(lua_State* tolua_S)
{
    register_crypto_module(tolua_S);
    cc_extend_functions(tolua_S);
    return 1;
}
