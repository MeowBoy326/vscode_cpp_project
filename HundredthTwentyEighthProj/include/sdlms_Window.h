#pragma once

#include <SDL_video.h>
#include <SDL_render.h>


class Window {
private:
	static  SDL_Window* window;
	static  SDL_Renderer* renderer;

	static  int mouse_x;
	static  int mouse_y;

	static  const char* window_title;

public:
	static void create_window(const char* title, unsigned int width, unsigned int height, unsigned int pixel_scale_h = 1, unsigned int pixel_scale_v = 1);
	static void destroy_window();

	static SDL_Window* get_main_window();
	static SDL_Renderer* get_renderer();

	static const char* get_title();

	static SDL_Point get_mouse_position();

	static void clear();
	static void update();
};
