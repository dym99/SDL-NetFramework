//Dylan Moore - 100662175
//Sydney Caldwell - 100652057

#include "Texture.h"
#include "Window.h"
Texture::Texture(const char* _filepath)
{
	m_ok = true;
	m_texture = IMG_LoadTexture(Window::getRenderer(), _filepath);
	if (m_texture == NULL) {
		m_ok = false;
	}
}

Texture::~Texture()
{
	SDL_DestroyTexture(m_texture);
}

bool Texture::isOk() const
{
	return m_ok;
}

void Texture::render(const SDL_Rect& _bounds, const float& _angle, const SDL_Point& _pivot, const SDL_RendererFlip& _flip)
{
	if (!Window::isOpen()) return;

	SDL_RenderCopyEx(Window::getRenderer(), m_texture, NULL, &_bounds, _angle, &_pivot, _flip);
}
