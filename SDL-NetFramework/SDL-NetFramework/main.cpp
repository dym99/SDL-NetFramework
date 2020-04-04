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

struct ClientInfo {
	bool posChanged = false;
	glm::vec2 pos = {};
};

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
		}

		//Gotta receive those messages
		int recvlen = 0;
		int addrlen = addrinfoptr->ai_addrlen;
		while (1) {
			void* data = Net::recvFromUDPBin(addrinfoptr->ai_addr, &addrlen, recvlen);
			if (recvlen == -1) {
				int err = WSAGetLastError();
				if (err == WSAEWOULDBLOCK) {
					break;
				}
				else {
					DEBUG_LOG("Failed to recv! WSAError: %ld\n", err);
					break;
				}
			}
			
			//Parse that dang data

			//Ensure it is big enough to read an int.
			if (recvlen >= 4) {
				//The first four bytes are just the number of clients.
				int numClients = *((int*)data);
				//
				// Each client data packet is:
				//
				//	| Client id (int)        |
				//	|------------------------|
				//	| Is pos changed? (bool) |
				//	|------------------------|
				//	| Position (2xfloat)     |
				//
				//	Grand total of 4+1+(4*2) = 13 bytes per player
				//

				//ensure the packet is complete (enough room for each client and numClients (int)
				if (recvlen < 13 * numClients + 4) {
					//All good
					//Loop through and extract those juicy position updates
					//Start an iterator at the beginning (past numclients though)
					int bufIterator = 0 + sizeof(int);
					for (int i = 0; i < numClients; ++i) {
						int cli_id = *((int*)((char*)data)[bufIterator]);
						bufIterator += sizeof(int);
						bool cli_isPosChanged = *((bool*)((char*)data)[bufIterator]);
						bufIterator += sizeof(bool);
						glm::vec2 cli_pos = glm::vec2(0, 0);
						if (cli_isPosChanged) {
							cli_pos = *((glm::vec2*)((char*)data)[bufIterator]);
							bufIterator += sizeof(glm::vec2);
						}

						//Take those values and pass em to somewhere where they actually do something.
						ClientInfo client;
						client.posChanged = cli_isPosChanged;
						client.pos = cli_pos;
						activePlayers.insert_or_assign(cli_id, client);
					}
				}
				else {
					//Packet was bad
					DEBUG_LOG("Bad packet: Packet too small to fit %d player infos\n", numClients);
				}
			}
			else {
				//Packet was bad
				DEBUG_LOG("Bad packet: Packet too small: only %d bytes.\n", recvlen);
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

		while (std::chrono::duration_cast<std::chrono::milliseconds>(nowChrono - lastClockChrono) < std::chrono::milliseconds(/*16*/250)) {
			nowChrono = std::chrono::steady_clock::now();
		}

		glm::vec2 lastPos = pBehaviour.getLastPos();
		glm::vec2 pos = pBehaviour.getPos();
		if (lastPos != pos) {
			Net::sendToUDPBin(addrinfoptr->ai_addr, addrinfoptr->ai_addrlen, &pos, sizeof(glm::vec2));
		}
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