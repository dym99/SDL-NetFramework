#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
class Texture
{
public:
	Texture(const char* _filepath);
	~Texture();

	bool isOk() const;

	void render(const SDL_Rect& bounds, const float& _angle = 0.0f, const SDL_Point& _pivot = { 0, 0 }, const SDL_RendererFlip& _flip = SDL_FLIP_NONE);

private:
	SDL_Texture* m_texture;
	bool m_ok;
};

