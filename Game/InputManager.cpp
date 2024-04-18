#include "pch.h"
#include "InputManager.h"

InputManager::InputManager(SDL_GameController* SDLGameController)
	: m_SDLGameController{ SDLGameController }
	, m_Dir{}
	, m_Jumping{}
	, m_Grabbing{}
	, m_Dashing{}
{
}

void InputManager::Update()
{
	float deadZoneX{ 0.4f };
	Sint16 xAxisSint16{ SDL_GameControllerGetAxis(m_SDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) };
	float xAxis{ float(xAxisSint16) / std::numeric_limits<Sint16>::max() };
	if (xAxis < -deadZoneX) m_Dir.x = -1.f;
	else if (xAxis > deadZoneX) m_Dir.x = 1.f;
	else m_Dir.x = 0.f;

	float deadZoneY{ 0.8f };
	Sint16 yAxisSint16{ SDL_GameControllerGetAxis(m_SDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) };
	float yAxis{ float(yAxisSint16) / std::numeric_limits<Sint16>::max() };
	if (yAxis < -deadZoneY) m_Dir.y = 1.f;
	else if (yAxis > deadZoneY) m_Dir.y = -1.f;
	else m_Dir.y = 0.f;

	m_Jumping = SDL_GameControllerGetButton(m_SDLGameController, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A);

	if (SDL_GameControllerGetAxis(m_SDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERRIGHT))
	{
		float deadZoneRightTrigger{ 0.5f };
		Sint16 axisSint16{ SDL_GameControllerGetAxis(m_SDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERRIGHT) };
		float axis{ float(axisSint16) / std::numeric_limits<Sint16>::max() };
		m_Grabbing = std::abs(axis) > deadZoneRightTrigger;
	}

	m_Dashing = SDL_GameControllerGetButton(m_SDLGameController, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
}

Vector2i InputManager::GetDir() const
{
	return m_Dir;
}

bool InputManager::IsJumping() const
{
	return m_Jumping;
}

bool InputManager::IsGrabbing() const
{
	return m_Grabbing;
}

bool InputManager::IsDashing() const
{
	return m_Dashing;
}
