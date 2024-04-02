#pragma once
#include "Game.h"

class Madeline final
{
public:
	struct MadelineData
	{
		float groundJumpHeight;
		float groundJumpTime;
		float runningSpeed;
		float wallNeutralJumpDistX;
		float wallNeutralJumpDistXTime;
		float wallJumpDistX;
		float wallJumpDistXTime;
		float maxFallSpeed;
		float wallClimbingSpeed;
		float wallSlidingSpeed;
		float maxDistFromWallToWallJump;
	};

	/*rectilinear motion constant acceleration,
	acc and vel needed to travel a dist in time*/
	struct RMCAInfo
	{
		bool completed;
		float initVel;
		float deceleration;
		float time;
		float percentage;
	};

	struct VelTransitionInfo
	{
		bool completed;
		float v0;
		float v1;
		float time;
		float percentage;
	};

	enum class Direction
	{
		Left, Up, Right, Down, LeftUp, RightUp, RightDown, LeftDown
	};

	enum class Axis
	{
		X, Y
	};

	enum class Action
	{
		Jumping, Grabbing, Dashing
	};

	enum class State
	{
		Idle, Running, EndingJump, GroundJumping, WallJumping, WallHopping, WallNeutralJumping, Falling, Crouching, WallGrabbing, WallClimbing, WallSliding
	};
	std::vector<std::string> m_StateNames{ "Idle", "Running", "EndingJump", "GroundJumping", "WallJumping", "WallHopping", "NeutralJumping", "Falling", "Crouching", "WallGrabbing", "WallClimbing", "WallSliding"};


	explicit Madeline(int spawnRow, int spawnCol, float width, float height, const MadelineData& data);

	void Draw() const;
	void Update(float dt, Vector2i inputDir, const std::vector<Action>& actions);
	void SetPosition(const Point2f& pos);
private:
	//Functions
	void SetState(const Vector2i& inputDir, const std::vector<Action>& actions);
	void SetTargetVelByState();
	void SetMaximumVel(float maxVel, bool allowDirChange, Axis axis, bool setVelImmediatly = true, float time = 0.f, float accOverride = 0.f);
	void SetRMCAParameters(float dist, float time, int dir, RMCAInfo& RMCAInfo);
	void UpdateVelUsingAcc(float dt, Axis axis);
	void UpdateVelUsingRMCA(float dt, bool condition, float& vel, RMCAInfo& RMCAInfo);
	void SetMaximumVelAndAccByDir(int inputIDr, Axis axis);
	void ActivateJump();

	bool ContainsAction(std::vector<Action> actions, Action action);
	void DrawCollision() const;
	void DrawCollisionTile(TileIdx tileIdx, const Color4f& color) const;

	//Members
	State m_State;
	Rectf m_Bounds;
	Vector2i m_MovementDir;
	Vector2f m_Vel;
	Vector2f m_MaxVel;
	Vector2f m_Acc;
	bool m_AllowDirChangeX;
	bool m_AllowDirChangeY;
	RMCAInfo m_RMCAX;
	RMCAInfo m_RMCAY;

	//Movement parameters
	float m_GroundJumpHeight;
	float m_GroundJumpTime;
	float m_RunningSpeed;
	float m_WallNeutralJumpDistX;
	float m_WallNeutralJumpDistXTime;
	float m_WallJumpDistX;
	float m_WallJumpDistXTime;
	float m_MaxFallSpeed;
	float m_WallClimbingSpeed;
	float m_WallSlidingSpeed;
	float m_MaxDistFromWallToWallJump;

	//State parameters
	bool m_OnGround;
	bool m_CanJump;
	bool m_Jumping;
	bool m_AgainstWall;
	bool m_AgainstRightWall;
	bool m_AgainstLeftWall;
	float m_DistFromWall;

	//Input bools
	bool m_HoldingJump;
	bool m_HoldingGrab;
};

