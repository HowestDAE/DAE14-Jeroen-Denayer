#include "pch.h"
#include "InputManager.h"

InputManager::InputManager()
	: m_pKeyStates{ nullptr }
	, m_pSDLGameController{ nullptr }
	, m_MouseInfo{ MouseInfo{} }
	, m_ControllerInfo{ ControllerInfo{} }
	, m_CallBacks{ std::unordered_map<Event, std::vector<Callback>>{} }
{
}

void InputManager::IUpdate()
{
	//Controller Input
	float deadZoneX{ 0.4f };
	Sint16 xAxisSint16{ SDL_GameControllerGetAxis(m_pSDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) };
	float xAxis{ float(xAxisSint16) / std::numeric_limits<Sint16>::max() };
	if (xAxis < -deadZoneX) m_ControllerInfo.leftJoystickDir.x = -1.f;
	else if (xAxis > deadZoneX) m_ControllerInfo.leftJoystickDir.x = 1.f;
	else m_ControllerInfo.leftJoystickDir.x = 0.f;

	float deadZoneY{ 0.8f };
	Sint16 yAxisSint16{ SDL_GameControllerGetAxis(m_pSDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) };
	float yAxis{ float(yAxisSint16) / std::numeric_limits<Sint16>::max() };
	if (yAxis < -deadZoneY) m_ControllerInfo.leftJoystickDir.y = 1.f;
	else if (yAxis > deadZoneY) m_ControllerInfo.leftJoystickDir.y = -1.f;
	else m_ControllerInfo.leftJoystickDir.y = 0.f;

	m_ControllerInfo.pressingButtonX = SDL_GameControllerGetButton(m_pSDLGameController, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A);

	if (SDL_GameControllerGetAxis(m_pSDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERRIGHT))
	{
		float deadZoneRightTrigger{ 0.5f };
		Sint16 axisSint16{ SDL_GameControllerGetAxis(m_pSDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERRIGHT) };
		float axis{ float(axisSint16) / std::numeric_limits<Sint16>::max() };
		m_ControllerInfo.pressingRightTrigger = std::abs(axis) > deadZoneRightTrigger;
	}

	m_ControllerInfo.pressingRightShoulder = SDL_GameControllerGetButton(m_pSDLGameController, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
}

void InputManager::IReset()
{
	m_MouseInfo.pressedLMB = false;
	m_MouseInfo.scrollingMMB = false;
	m_MouseInfo.pressedMMB = false;
	m_MouseInfo.draggingMMB = false;

	m_PressedKeys.clear();
}

void InputManager::IProcessKeyDownEvent(const SDL_KeyboardEvent& e)
{
	switch (e.keysym.sym)
	{
	case SDLK_RETURN:
		m_PressedKeys.emplace(Key::Enter);
		break;
	case SDLK_ESCAPE:
		m_PressedKeys.emplace(Key::Escape);
		break;
	case SDLK_e:
		m_PressedKeys.emplace(Key::E);
		break;
	case SDLK_f:
		m_PressedKeys.emplace(Key::F);
		break;
	}
	if (!m_PressedKeys.empty())
		TriggerCallBacks(Event::KeyPressed);
}

void InputManager::IProcessKeyUpEvent(const SDL_KeyboardEvent& e)
{
}

void InputManager::IProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
	Point2f prevMousePos(m_MouseInfo.pos);
	m_MouseInfo.pos.x = float(e.x);
	m_MouseInfo.pos.y = float(e.y);
	if (m_MouseInfo.pressingMMB)
	{
		m_MouseInfo.draggingMMB = true;
		m_MouseInfo.dragDist.x = m_MouseInfo.pos.x - prevMousePos.x;
		m_MouseInfo.dragDist.y = m_MouseInfo.pos.y - prevMousePos.y;
		TriggerCallBacks(Event::DraggingMMB);
	}
}

void InputManager::IProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
{
	switch (e.button)
	{
	case SDL_BUTTON_LEFT:
		m_MouseInfo.pressedLMB = true;
		m_MouseInfo.pressingLMB = true;
		TriggerCallBacks(Event::ClickedLMB);
		break;
	case SDL_BUTTON_MIDDLE:
		m_MouseInfo.pressedMMB = true;
		m_MouseInfo.pressingMMB = true;
		break;
	}
}

void InputManager::IProcessMouseUpEvent(const SDL_MouseButtonEvent& e)
{
	switch (e.button)
	{
	case SDL_BUTTON_LEFT:
		m_MouseInfo.pressingLMB = false;
		break;
	case SDL_BUTTON_MIDDLE:
		m_MouseInfo.pressingMMB = false;
		break;
	}
}

void InputManager::IProcessMouseWheelEvent(int direction)
{
	m_MouseInfo.scrollingMMB = true;
	m_MouseInfo.scrollDir = direction;
	TriggerCallBacks(Event::ScrollingMMB);
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
	InputManager& im{ Get() };
	im.m_pKeyStates = SDL_GetKeyboardState(nullptr);
	im.m_pSDLGameController = SDLGameController;
}

void InputManager::ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
{
	Get().IProcessKeyDownEvent(e);
}

void InputManager::ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
{
	Get().IProcessKeyUpEvent(e);
}

void InputManager::ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
	Get().IProcessMouseMotionEvent(e);
}

void InputManager::ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
{
	Get().IProcessMouseDownEvent(e);
}

void InputManager::ProcessMouseUpEvent(const SDL_MouseButtonEvent& e)
{
	Get().IProcessMouseUpEvent(e);
}

void InputManager::ProcessMouseWheelEvent(int direction)
{
	Get().IProcessMouseWheelEvent(direction);
}

void InputManager::Update()
{
	Get().IUpdate();
}

void InputManager::Reset()
{
	Get().IReset();
}

void InputManager::RegisterCallback(Event e, Callback callback)
{
	Get().m_CallBacks[e].push_back(callback);
}

bool InputManager::IsKeyPressed(Key key)
{
	const InputManager& am{ Get() };
	return am.m_PressedKeys.find(key) != am.m_PressedKeys.end();
}

const InputManager::MouseInfo& InputManager::GetMouseInfo()
{
	return Get().m_MouseInfo;
}

const InputManager::ControllerInfo& InputManager::GetControllerInfo()
{
	return Get().m_ControllerInfo;
}
