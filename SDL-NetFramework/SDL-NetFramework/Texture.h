#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
class Texture
{
public:
	Texture(const char* _filepath);
	~Texture();

	bool isOk() const;

	void render(const SDL_Rect& bounds);

private:
	SDL_Texture* m_texture;
	bool m_ok;
};

