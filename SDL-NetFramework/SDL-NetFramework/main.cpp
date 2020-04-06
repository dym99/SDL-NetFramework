//Dylan Moore - 100662175
//Sydney Caldwell - 100652057

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <string.h>


#include "Window.h"
#include "Debug.h"
#include "Events.h"
#include "Scene.h"
#include "Time.h"
#include "PlayerBehaviour.h"
#include "RemotePlayerBehaviour.h"
#include "ScoreKeeper.h"

#include "ClientInfo.h"

#include "Net.h"

#include <chrono>
#include <iostream>
#include <unordered_map>

static bool serve = false;
static std::string serverAddr, serverPort;

static unsigned short bindPort;
static int client = INVALID_SOCKET;
static struct sockaddr_in clientAddr = {};
static int clientnamelen = sizeof(sockaddr);

static struct addrinfo* addrinfoptr = NULL, hints;

//Interval between packets sent (in milliseconds)
static int sendInterval = 16;


int selfID = 0;

static std::unordered_map<int, ClientInfo> activePlayers;

int main(int argc, char *argv[]) {
	
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		DEBUG_LOG("SDL_Init failed!\n");
		return 1;
	}

	IMG_Init(IMG_INIT_PNG);
	TTF_Init();

	Window::open("Title", 1280, 720);

	if (!Window::isOpen()) {
		DEBUG_LOG("Failed to create window!\n");
		SDL_Quit();
		return 1;
	}
	
	//Load resources here
	Texture background("res/Backgrounds/Background_Space.png");
	Texture alien("res/Balls/Ball_Alien.png");
	Texture paddle("res/Paddles/Paddle_AH.png");
	Texture paddle2("res/Paddles/Paddle_AH_Blue.png");
	

	Sprite backgroundSprite(&background, { 0,0 }, { 1300,720 });
	backgroundSprite.setZOrder(1000.0f);

	Sprite playerSprite(&alien, { 32, 32 }, { 32, 32 });

	//Scenes here
	Scene testScene;
	testScene.addSprite(&backgroundSprite);
	testScene.addSprite(&playerSprite);


	Scene* currentScene = &testScene;

	//Initialise networking
	Net::init();

	Net::startTCPSocket();
	Net::startUDPSocket();

	Net::bindUDP(0);

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
		Net::cleanup();
		return 1;
	}
	else {
		Net::connectTCP(addrinfoptr->ai_addr, addrinfoptr->ai_addrlen);
		std::string selfIDStr = Net::recvTCP();
		
		if (sscanf_s(selfIDStr.c_str(), "%d", &selfID) != 1) {
			fprintf_s(stderr, "Failed to parse client ID!\n");
		}
		else {
			printf("Client ID: %d\n", selfID);
		}
	}

	PlayerBehaviour pBehaviour = PlayerBehaviour();
	
	playerSprite.addBehaviour(&pBehaviour);

	//Run application
	SDL_Event event;

	currentScene->init();

	std::chrono::time_point<std::chrono::steady_clock>
		lastClockChrono = std::chrono::steady_clock::now(),
		nowChrono = std::chrono::steady_clock::now();

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

		//Gotta clear those "posChanged"!
		for (auto i = activePlayers.begin(); i != activePlayers.end(); ++i) {
			i->second.posChanged = false;

			//Also check if the player has an avatar

			//If you are this player, you have one, so don't worry about it.
			if (i->first != selfID) {
				
				//If no avatar exists, make one.
				if (!i->second.hasAvatar) {
					i->second.hasAvatar = 1;
					i->second.remotePlayerSprite = new Sprite(&alien, { 32, 32 }, { 32, 32 });
					i->second.remotePlayer = new RemotePlayerBehaviour(&activePlayers, i->first);
					i->second.remotePlayerSprite->addBehaviour(i->second.remotePlayer);
					testScene.addSprite(i->second.remotePlayerSprite);
				}

			}
		}

		printf("My ID: %d\n", selfID);

		//Gotta receive those messages
		int addrlen = addrinfoptr->ai_addrlen;
		sockaddr addr = *addrinfoptr->ai_addr;
		while (1) {
			std::string data = Net::recvFromUDP(&addr, &addrlen);
			if (data == "") {
				printf("Would block.\n");
				break;
			}
			
			//Parse that dang data
			int numClients = 0;
			const char *dataStr = data.c_str();
			char* currentPlace = (char*)dataStr;
			if (sscanf_s(currentPlace, "%d", &numClients)==1) {
				for (int i = 0; i < numClients; ++i) {
					//Process a clientinfo
					ClientInfo client = {};

					//Initial value in case of failure.
					client.posChanged = false;
					client.pos = { 0,0 };

					//Next line.
					currentPlace = strchr(currentPlace, '\n') + 1;
					if (!currentPlace) {
						DEBUG_LOG("Unexpected end of string!\n");
						break;
					}

					//Read clientID
					int clientID = 0;
					if (sscanf_s(currentPlace, "%d", &clientID) != 1) {
						DEBUG_LOG("Failed to read clientID for index %d\n", i);
						break;
					}

					if ((activePlayers.find(clientID) != activePlayers.end())) {
						client = activePlayers.at(clientID);
						client.posChanged = false;
						client.pos = { 0,0 };
					}

					//Next line
					currentPlace = strchr(currentPlace, '\n') + 1;
					if (!currentPlace) {
						DEBUG_LOG("Unexpected end of string!\n");
						break;
					}

					//Read posChanged
					int clientPosChanged = 0;
					if (sscanf_s(currentPlace, "%d", &clientPosChanged) != 1) {
						DEBUG_LOG("Failed to read posChanged for index %d\n", i);
						break;
					}
					//If position changed
					if (clientPosChanged) {
						//Next line
						currentPlace = strchr(currentPlace, '\n') + 1;
						if (!currentPlace) {
							DEBUG_LOG("Unexpected end of string!\n");
							break;
						}
						//Read new position
						glm::vec2 pos = glm::vec2(0,0);
						if (sscanf_s(currentPlace, "%f %f", &pos.x, &pos.y) == 2) {
							client.pos = pos;
							client.posChanged = true;
						}
					}

					if (clientID != selfID) {
						activePlayers.insert_or_assign(clientID, client);
					}
				}
				
			}
			else {
				DEBUG_LOG("Bad data received!\n");
			}
			

		}


		//Update
		currentScene->update();

		//Render
		Window::clear(0, 0, 0, 1);

		currentScene->render();

		Window::present();

		//Update chrono
		lastClockChrono = nowChrono;
		nowChrono = std::chrono::steady_clock::now();

		//Freeze the thread for the duration of the send interval.
		while (std::chrono::duration_cast<std::chrono::milliseconds>(nowChrono - lastClockChrono) < std::chrono::milliseconds(sendInterval)) {
			nowChrono = std::chrono::steady_clock::now();
		}

		glm::vec2 lastPos = pBehaviour.getLastPos();
		glm::vec2 pos = pBehaviour.getPos();

		char buf[32] = {};
		memset(buf, 0, 32);
		sprintf_s(buf, "%f %f", pos.x, pos.y);
		if (lastPos != pos) {

			int addrlen = addrinfoptr->ai_addrlen;
			sockaddr addr = *addrinfoptr->ai_addr;
			Net::sendToUDP(&addr, addrlen, std::string(buf));
		}
		else {
			//Send a dummy packet because for some reason it won't receive unless a packet has been sent?
			//We don't know why this is neccesary and the program occasionally works without this line.
			//We've spent a couple days now trying to figure this out and it works.
			//And if it works, don't fix it.
			Net::sendToUDP(&addr, addrlen, std::string("0"));
		}
	}

	//Clean up

	//Delete dynamically allocated sprites.
	for (auto i = activePlayers.begin(); i != activePlayers.end(); ++i) {
		//If you are this player, your sprite wasn't dynamically allocated.
		if (i->first != selfID) {

			if (i->second.remotePlayerSprite) {
				delete i->second.remotePlayerSprite;
			}
			if (i->second.remotePlayer) {
				delete i->second.remotePlayer;
			}

		}
	}
	
	Net::closeTCPSocket();
	Net::closeUDPSocket();

	if (!serve) {
		freeaddrinfo(addrinfoptr);
	}

	Net::cleanup();
	SDL_Quit();
	return 0;
}