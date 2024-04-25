#include "pch.h"
#include "InputManager.h"

const Uint8* InputManager::s_pKeyStates{ nullptr };
SDL_GameController* InputManager::s_SDLGameController{ nullptr };
InputManager::MouseInfo InputManager::s_MouseInfo{};
InputManager::ControllerInfo InputManager::s_ControllerInfo{};
InputManager::KeyInfo InputManager::s_KeyInfo{};
std::unordered_map<InputManager::Event, std::vector<InputManager::Callback>> InputManager::m_CallBacks{};

InputManager::InputManager()
{
}

void InputManager::TriggerCallBacks(Event e)
{
	auto it = m_CallBacks.find(e);
	if (it != m_CallBacks.end())
	{
		for (Callback callback : m_CallBacks[e])
			callback();
	}
}

InputManager& InputManager::Get()
{
	static InputManager instance;
	return instance;
}

void InputManager::Init(SDL_GameController* SDLGameController)
{
	s_pKeyStates = SDL_GetKeyboardState(nullptr);
	s_SDLGameController = SDLGameController;
}

void InputManager::ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
{
	switch (e.keysym.sym)
	{
	case SDLK_RETURN:
		s_KeyInfo.pressedEnter = true;
		TriggerCallBacks(Event::PressedEnter);
		break;
	case SDLK_ESCAPE:
		s_KeyInfo.pressedEscape = true;
		TriggerCallBacks(Event::PressedEscape);
		break;
	case SDLK_e:
		s_KeyInfo.pressedE = true;
		TriggerCallBacks(Event::PressedE);
		break;
	}
}

void InputManager::ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
{
}

void InputManager::ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
	Point2f prevMousePos(s_MouseInfo.pos);
	s_MouseInfo.pos.x = float(e.x);
	s_MouseInfo.pos.y = float(e.y);
	s_MouseInfo.draggingLMB = true;
	s_MouseInfo.dragDist.x = s_MouseInfo.pos.x - prevMousePos.x;
	s_MouseInfo.dragDist.y = s_MouseInfo.pos.y - prevMousePos.y;
}

void InputManager::ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
{
	switch ( e.button )
	{
	case SDL_BUTTON_LEFT:
		s_MouseInfo.pressedLMB = true;
		s_MouseInfo.pressingLMB = true;
		TriggerCallBacks(Event::ClickedLMB);
		break;
	}
}

void InputManager::ProcessMouseUpEvent(const SDL_MouseButtonEvent& e)
{
	switch (e.button)
	{
	case SDL_BUTTON_LEFT:
		s_MouseInfo.pressingLMB = false;
		break;
	}
}

void InputManager::ProcessMouseWheelEvent(int direction)
{
	s_MouseInfo.scrollingMMB = true;
	s_MouseInfo.scrollDir = direction;
	TriggerCallBacks(Event::ScrollingMMB);
}

void InputManager::Update()
{
	//Controller Input
	float deadZoneX{ 0.4f };
	Sint16 xAxisSint16{ SDL_GameControllerGetAxis(s_SDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) };
	float xAxis{ float(xAxisSint16) / std::numeric_limits<Sint16>::max() };
	if (xAxis < -deadZoneX) s_ControllerInfo.leftJoystickDir.x = -1.f;
	else if (xAxis > deadZoneX) s_ControllerInfo.leftJoystickDir.x = 1.f;
	else s_ControllerInfo.leftJoystickDir.x = 0.f;

	float deadZoneY{ 0.8f };
	Sint16 yAxisSint16{ SDL_GameControllerGetAxis(s_SDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) };
	float yAxis{ float(yAxisSint16) / std::numeric_limits<Sint16>::max() };
	if (yAxis < -deadZoneY) s_ControllerInfo.leftJoystickDir.y = 1.f;
	else if (yAxis > deadZoneY) s_ControllerInfo.leftJoystickDir.y = -1.f;
	else s_ControllerInfo.leftJoystickDir.y = 0.f;

	s_ControllerInfo.pressingButtonX = SDL_GameControllerGetButton(s_SDLGameController, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A);

	if (SDL_GameControllerGetAxis(s_SDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERRIGHT))
	{
		float deadZoneRightTrigger{ 0.5f };
		Sint16 axisSint16{ SDL_GameControllerGetAxis(s_SDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERRIGHT) };
		float axis{ float(axisSint16) / std::numeric_limits<Sint16>::max() };
		s_ControllerInfo.pressingRightTrigger = std::abs(axis) > deadZoneRightTrigger;
	}

	s_ControllerInfo.pressingRightShoulder = SDL_GameControllerGetButton(s_SDLGameController, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
}

void InputManager::Reset()
{
	s_MouseInfo.pressedLMB = false;
	s_MouseInfo.scrollingMMB = false;

	s_KeyInfo.pressedEnter = false;
	s_KeyInfo.pressedEscape = false;
	s_KeyInfo.pressedE = false;
}

void InputManager::RegisterCallback(Event e, Callback callback)
{
	m_CallBacks[e].push_back(callback);
}

InputManager::MouseInfo& InputManager::GetMouseInfo()
{
	return s_MouseInfo;
}

InputManager::ControllerInfo& InputManager::GetControllerInfo()
{
	return s_ControllerInfo;
}

InputManager::KeyInfo& InputManager::GetKeyInfo()
{
	return s_KeyInfo;
}
