#include <SDL2/SDL.h>

#include "Window.h"
#include "Debug.h"
#include "Events.h"

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


	//Run application
	SDL_Event event;
	while (Window::isOpen()) {
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


		//Render
		Window::clear(0, 0, 0, 1);

		//draw heres

		Window::present();
	}

	Window::close();
	SDL_Quit();
	return 0;
}