#include "pch.h"
#include "InputManager.h"

InputManager::InputManager()
	: m_pKeyStates{ SDL_GetKeyboardState(nullptr) }
	, m_pSDLGameController{ FindController() }
	, m_MouseInfo{ MouseInfo{} }
	, m_ControllerInfo{ ControllerInfo{} }
	, m_MouseEvents{ std::unordered_set<MouseEvent>{} }
	, m_PressedKeys{ std::unordered_set<Key>{} }
	, m_GameActions{ std::unordered_set<GameAction>{} }
	, m_Callbacks{ std::vector<CallbackInfo>{} }
{
}

InputManager::~InputManager()
{
	SDL_GameControllerClose(m_pSDLGameController);
}

InputManager& InputManager::Get()
{
	static InputManager instance;
	return instance;
}

void InputManager::Update() { Get().IUpdate(); }

void InputManager::Reset() { Get().IReset(); }

bool InputManager::HandleEvent(SDL_Event& e) { return Get().IHandleEvent(e); }

void InputManager::RegisterCallback(MouseEvent mouseEvent, std::function<void()> callback, GameData::Mode mode)
{
	Get().m_Callbacks.push_back(CallbackInfo{ callback, mode, mouseEvent, Key::None, GameAction::None });
}

void InputManager::RegisterCallback(Key key, std::function<void()> callback, GameData::Mode mode)
{
	Get().m_Callbacks.push_back(CallbackInfo{ callback, mode, MouseEvent::None, key, GameAction::None });
}

void InputManager::RegisterCallback(GameAction gameAction, std::function<void()> callback, GameData::Mode mode)
{
	Get().m_Callbacks.push_back(CallbackInfo{ callback, mode, MouseEvent::None, Key::None, gameAction });
}

bool InputManager::IsMouseEventTriggered(MouseEvent mouseEvent)
{
	return Get().m_MouseEvents.find(mouseEvent) != Get().m_MouseEvents.end();
}

bool InputManager::IsKeyPressed(Key key)
{
	return Get().m_PressedKeys.find(key) != Get().m_PressedKeys.end();
}

bool InputManager::IsGameActionTriggered(GameAction gameAction)
{
	return Get().m_GameActions.find(gameAction) != Get().m_GameActions.end();
}

const InputManager::MouseInfo& InputManager::GetMouseInfo() { return Get().m_MouseInfo; }

const InputManager::ControllerInfo& InputManager::GetControllerInfo() { return Get().m_ControllerInfo; }

void InputManager::IUpdate()
{
	//Controller Input
	if (m_pKeyStates[SDL_SCANCODE_LEFT])
		m_ControllerInfo.leftJoystickDir.x -= 1;
	else if (m_pKeyStates[SDL_SCANCODE_RIGHT])
		m_ControllerInfo.leftJoystickDir.x += 1;
	else
	{
		float deadZoneX{ 0.4f };
		Sint16 xAxisSint16{ SDL_GameControllerGetAxis(m_pSDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) };
		float xAxis{ float(xAxisSint16) / std::numeric_limits<Sint16>::max() };
		if (xAxis < -deadZoneX) m_ControllerInfo.leftJoystickDir.x = -1;
		else if (xAxis > deadZoneX) m_ControllerInfo.leftJoystickDir.x = 1;
		else m_ControllerInfo.leftJoystickDir.x = 0;
	}

	if (m_pKeyStates[SDL_SCANCODE_DOWN])
	{
		m_ControllerInfo.leftJoystickDir.y -= 1;
		m_PressedKeys.emplace(Key::Down);
	}
	else if (m_pKeyStates[SDL_SCANCODE_UP])
	{
		m_ControllerInfo.leftJoystickDir.y += 1;
		m_PressedKeys.emplace(Key::Down);
	}
	else
	{
		float deadZoneY{ 0.8f };
		Sint16 yAxisSint16{ SDL_GameControllerGetAxis(m_pSDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) };
		float yAxis{ float(yAxisSint16) / std::numeric_limits<Sint16>::max() };
		if (yAxis < -deadZoneY) m_ControllerInfo.leftJoystickDir.y = 1;
		else if (yAxis > deadZoneY) m_ControllerInfo.leftJoystickDir.y = -1;
		else m_ControllerInfo.leftJoystickDir.y = 0;
	}

	if (SDL_GameControllerGetAxis(m_pSDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERRIGHT))
	{
		float deadZoneRightTrigger{ 0.5f };
		Sint16 axisSint16{ SDL_GameControllerGetAxis(m_pSDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERRIGHT) };
		float axis{ float(axisSint16) / std::numeric_limits<Sint16>::max() };
		if (std::abs(axis) > deadZoneRightTrigger)
			m_GameActions.emplace(GameAction::Grab);
		else
			m_GameActions.erase(GameAction::Grab);
	}

	TriggerCallbacks();
}

void InputManager::IReset()
{
	m_ControllerInfo.leftJoystickDir.x = 0;
	m_ControllerInfo.leftJoystickDir.y = 0;

	m_MouseInfo.dragDist = Vector2f{};
	m_MouseInfo.scrollDir = 0;

	m_MouseEvents.erase(MouseEvent::MovingLMB);
	m_MouseEvents.erase(MouseEvent::ScrollingMMB);
	m_MouseEvents.erase(MouseEvent::DraggingMMB);
}

bool InputManager::IHandleEvent(SDL_Event& e)
{
	bool handleEvents{ true };
	switch (e.type)
	{
	case SDL_QUIT:
		handleEvents = false;
		break;
	case SDL_KEYDOWN:
		ProcessKeyEvent(e.key, true);
		break;
	case SDL_KEYUP:
		ProcessKeyEvent(e.key, false);
		break;
	case SDL_MOUSEMOTION:
		e.motion.y = int(GameData::VIEWPORT().height) - e.motion.y;
		ProcessMouseMotionEvent(e.motion);
		break;
	case SDL_MOUSEBUTTONDOWN:
		e.button.y = int(GameData::VIEWPORT().height) - e.button.y;
		ProcessMouseButtonEvent(e.button, true);
		break;
	case SDL_MOUSEBUTTONUP:
		e.button.y = int(GameData::VIEWPORT().height) - e.button.y;
		ProcessMouseButtonEvent(e.button, false);
		break;
	case SDL_MOUSEWHEEL:
		ProcessMouseWheelEvent(e.wheel.y);
		break;
	case SDL_CONTROLLERBUTTONDOWN:
		ProcessControllerButtonEvent(e.cbutton, true);
		break;
	case SDL_CONTROLLERBUTTONUP:
		ProcessControllerButtonEvent(e.cbutton, false);
		break;
	case SDL_CONTROLLERDEVICEADDED:
		if (!m_pSDLGameController)
		{
			//m_SDLGameController = SDL_GameControllerOpen(e.cdevice.which);
			m_pSDLGameController = FindController();
		}
		break;
	case SDL_CONTROLLERDEVICEREMOVED:
		if (m_pSDLGameController && e.cdevice.which == SDL_JoystickInstanceID(
			SDL_GameControllerGetJoystick(m_pSDLGameController)))
		{
			std::cout << "Disconnected Game Controller" << std::endl;
			SDL_GameControllerClose(m_pSDLGameController);
			m_pSDLGameController = FindController();
		}
		break;
	}
	return handleEvents;
}

void InputManager::ProcessKeyEvent(const SDL_KeyboardEvent& e, bool keyDown)
{
	switch (e.keysym.sym)
	{
	case SDLK_RETURN:
		if (keyDown)
			m_PressedKeys.emplace(Key::Enter);
		else
			m_PressedKeys.erase(Key::Enter);
		break;
	case SDLK_ESCAPE:
		if (keyDown)
			m_PressedKeys.emplace(Key::Escape);
		else
			m_PressedKeys.erase(Key::Escape);
		break;
	case SDLK_SPACE: case SDLK_UP:
		if (keyDown)
			m_GameActions.emplace(GameAction::Jump);
		else
			m_GameActions.erase(GameAction::Jump);
		break;
	case SDLK_g:
		if (keyDown)
			m_GameActions.emplace(GameAction::Grab);
		else
			m_GameActions.erase(GameAction::Grab);
		break;
	case SDLK_d:
		if (keyDown)
			m_GameActions.emplace(GameAction::Dash);
		else
			m_GameActions.erase(GameAction::Dash);
		break;
	case SDLK_e:
		if (keyDown)
			m_PressedKeys.emplace(Key::E);
		else
			m_PressedKeys.erase(Key::E);
		break;
	case SDLK_f:
		if (keyDown)
			m_PressedKeys.emplace(Key::F);
		else
			m_PressedKeys.erase(Key::F);
		break;
	}
}

void InputManager::ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
	Point2f prevMousePos(m_MouseInfo.pos);
	m_MouseInfo.pos.x = float(e.x);
	m_MouseInfo.pos.y = float(e.y);
	m_MouseEvents.emplace(MouseEvent::MovingLMB);
	if (IsMouseEventTriggered(MouseEvent::ClickedMMB))
	{
		m_MouseInfo.dragDist.x = m_MouseInfo.pos.x - prevMousePos.x;
		m_MouseInfo.dragDist.y = m_MouseInfo.pos.y - prevMousePos.y;
		m_MouseEvents.emplace(MouseEvent::DraggingMMB);
	}
}

void InputManager::ProcessMouseButtonEvent(const SDL_MouseButtonEvent& e, bool buttonDown)
{
	switch (e.button)
	{
	case SDL_BUTTON_LEFT:
		if (buttonDown)
			m_MouseEvents.emplace(MouseEvent::ClickedLMB);
		else
			m_MouseEvents.erase(MouseEvent::ClickedLMB);
		break;
	case SDL_BUTTON_MIDDLE:
		if (buttonDown)
			m_MouseEvents.emplace(MouseEvent::ClickedMMB);
		else
			m_MouseEvents.erase(MouseEvent::ClickedMMB);
		break;
	}
}

void InputManager::ProcessMouseWheelEvent(int direction)
{
	m_MouseInfo.scrollDir = direction;
	m_MouseEvents.emplace(MouseEvent::ScrollingMMB);
}

void InputManager::ProcessControllerButtonEvent(const SDL_ControllerButtonEvent& e, bool buttonDown)
{
	switch (e.button)
	{
	case SDL_CONTROLLER_BUTTON_START:
		if (buttonDown)
			m_PressedKeys.emplace(Key::Escape);
		else
			m_PressedKeys.erase(Key::Escape);
		break;
	case SDL_CONTROLLER_BUTTON_A:
		if (buttonDown)
		{
			m_GameActions.emplace(GameAction::Jump);
			m_PressedKeys.emplace(Key::Enter);
		}
		else
		{
			m_GameActions.erase(GameAction::Jump);
			m_PressedKeys.erase(Key::Enter);
		}
		break;
	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
		if (buttonDown)
			m_GameActions.emplace(GameAction::Dash);
		else
			m_GameActions.erase(GameAction::Dash);
		break;
	case SDL_CONTROLLER_BUTTON_B:
		if (buttonDown)
			m_PressedKeys.emplace(Key::Escape);
		else
			m_PressedKeys.erase(Key::Escape);
		break;
	}
}

void InputManager::TriggerCallbacks()
{
	for (CallbackInfo& info : m_Callbacks)
	{
		if (info.mode == GameData::GetMode() &&
			(info.mouseEvent == MouseEvent::None || IsMouseEventTriggered(info.mouseEvent)) &&
			(info.key == Key::None || IsKeyPressed(info.key)) && 
			(info.gameAction == GameAction::None || IsGameActionTriggered(info.gameAction)) )
			info.callback();
	}
}

SDL_GameController* InputManager::FindController()
{
	for (int i = 0; i < SDL_NumJoysticks(); i++) {
		if (SDL_IsGameController(i)) {
			std::cout << "Found Game Controller" << std::endl;
			return SDL_GameControllerOpen(i);
		}
	}
	return nullptr;
}
