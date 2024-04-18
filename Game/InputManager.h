#pragma once

class InputManager final
{
public:
	InputManager(SDL_GameController* SDLGameController);

	void Update();
	Vector2i GetDir() const;
	bool IsJumping() const;
	bool IsGrabbing() const;
	bool IsDashing() const;
private:
	SDL_GameController* m_SDLGameController;

	Vector2i m_Dir;
	bool m_Jumping;
	bool m_Grabbing;
	bool m_Dashing;
};

