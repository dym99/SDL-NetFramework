//Dylan Moore - 100662175
//Sydney Caldwell - 100652057

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "Window.h"
#include "Debug.h"
#include "Events.h"
#include "Scene.h"
#include "Time.h"
#include "PlayerBehaviour.h"
#include "ScoreKeeper.h"

#include "Net.h"

#include <iostream>

static bool serve = false;
static std::string serverAddr, serverPort;

static unsigned short bindPort;
static int client = INVALID_SOCKET;
static struct sockaddr_in clientAddr = {};
static int clientnamelen = sizeof(sockaddr);

static struct addrinfo* addrinfoptr = NULL, hints;

int main(int argc, char *argv[]) {
	
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		return 1;
	}

	IMG_Init(IMG_INIT_PNG);
	TTF_Init();

	Window::open("Title", 1280, 720);

	if (!Window::isOpen()) {
		DEBUG_LOG("Failed to create window!");
		SDL_Quit();
		return 1;
	}
	
	//Load resources here
	Texture background("res/Backgrounds/Background_Space.png");
	Texture puck("res/Balls/Ball_Alien.png");
	Texture paddle("res/Paddles/Paddle_AH.png");
	Texture paddle2("res/Paddles/Paddle_AH_Blue.png");
	

	Sprite backgroundSprite(&background, { 0,0 }, { 1300,720 });
	backgroundSprite.setZOrder(1000.0f);

	Sprite playerSprite(&puck, { 32, 32 }, { 32, 32 });

	//Scenes here
	Scene testScene;
	testScene.addSprite(&backgroundSprite);
	testScene.addSprite(&playerSprite);


	Scene* currentScene = &testScene;

	//Initialise networking
	Net::init();

	Net::startTCPSocket();
	Net::startUDPSocket();

	printf("Enter an ip address:\n > ");

	std::cin >> serverAddr;

	printf("Enter a port:\n > ");

	std::cin >> serverPort;

	//Get the server address and connect.
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	if (getaddrinfo(serverAddr.c_str(), serverPort.c_str(), &hints, &addrinfoptr) != 0) {
		DEBUG_LOG("Getaddrinfo failed!! Couldn't connect to server! WSAError: %d\n", WSAGetLastError());
	}
	else {
		Net::connectTCP(addrinfoptr->ai_addr, addrinfoptr->ai_addrlen);
	}

	PlayerBehaviour pBehaviour = PlayerBehaviour();
	
	playerSprite.addBehaviour(&pBehaviour);

	//Run application
	SDL_Event event;

	currentScene->init();

	while (Window::isOpen()) {
		//Calculate delta time
		clock_t now = clock();
		Time::deltaTime = ((float)(now - Time::lastClock)) / (float)CLOCKS_PER_SEC;
		Time::time += Time::deltaTime;
		Time::lastClock = now;

		EVENTS->newFrame();
		//Poll Events
		while (SDL_PollEvent(&event)) {
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

		//Slow down the game a little bit so it runs more consitently.
		SDL_Delay(10);
	}

	//Clean up
	
	Net::closeTCPSocket();
	Net::closeUDPSocket();

	if (!serve) {
		freeaddrinfo(addrinfoptr);
	}

	Net::cleanup();
	SDL_Quit();
	return 0;
}