#include <SDL2/SDL.h>

#include "Window.h"
#include "Debug.h"
#include "Events.h"
#include "Scene.h"
#include "Time.h"
#include "PaddleBehaviour.h"

#include "Net.h"

#include <iostream>

static bool serve = false;
static std::string serverAddr, serverPort;

static unsigned short bindPort;

static struct addrinfo* addrinfoptr = NULL, hints;

int main(int argc, char *argv[]) {
	
	printf("Start server? (y/n)\n");
	std::string input;
	std::cin >> input;

	if (input == "y" || input == "Y") {
		serve = true;

		printf("Enter port to bind to:\n");
		std::cin >> bindPort;
	}
	else {
		printf("Enter server address:\n");
		std::cin >> serverAddr;
		printf("Enter server port:\n");
		std::cin >> serverPort;
	}

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
	Texture background("res/Backgrounds/Background_Hockey.png");
	Texture ball("res/Balls/Puck.png");
	Texture paddle("res/Paddles/Paddle_AH.png");
	

	Sprite backgroundSprite(&background, { 0,0 }, { 1300,720 });
	backgroundSprite.setZOrder(1000.0f);

	Sprite ballSprite(&ball, { 32, 32 }, { 32, 32 });

	Sprite paddleSprite1(&paddle, { 40, 100 }, { 48, 48 });
	paddleSprite1.setZOrder(-1.0f);

	PaddleBehaviour pBehaviour = PaddleBehaviour(200.f);
	paddleSprite1.addBehaviour(&pBehaviour);

	//Scenes here
	Scene testScene;
	testScene.addSprite(&backgroundSprite);
	testScene.addSprite(&ballSprite);
	testScene.addSprite(&paddleSprite1);

	Scene* currentScene = &testScene;

	//Initialise networking
	Net::init();

	Net::startTCPSocket();
	Net::startUDPSocket();

	if (serve) {
		Net::bindTCP(bindPort);
		Net::listenTCP();

		Net::bindUDP(bindPort);
	}
	else {
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
	}

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
	Net::closeTCPSocket();
	Net::closeUDPSocket();

	if (!serve) {
		freeaddrinfo(addrinfoptr);
	}

	Net::cleanup();
	SDL_Quit();
	return 0;
}