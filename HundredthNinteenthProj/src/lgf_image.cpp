#include <iostream>
#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>
#include "lua_includes.h"

#include "lgf_image.hpp"
#include "lgf_renderer.hpp"

std::vector<SDL_Texture *> textures;

// Loads an image from a filepath
int ImageLoader::loadImage(lua_State *L) {
  const char *path = lua_tostring(L, 1);

  SDL_Texture *tex = Renderer::loadImg(path);
  textures.push_back(tex);

  return 0;
}

// Draws that image
int ImageLoader::drawImage(lua_State *L) {
  int x = lua_tonumber(L, 1);
  int y = lua_tonumber(L, 2);
  int w = lua_tonumber(L, 3);
  int h = lua_tonumber(L, 4);
  
  int srcX = lua_tonumber(L, 5);
  int srcY = lua_tonumber(L, 6);
  int srcWidth = lua_tonumber(L, 7);
  int srcHeight = lua_tonumber(L, 8);

  int textureIndex = lua_tonumber(L, 5);
  Renderer::drawImg(x, y, w, h, srcX, srcY, srcWidth, srcHeight, textures[textureIndex]);

  return 0;
}

void ImageLoader::registerLuaFunctions(lua_State *L) {
  lua_register(L, "loadImage", loadImage);
  lua_register(L, "drawImage", drawImage);
}
