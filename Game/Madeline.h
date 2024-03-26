#pragma once
#include "Game.h"

class Madeline final
{
public:
	enum class Direction
	{
		Left, Up, Right, Down, LeftUp, RightUp, RightDown, LeftDown
	};

	enum class Action
	{
		MovingLeft, MovingRight, MovingDown, MovingUp, Jumping, Dashing
	};

	enum class State
	{
		Idle, Running, Jumping, Crouching, Hanging, Falling
	};

	explicit Madeline(const Point2f pos, float width, float height);

	void Draw() const;
	void Update(float dt, std::vector<Action>);
private:
	struct VelBasedCornerTiles
	{
		TileIdx cornerTile;
		TileIdx xCornerTile;
		TileIdx yCornerTile;
	};

	//Functions
	void UpdateJumpVel(float dt, bool jumpKeyPressed);
	void ResetJump();
	void CheckCollisionDirection(bool& xCollision, bool& yCollision, bool& cornerCollision) const;
	float GetCollisionInfo(const Rectf& b1, const Vector2f vel, const float collX, const float collY, float& normalX, float& normalY) const;
	
	//Utility
	Rectf GetBounds() const;
	void SetVelBasedCornerTiles();
	bool ContainsAction(std::vector<Action> actions, Action action);
	void DrawCollision() const;
	void DrawCollisionTile(TileIdx tileIdx, const Color4f& color) const;

	//Members
	State m_State;
	Rectf m_Bounds;

	Vector2f m_Vel;
	Vector2f m_Acc;

	float m_RunningSpeed;

	float m_Jump_Speed;
	float m_PercentageInJump;
	bool m_CanJump;
	bool m_Jumping;

	float m_Max_Fall_Speed;

	bool m_OnGround;
	bool m_AgainstWall;

	bool m_MovingRight;
	bool m_MovingUp;
	bool m_MovingInX;
	bool m_MovingInY;

	VelBasedCornerTiles m_VelBasedCornerTiles;
};

