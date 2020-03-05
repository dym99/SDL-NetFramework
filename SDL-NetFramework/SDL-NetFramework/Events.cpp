#include "Events.h"

Events* Events::m_instance = nullptr;


Events::Events()
{
	////SDL_JoystickOpen(0);
	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
		if (SDL_IsGameController(i)) {
			char* mapping;
			SDL_Log("Index \'%i\' is a compatible controller, named \'%s\'", i, SDL_GameControllerNameForIndex(i));
			m_controller = SDL_GameControllerOpen(i);
			if (m_controller != NULL) {
				mapping = SDL_GameControllerMapping(m_controller);
				if (mapping != NULL) {
					SDL_Log("Controller %i is mapped as \"%s\".", i, mapping);
					SDL_free(mapping);
					break;
				}
				SDL_GameControllerClose(m_controller);
			}
		}
	}

	m_keysHeld = std::vector<int>();
	m_keysDown = std::vector<int>();
	m_keysUp = std::vector<int>();

	m_mouseHeld = std::vector<int>();
	m_mouseDown = std::vector<int>();
	m_mouseUp = std::vector<int>();

	m_buttonHeld = std::vector<int>();
	m_buttonDown = std::vector<int>();
	m_buttonUp = std::vector<int>();

	m_joyDeadZone = 0.08f;
}

Events::~Events()
{
	SDL_GameControllerClose(m_controller);
}


void Events::newFrame()
{
	m_keysDown.clear();
	m_keysUp.clear();

	m_mouseDown.clear();
	m_mouseUp.clear();

	m_buttonDown.clear();
	m_buttonUp.clear();

	m_mouseDelta[0] = 0;
	m_mouseDelta[1] = 0;

	m_mouseWDown = false;
	m_mouseWUp = false;
}

void Events::processEvent(SDL_Event _event)
{
	switch (_event.type) {
	case SDL_KEYDOWN:
		if (_event.key.repeat == 0) {
			if (!vectorContains(m_keysHeld, _event.key.keysym.sym)) {
				m_keysHeld.push_back(_event.key.keysym.sym);
			}
			if (!vectorContains(m_keysDown, _event.key.keysym.sym)) {
				m_keysDown.push_back(_event.key.keysym.sym);
			}
		}
		break;
	case SDL_KEYUP:
		if (_event.key.repeat == 0) {
			for (int i = 0; i < m_keysHeld.size(); ++i) {
				if (m_keysHeld[i] == _event.key.keysym.sym) {
					m_keysHeld.erase(m_keysHeld.begin() + i);
				}
			}
			if (!vectorContains(m_keysUp, _event.key.keysym.sym)) {
				m_keysUp.push_back(_event.key.keysym.sym);
			}
		}
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (!vectorContains(m_mouseHeld, _event.button.button)) {
			m_mouseHeld.push_back(_event.button.button);
		}
		if (!vectorContains(m_mouseDown, _event.button.button)) {
			m_mouseDown.push_back(_event.button.button);
		}
		break;
	case SDL_MOUSEBUTTONUP:
		for (int i = 0; i < m_mouseHeld.size(); ++i) {
			if (m_mouseHeld[i] == _event.button.button) {
				m_mouseHeld.erase(m_mouseHeld.begin() + i);
			}
		}
		if (!vectorContains(m_mouseUp, _event.button.button)) {
			m_mouseUp.push_back(_event.button.button);
		}
		break;
	case SDL_CONTROLLERBUTTONDOWN:
		printf("JoyButton: %u\n", _event.jbutton.button);
		if (!vectorContains(m_buttonHeld, _event.jbutton.button)) {
			m_buttonHeld.push_back(_event.jbutton.button);
		}
		if (!vectorContains(m_buttonDown, _event.jbutton.button)) {
			m_buttonDown.push_back(_event.jbutton.button);
		}
		break;
	case SDL_CONTROLLERBUTTONUP:
		for (int i = 0; i < m_buttonHeld.size(); ++i) {
			if (m_buttonHeld[i] == _event.jbutton.button) {
				m_buttonHeld.erase(m_buttonHeld.begin() + i);
			}
		}
		if (!vectorContains(m_buttonUp, _event.jbutton.button)) {
			m_buttonUp.push_back(_event.jbutton.button);
		}
		break;

	case SDL_CONTROLLERAXISMOTION:
		m_controllerAxes[_event.jaxis.axis] = ((float)_event.jaxis.value) / (((float)_event.jaxis.value) > 0 ? 32767.f : 32768.f);
		break;
	case SDL_MOUSEMOTION:
		m_mousePosition[0] = _event.motion.x;
		m_mousePosition[1] = _event.motion.y;
		m_mouseDelta[1] = _event.motion.yrel;
		m_mouseDelta[0] = _event.motion.xrel;
		break;
	case SDL_MOUSEWHEEL:
		m_mouseWUp = _event.wheel.y > 0;
		m_mouseWDown = _event.wheel.y < 0;
		break;
	}
}

void Events::setJoyDeadZone(float _deadzone)
{
	m_joyDeadZone = _deadzone;
}

float Events::getJoyDeadZone()
{
	return m_joyDeadZone;
}

bool Events::isKeyHeld(int _key)
{
	return vectorContains(m_keysHeld, _key);
}

bool Events::isKeyDown(int _key)
{
	return vectorContains(m_keysDown, _key);
}

bool Events::isKeyUp(int _key)
{
	return vectorContains(m_keysUp, _key);
}

bool Events::isMouseHeld(int _button)
{
	return vectorContains(m_mouseHeld, _button);
}

bool Events::isMouseDown(int _button)
{
	return vectorContains(m_mouseDown, _button);
}

bool Events::isMouseUp(int _button)
{
	return vectorContains(m_mouseUp, _button);
}

bool Events::isButtonHeld(Gamepad_Button _button)
{
	return vectorContains(m_buttonHeld, (int)_button);
}

bool Events::isButtonDown(Gamepad_Button _button)
{
	return vectorContains(m_buttonDown, (int)_button);
}

bool Events::isButtonUp(Gamepad_Button _button)
{
	return vectorContains(m_buttonUp, (int)_button);
}

float Events::getAxis(Gamepad_Axis _axis)
{
	return abs(m_controllerAxes[(int)_axis]) > m_joyDeadZone ? m_controllerAxes[(int)_axis] : 0.0f;
}

void Events::getMousePosition(int* x, int* y) const
{
	*x = m_mousePosition[0];
	*y = m_mousePosition[1];
}

void Events::getMouseDelta(int* x, int* y) const
{
	*x = m_mouseDelta[0];
	*y = m_mouseDelta[1];
}

bool Events::getMouseWheelUp()
{
	return m_mouseWUp;
}

bool Events::getMouseWheelDown()
{
	return m_mouseWDown;
}

Events* Events::getInstance()
{
	if (m_instance == nullptr) {
		m_instance = new Events();
	}
	return m_instance;
}

void Events::destroyInstance()
{
	delete m_instance;
}

bool Events::vectorContains(std::vector<int> _vec, int _key)
{
	bool temp = false;
	for (int i = 0; i < _vec.size(); ++i) {
		if (_vec[i] == _key) {
			temp = true;
			break;
		}
	}
	return temp;
}
