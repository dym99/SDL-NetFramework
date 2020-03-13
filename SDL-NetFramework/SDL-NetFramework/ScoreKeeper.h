//Dylan Moore - 100662175
//Sydney Caldwell - 100652057

#pragma once
#include "Sprite.h"
#include <SDL2/SDL_ttf.h>

class ScoreKeeper : public Sprite
{
public:
	ScoreKeeper(SDL_Renderer* _renderer, const glm::vec2& _position, bool _server, int _client);
	~ScoreKeeper();
	void init() override;
	void update() override;
	void render() override;

	void increaseLocalScore();
	void increaseRemoteScore();

	static ScoreKeeper* m_instance;
private:
	TTF_Font* font;
	int local;
	int remote;
	SDL_Renderer* m_renderer;
	bool m_server;
	int m_client;
};

