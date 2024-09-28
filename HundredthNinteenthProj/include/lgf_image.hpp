#pragma once

#include <vector>
#include <SDL.h>
#include "lua_includes.h"

namespace ImageLoader {

int loadImage(lua_State *L);
int drawImage(lua_State *L);

void registerLuaFunctions(lua_State *L);

};
