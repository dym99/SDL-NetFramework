#pragma once
#include <SDL2/SDL.h>
#include <vector>

enum class Gamepad_Button {
	A = 0,
	B = 1,
	X = 2,
	Y = 3,
	VIEW = 4,
	XBOX = 5,
	MENU = 6,
	LS = 7,
	RS = 8,
	LB = 9,
	RB = 10,
	DPAD_UP = 11,
	DPAD_DOWN = 12,
	DPAD_LEFT = 13,
	DPAD_RIGHT = 14,

	NUM_BUTTONS = 15
};

enum class Gamepad_Axis {
	LS_X = 0,
	LS_Y = 1,
	RS_X = 2,
	RS_Y = 3,
	LT = 4,
	RT = 5,

	NUM_AXIS = 6
};

#define EVENTS Events::getInstance()
class Events
{
public:
	Events();
	~Events();

	bool isKeyHeld(int _key);
	bool isKeyDown(int _key);
	bool isKeyUp(int _key);

	bool isMouseHeld(int _button);
	bool isMouseDown(int _button);
	bool isMouseUp(int _button);

	bool isButtonHeld(Gamepad_Button _button);
	bool isButtonDown(Gamepad_Button _button);
	bool isButtonUp(Gamepad_Button _button);

	float getAxis(Gamepad_Axis _axis);
	void getMousePosition(int* x, int* y) const;
	void getMouseDelta(int* x, int* y) const;

	bool getMouseWheelUp();
	bool getMouseWheelDown();

	void newFrame();
	void processEvent(SDL_Event _event);

	void setJoyDeadZone(float _deadzone);
	float getJoyDeadZone();

	static Events* getInstance();
	static void destroyInstance();
private:
	static Events* m_instance;

	std::vector<int> m_keysHeld;
	std::vector<int> m_keysDown;
	std::vector<int> m_keysUp;

	std::vector<int> m_mouseHeld;
	std::vector<int> m_mouseDown;
	std::vector<int> m_mouseUp;

	std::vector<int> m_buttonHeld;
	std::vector<int> m_buttonDown;
	std::vector<int> m_buttonUp;

	SDL_GameController* m_controller;

	int m_mousePosition[2];
	int m_mouseDelta[2];
	bool m_mouseWDown, m_mouseWUp;

	float m_joyDeadZone;
	float m_controllerAxes[(int)Gamepad_Axis::NUM_AXIS];

	bool vectorContains(std::vector<int> _vec, int _key);
};

