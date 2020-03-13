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
#include "PaddleBehaviour.h"
#include "RemotePaddleBehaviour.h"
#include "PuckBehaviour.h"
#include "ScoreKeeper.h"
#include "FakeLagBehaviour.h"

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

	IMG_Init(IMG_INIT_PNG);
	TTF_Init();

	Window::open("Title", 1280, 720);

	if (!Window::isOpen()) {
		DEBUG_LOG("Failed to create window!");
		SDL_Quit();
		return 1;
	}
	
	//Load resources here
	Texture background("res/Backgrounds/Background_Hockey.png");
	Texture puck("res/Balls/Puck.png");
	Texture paddle("res/Paddles/Paddle_AH.png");
	Texture paddle2("res/Paddles/Paddle_AH_Blue.png");
	

	Sprite backgroundSprite(&background, { 0,0 }, { 1300,720 });
	backgroundSprite.setZOrder(1000.0f);

	Sprite puckSprite(&puck, { 32, 32 }, { 32, 32 });

	Sprite paddleSprite1(&paddle, { 40, 100 }, { 48, 48 });
	paddleSprite1.setZOrder(-2.0f);

	Sprite paddleSprite2(&paddle2, { 1240, 100 }, { 48, 48 });
	paddleSprite1.setZOrder(-1.0f);


	//Scenes here
	Scene testScene;
	testScene.addSprite(&backgroundSprite);
	testScene.addSprite(&puckSprite);
	testScene.addSprite(&paddleSprite1);
	testScene.addSprite(&paddleSprite2);


	Scene* currentScene = &testScene;

	//Initialise networking
	Net::init();

	Net::startTCPSocket();
	Net::startUDPSocket();

	if (serve) {
		Net::bindTCP(bindPort);
		Net::listenTCP();

		Net::bindUDP(bindPort);

		printf("Awaiting connection... ");
		
		client = Net::acceptTCP(NULL, NULL);

		if (client == INVALID_SOCKET) {
			printf("Accept failed! WSAError: %ld\n", WSAGetLastError());

			//Clean up
			Net::closeTCPSocket();
			Net::closeUDPSocket();
			
			if (!serve) {
				freeaddrinfo(addrinfoptr);
			}

			Net::cleanup();
			SDL_Quit();
			return 1;
		}
		else {
			printf("Found!\n");
			if (getpeername(client, (sockaddr*)&clientAddr, &clientnamelen) == SOCKET_ERROR) {
				DEBUG_LOG("getpeername failed! WSAError: %ld\n", WSAGetLastError());
				system("pause");
			}

			//Non-blocking mode after connection
			unsigned long iMode = 1;
			int iResult = ioctlsocket(client, FIONBIO, &iMode);
			if (iResult != NO_ERROR)
				printf("ioctlsocket failed with error: %ld\n", iResult);
		}
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
	if (serve) {
		PaddleBehaviour pBehaviour = PaddleBehaviour(serve, (sockaddr*)&clientAddr, clientnamelen, &puckSprite);
		RemotePaddleBehaviour pBehaviourRemote = RemotePaddleBehaviour(serve, (sockaddr*)&clientAddr, clientnamelen, &puckSprite);

		paddleSprite1.addBehaviour(&pBehaviour);
		paddleSprite2.addBehaviour(&pBehaviourRemote);
	}
	else {
		PaddleBehaviour pBehaviour = PaddleBehaviour(serve, addrinfoptr->ai_addr, addrinfoptr->ai_addrlen, &puckSprite);
		RemotePaddleBehaviour pBehaviourRemote = RemotePaddleBehaviour(serve, addrinfoptr->ai_addr, addrinfoptr->ai_addrlen, &puckSprite);

		paddleSprite1.addBehaviour(&pBehaviour);
		paddleSprite2.addBehaviour(&pBehaviourRemote);
	}

	PuckBehaviour puckBehaviour = PuckBehaviour(serve, client);
	puckSprite.addBehaviour(&puckBehaviour);

	FakeLagBehaviour flb = FakeLagBehaviour();
	puckSprite.addBehaviour(&flb);

	//Add the score keeper with networking info
	ScoreKeeper sKeeper(Window::getRenderer(), { 640, 25 }, serve, client);
	testScene.addSprite(&sKeeper);

	//Run application
	SDL_Event event;

	currentScene->init();

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