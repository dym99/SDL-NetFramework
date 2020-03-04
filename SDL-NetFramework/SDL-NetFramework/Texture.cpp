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

void Texture::render(const SDL_Rect& bounds)
{
	if (Window::isOpen())
		SDL_RenderCopy(Window::getRenderer(), m_texture, NULL, &bounds);
}
