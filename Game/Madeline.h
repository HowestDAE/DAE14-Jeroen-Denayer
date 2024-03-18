#pragma once
#include "Game.h"

class Madeline final
{
public:
	enum class Direction
	{
		Left, Up, Right, Down
	};

	enum class State
	{
		Idle, Running, Crouching, Hanging, Falling
	};

	Madeline(const Point2f pos, float width, float height);

	void Draw(const Game::GameInfo& gameInfo) const;
	void Update(float dt, const Game::GameInfo& gameInfo);
	void Move(float dt, Direction dir);
	void Jump();
private:
	//Functions
	Rectf GetBounds() const;
	void SetCollisionTiles(const Game::GameInfo& gameInfo);

	//Const Members
	const float m_JUMP_ADDED_VEL;

	//Members
	State m_State;
	Rectf m_Bounds;
	Vector2f m_Vel;
	float m_Speed;
	bool m_Test;
	std::vector<TileIdx> m_CollisionTiles;
};

