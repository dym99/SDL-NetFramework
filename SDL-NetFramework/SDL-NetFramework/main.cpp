#include <SDL2/SDL.h>

#include "Window.h"
#include "Debug.h"
#include "Events.h"
#include "Scene.h"
#include "Time.h"
#include "PaddleBehaviour.h"

#include "Net.h"

int main(int argc, char *argv[]) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		return 1;
	}
	Window::open("Title", 1280, 720);

	if (!Window::isOpen()) {
		DEBUG_LOG("Failed to create window!");
		SDL_Quit();
		return 1;
	}
	
	//Load resources here
	Texture background("res/Backgrounds/Background_Space.png");
	Texture ball("res/Balls/Ball.png");
	Texture paddle("res/Paddles/Paddle.png");
	

	Sprite backgroundSprite(&background, { 0,0 }, { 1300,720 });
	backgroundSprite.setZOrder(1000.0f);
	Sprite ballSprite(&ball, { 32, 32 }, { 32, 32 });
	Sprite ballSprite2(&ball, { 48, 48 }, { 32, 32 });

	Sprite paddleSprite1(&paddle, { 40, 100 }, { 20,150 });
	paddleSprite1.setZOrder(-1.0f);

	PaddleBehaviour pBehaviour = PaddleBehaviour(200.f);
	paddleSprite1.addBehaviour(&pBehaviour);

	//Scenes here
	Scene testScene;
	testScene.addSprite(&backgroundSprite);
	testScene.addSprite(&ballSprite);
	testScene.addSprite(&ballSprite2);
	testScene.addSprite(&paddleSprite1);

	Scene* currentScene = &testScene;

	//Initialise networking
	Net::init();

	//Run application
	SDL_Event event;
	while (Window::isOpen()) {
		//Calculate delta time
		clock_t now = clock();
		Time::deltaTime = ((float)(now - Time::lastClock)) / (float)CLOCKS_PER_SEC;
		Time::time += Time::deltaTime;
		Time::lastClock = now;

		//Poll Events
		while (SDL_PollEvent(&event)) {
			EVENTS->newFrame();
			EVENTS->processEvent(event);
			switch (event.type) {
			case SDL_QUIT:
			case SDL_APP_TERMINATING:
				Window::close();
				break;
			}
		}

		//Update
		currentScene->update();

		//Render
		Window::clear(0, 0, 0, 1);

		currentScene->render();

		Window::present();
	}

	//Clean up
	Net::cleanup();
	SDL_Quit();
	return 0;
}