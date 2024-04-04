#pragma once
#include "Game.h"

//Forward declarations
struct Game::InputActions;

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
		float dashDist;
		float dashDistTime;
	};

	enum class State
	{
		Idle, Running, Jumping, EndingJump, GroundJumping, WallJumping, WallHopping, WallNeutralJumping, Falling, Crouching, WallGrabbing, WallClimbing, WallSliding, Dashing
	};
	std::vector<std::string> m_StateNames{ "Idle", "Running", "Jumping", "EndingJump", "GroundJumping", "WallJumping", "WallHopping", "NeutralJumping", "Falling", "Crouching", "WallGrabbing", "WallClimbing", "WallSliding", "Dashing"};


	explicit Madeline(Point2f pos, float width, float height, const MadelineData& data, Level* pLevel);
	~Madeline();

	void Draw() const;
	void Update(float dt, const Game::InputActions& input);
	void SetPosition(const Point2f& pos);
	Point2f GetPosition() const;
private:
	//Functions
	void SetState(const Game::InputActions& input);
	void SetMaxVelByState(const Game::InputActions& input);
	void SetMovementParameters(Axis axis, float initVel, float targetVel, float time = 0.f, float accOverride = 0.F, bool allowDirChange = true, int allowDirChangeDirOverride = 2);
	void UpdateVelUsingAcc(float dt, Axis axis);
	void SetMaximumVelAndAccByDir(int inputIDr, Axis axis);

	void DrawCollision() const;
	void DrawCollisionTile(TileIdx tileIdx, const Color4f& color) const;

	//Members
	Level* m_pLevel;
	State m_State;
	Rectf m_Bounds;
	Vector2i m_MovementDir;
	Vector2f m_Vel;
	Vector2f m_MaxVel;
	Vector2f m_Acc;
	bool m_AllowDirChangeX;
	bool m_AllowDirChangeY;
	Vector2i m_AllowDirChangeDirOverride;

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
	float m_DashDist;
	float m_DashDistTime;

	//State parameters
	bool m_OnGround;
	bool m_CanJump;
	bool m_Jumping;
	bool m_AgainstWall;
	bool m_AgainstRightWall;
	bool m_AgainstLeftWall;
	float m_DistFromWall;
	bool m_CanDash;
	bool m_Dashing;

	//Input bools
	bool m_HoldingJump;
};

