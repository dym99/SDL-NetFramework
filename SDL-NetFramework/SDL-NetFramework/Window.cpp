//Dylan Moore - 100662175
//Sydney Caldwell - 100652057

#include "Window.h"

bool Window::m_open = false;
SDL_Window* Window::m_window = NULL;
SDL_Renderer* Window::m_renderer = NULL;


void Window::open(const char* _title, const int& _width, const int& _height)
{
	m_renderer = NULL;
	m_window = SDL_CreateWindow(_title,
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		1280, 720, SDL_WINDOW_SHOWN);
	if (m_window==NULL) {
		m_open = false;
	}
	else {
		m_renderer = SDL_CreateRenderer(m_window, 0, SDL_RENDERER_ACCELERATED);
		if (m_renderer == NULL) {
			SDL_DestroyWindow(m_window);
			m_open = false;
		}
		else {
			m_open = true;
		}
	}
}

void Window::close()
{
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	m_open = false;
}

bool Window::isOpen()
{
	return m_open;
}

void Window::setWindowSize(int _width, int _height)
{
	SDL_SetWindowSize(m_window, _width, _height);
}

void Window::getWindowSize(int& _width, int& _height)
{
	SDL_GetWindowSize(m_window, &_width, &_height);
}

void Window::clear(Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a)
{
	SDL_SetRenderDrawColor(m_renderer, _r, _g, _b, _a);
	SDL_RenderClear(m_renderer);
}

void Window::present()
{
	SDL_RenderPresent(m_renderer);
}

SDL_Renderer* Window::getRenderer()
{
	return m_renderer;
}
