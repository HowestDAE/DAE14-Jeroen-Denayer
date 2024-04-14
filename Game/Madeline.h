#pragma once
#include "Game.h"
#include <unordered_map>

//Forward declarations
class MultiSpriteSheet;
struct Game::InputActions;

class Madeline final
{
public:
	enum class State
	{
		Idle, Running, Jumping, EndingJump, GroundJumping, WallJumping, WallHopping, WallNeutralJumping, Falling, Crouching, WallGrabbing, WallClimbing, WallSliding, Dashing
	};

	struct MovementParameters
	{
		bool useCurVelAsInitial; //ensures a smooth transition between 2 velocities
		float initVel; //the current velocity is always set to this initial velocity
		float maxSpeed; //must have direction included
		float acc; //direction is automatically set to reach the desired velocity
		bool allowDirChange; //if the input dir affects if maxSpeed is + or -
		bool multiplyInitVelByWallDir; //if the initVel is + or - based on the wall direction
		bool multiplyInitVelByInputDir; //if the initVel is + or - based on the input direction
	};

	struct StateInfo
	{
		std::string name;
		std::string animation;
		MovementParameters* x;
		MovementParameters* y;
	};

	explicit Madeline(Point2f pos, float width, float height, Level* pLevel);
	~Madeline();

	void Draw() const;
	void Update(float dt, const Game::InputActions& input);
	Point2f GetPosition() const;
private:
	//Functions
	void SetState(const Game::InputActions& input);
	void UpdateVel(float dt, float& vel, float& acc, float dir, MovementParameters& movementInfo);
	void UpdateMultiSpriteSheet(float dt);

	//Members
	MultiSpriteSheet* m_pMultiSpriteSheet;
	Level* m_pLevel;
	State m_State;
	StateInfo* m_pStateInfo;
	std::vector<StateInfo> m_StateInfoArr;
	std::unordered_map<std::string, MovementParameters> m_MovementParametersMap;
	Rectf m_Bounds;
	Vector2f m_Vel;
	Vector2f m_Acc;
	BoolXY m_AllowDirChange;

	//State parameters
	bool m_OnGround;
	bool m_CanJump;
	bool m_Jumping;
	bool m_AgainstWall;
	bool m_AgainstRightWall;
	bool m_AgainstLeftWall;
	float m_DistFromWall;
	float m_MaxDistFromWallToWallJump;
	bool m_CanDash;
	bool m_Dashing;
};

