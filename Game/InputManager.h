#pragma once
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include "GameData.h"

class InputManager final
{
public:
	static InputManager& Get();
	static void Update();
	static void Reset();
	static bool HandleEvent(SDL_Event& e);

	InputManager(const InputManager& other) = delete;
	InputManager& operator=(const InputManager& other) = delete;
	InputManager(InputManager&& other) = delete;
	InputManager& operator=(InputManager&& other) = delete;

	struct MouseInfo
	{
		Vector2f pos;
		bool moving;
		bool pressingLMB;
		bool pressingMMB;
		Vector2f dragDist;
		int scrollDir;
	};

	struct ControllerInfo
	{
		Vector2i leftJoystickDir;
	};

	enum class MouseEvent
	{
		None, ClickedLMB, ReleasedLMB, DraggingLMB, MovingLMB, ClickedMMB, ScrollingMMB, DraggingMMB
	};

	enum class Key
	{
		None, Enter, Escape, E, F, C, B, D, Up, Down, ctrl, S
	};

	enum class GameAction
	{
		None, Jump, Dash, Grab
	};

	struct CallbackInfo
	{
		std::function<void()> callback;
		GameData::Mode mode;
		MouseEvent mouseEvent;
		Key key;
		GameAction gameAction;
	};

	static void RegisterCallback(MouseEvent mouseEvent, std::function<void()> callback, GameData::Mode mode);
	static void RegisterCallback(Key key, std::function<void()> callback, GameData::Mode mode);
	static void RegisterCallback(GameAction gameAction, std::function<void()> callback, GameData::Mode mode);

	static bool IsMouseEventTriggered(MouseEvent mouseEvent);
	static bool IsKeyPressed(Key key);
	static bool IsGameActionTriggered(GameAction gameAction);
	static const MouseInfo& GetMouseInfo();
	static const ControllerInfo& GetControllerInfo();
private:
	InputManager();
	~InputManager();

	//Internal implementation of public functions
	void IUpdate();
	void IReset();
	bool IHandleEvent(SDL_Event& e);

	//SDL Event Functions
	void ProcessKeyEvent(const SDL_KeyboardEvent& e, bool keyDown);
	void ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e);
	void ProcessMouseButtonEvent(const SDL_MouseButtonEvent& e, bool buttonDown);
	void ProcessMouseWheelEvent(int direction);
	void ProcessControllerButtonEvent(const SDL_ControllerButtonEvent& e, bool buttonDown);

	//Functions
	void TriggerCallbacks();
	SDL_GameController* FindController();
	void AddRemoveKey(Key key, bool keyDown);

	//Members
	const Uint8* m_pKeyStates;
	SDL_GameController* m_pSDLGameController;

	MouseInfo m_MouseInfo;
	ControllerInfo m_ControllerInfo;
	std::unordered_set<MouseEvent> m_MouseEvents;
	std::unordered_set<Key> m_PressedKeys;
	std::unordered_set<GameAction> m_GameActions;
	std::vector<CallbackInfo> m_Callbacks;
};
