//Dylan Moore - 100662175
//Sydney Caldwell - 100652057

#include "ScoreKeeper.h"

#include "Net.h"

ScoreKeeper* ScoreKeeper::m_instance = nullptr;

ScoreKeeper::ScoreKeeper(SDL_Renderer* _renderer, const glm::vec2& _position, bool _server, int _client) : Sprite::Sprite(NULL, _position, { 0,0 }) {
	m_renderer = _renderer;
	font = TTF_OpenFont("./res/Font/OpenSans-Regular.ttf", 32);
	local = 0;
	remote = 0;
	m_server = _server;
	m_client = _client;
	if (m_instance==nullptr)
		ScoreKeeper::m_instance = this;
}

ScoreKeeper::~ScoreKeeper()
{
	Sprite::~Sprite();
	TTF_CloseFont(font);
}

void ScoreKeeper::init()
{
	Sprite::init();
}

void ScoreKeeper::update()
{
	Sprite::update();
	
		std::string str = "";
		if (m_server) {
			str = Net::recvTCP(m_client);
		}
		else {
			str = Net::recvTCP();
		}
		if (str!="") printf("[TCP]Recv: %s\n", str.c_str());
	
		if (str == "SCORE") {
			increaseLocalScore();
		}
	
}

void ScoreKeeper::render()
{
	//Don't render a sprite
	//Do font instead
	char localScore[25] = {};
	char remoteScore[25] = {};

	_itoa_s(local, localScore, 10);
	_itoa_s(remote, remoteScore, 10);

	SDL_Surface* surfL = TTF_RenderText_Solid(font, localScore, { 225,0,0,225 });
	SDL_Surface* surfR = TTF_RenderText_Solid(font, remoteScore, { 0,0,225,225 });

	SDL_Texture* textureL = SDL_CreateTextureFromSurface(m_renderer, surfL);
	SDL_Texture* textureR = SDL_CreateTextureFromSurface(m_renderer, surfR);

	int width=0,height = 0;
	SDL_QueryTexture(textureL, NULL, NULL, &width, &height);
	SDL_Rect rect = { static_cast<int>(getPosition().x-100.f), static_cast<int>(getPosition().y), width, height };
	SDL_RenderCopy(m_renderer, textureL, NULL, &rect);

	SDL_QueryTexture(textureL, NULL, NULL, &width, &height);
	rect = {static_cast<int>(getPosition().x+100.f), static_cast<int>(getPosition().y), width, height };
	SDL_RenderCopy(m_renderer, textureR, NULL, &rect);

	SDL_DestroyTexture(textureL);
	SDL_DestroyTexture(textureR);

	SDL_FreeSurface(surfL);
	SDL_FreeSurface(surfR);
}

void ScoreKeeper::increaseLocalScore()
{
	++local;
}

void ScoreKeeper::increaseRemoteScore()
{
	++remote;
}
