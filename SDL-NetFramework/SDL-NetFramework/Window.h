//Dylan Moore - 100662175
//Sydney Caldwell - 100652057

#pragma once

#include <SDL2/SDL.h>

#include "Texture.h"

class Window
{
public:
	static void open(const char* _title, const int& _width, const int& _height);
	static void close();

	static bool isOpen();

	static void setWindowSize(int _width, int _height);
	static void getWindowSize(int& _width, int& _height);

	static void clear(Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a);
	static void present();

	static SDL_Renderer* getRenderer();
private:

	static bool m_open;

	static SDL_Window* m_window;
	static SDL_Renderer* m_renderer;
};