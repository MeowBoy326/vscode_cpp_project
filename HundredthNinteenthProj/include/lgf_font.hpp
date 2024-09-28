#pragma once

#include "lua_includes.h"
#include <SDL.h>
#include <SDL_ttf.h>

struct Text
{
  SDL_Texture *texture;
  SDL_Color color;
  std::string text;
  int x;
  int y;
};

namespace FontLoader
{
  
int loadFont(lua_State *L);
int loadText(lua_State *L);
int renderText(lua_State *L);

int changeText(lua_State *L);
int changePosition(lua_State *L);

void registerLuaFunctions(lua_State *L);

};
