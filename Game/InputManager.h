#pragma once
#include <unordered_map>
#include <unordered_set>
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

	static void ProcessKeyDownEvent(const SDL_KeyboardEvent& e);
	static void ProcessKeyUpEvent(const SDL_KeyboardEvent& e);
	static void ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e);
	static void ProcessMouseDownEvent(const SDL_MouseButtonEvent& e);
	static void ProcessMouseUpEvent(const SDL_MouseButtonEvent& e);
	static void ProcessMouseWheelEvent(int direction);

	enum class Event
	{
		ClickedLMB, ScrollingMMB, DraggingMMB, KeyPressed
	};

	enum class Key
	{
		Enter, Escape, E, F, G
	};

	struct MouseInfo
	{
		Vector2f pos;
		bool pressedLMB;
		bool pressingLMB;
		bool pressedMMB;
		bool pressingMMB;
		bool draggingMMB;
		Vector2f dragDist;
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

	typedef std::function<void()> Callback;
	static void RegisterCallback(Event e, Callback callback);

	static bool IsKeyPressed(Key key);
	static const MouseInfo& GetMouseInfo();
	static const ControllerInfo& GetControllerInfo();
private:
	InputManager();

	//Internal implementation of public functions
	void IUpdate();
	void IReset();
	void IProcessKeyDownEvent(const SDL_KeyboardEvent& e);
	void IProcessKeyUpEvent(const SDL_KeyboardEvent& e);
	void IProcessMouseMotionEvent(const SDL_MouseMotionEvent& e);
	void IProcessMouseDownEvent(const SDL_MouseButtonEvent& e);
	void IProcessMouseUpEvent(const SDL_MouseButtonEvent& e);
	void IProcessMouseWheelEvent(int direction);

	//Functions
	void TriggerCallBacks(Event e);

	//Members
	const Uint8* m_pKeyStates;
	SDL_GameController* m_pSDLGameController;
	MouseInfo m_MouseInfo;
	ControllerInfo m_ControllerInfo;
	std::unordered_set<Key> m_PressedKeys;
	std::unordered_map<Event, std::vector<Callback>> m_CallBacks;
};

//enum class Mouse
//{

//};

//enum class Controller
//{
//	LeftJoystick, X, RightShoulder, RightBumper
//};

