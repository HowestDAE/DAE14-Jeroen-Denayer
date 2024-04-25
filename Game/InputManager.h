#pragma once
#include <unordered_map>
#include <functional>

class InputManager final
{
public:
	static InputManager& Get();
	static void Init(SDL_GameController* SDLGameController);
	static void Update();
	static void Reset();

	InputManager(const InputManager& other) = delete;
	InputManager& operator=(const InputManager& other) = delete;
	InputManager(InputManager&& other) = delete;
	InputManager& operator=(InputManager&& other) = delete;

	static void ProcessKeyDownEvent(const SDL_KeyboardEvent& e);
	static void ProcessKeyUpEvent(const SDL_KeyboardEvent& e);
	static void ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e);
	static void ProcessMouseDownEvent(const SDL_MouseButtonEvent& e);
	static void ProcessMouseUpEvent(const SDL_MouseButtonEvent& e);
	static void ProcessMouseWheelEvent(int direction);

	enum class Event
	{
		ClickedLMB, ScrollingMMB, PressedEnter, PressedEscape, PressedE
	};

	struct MouseInfo
	{
		Point2f pos;
		bool pressedLMB;
		bool pressingLMB;
		bool draggingLMB;
		Point2f dragDist;
		bool scrollingMMB;
		int scrollDir;
	};

	struct ControllerInfo
	{
		Vector2i leftJoystickDir;
		bool pressingButtonX;
		bool pressingRightShoulder;
		bool pressingRightTrigger;
	};

	struct KeyInfo
	{
		bool pressedEnter;
		bool pressedEscape;
		bool pressedE;
	};

	typedef std::function<void()> Callback;
	static void RegisterCallback(Event e, Callback callback);

	static MouseInfo& GetMouseInfo();
	static ControllerInfo& GetControllerInfo();
	static KeyInfo& GetKeyInfo();
private:
	InputManager();

	static void TriggerCallBacks(Event e);

	static const Uint8* s_pKeyStates;
	static SDL_GameController* s_SDLGameController;
	static MouseInfo s_MouseInfo;
	static ControllerInfo s_ControllerInfo;
	static KeyInfo s_KeyInfo;

	static std::unordered_map<Event, std::vector<Callback>> m_CallBacks;
};

//enum class Mouse
//{

//};

//enum class Controller
//{
//	LeftJoystick, X, RightShoulder, RightBumper
//};

