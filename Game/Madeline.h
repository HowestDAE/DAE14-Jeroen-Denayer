 #pragma once
#include "PhysicsBody.h"
#include <unordered_map>
#include "MultiSpriteSheet.h"

//Forward declarations

class Madeline final
	:public PhysicsBody
{
public:
	explicit Madeline(const Point2f& pos, float width, float height);
	virtual ~Madeline() override;
	Madeline(const Madeline& other) = delete;
	Madeline& operator=(const Madeline& other) = delete;
	Madeline(Madeline&& other) = delete;
	Madeline& operator=(Madeline&& other) = delete;

	enum class State
	{
		Idle, Running, Jumping, EndingJump, GroundJumping, WallJumping, WallHopping, WallNeutralJumping, Falling, Crouching, WallGrabbing, WallClimbing, WallSliding, Dashing
	};

	enum class CollisionRectNames
	{
		Body, WallDetection
	};

	struct MovementParameters
	{
		float maxSpeed; //must have direction included, is flipped based on input.dir if allowDirChange is true
		float acc; //direction is automatically set to reach the desired velocity
		float initVel; //the current velocity is set to initVel when SetInitVel = true
		bool SetInitVel; //ensures a smooth transition between 2 velocities
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
		std::string soundEffect;
	};

	virtual void Draw(const LevelScreen* pLevelScreen = nullptr) const override;
	virtual void Update(float dt) override;
	virtual void CollisionInfoResponse(int idx, const CollisionInfo& ci) override;

	void ResetDash();
private:
	//Functions
	void SetState();
	void SetStateParameters(float dt);
	void InitialiseState();
	void UpdateState(float dt);
	void ApplyMovementParameters(float& targetVel, float& vel, float& acc, const MovementParameters& movementParameters, int inputDir);

	//Members
	MultiSpriteSheet m_MultiSpriteSheet;
	State m_State;
	State m_PrevState;
	std::vector<StateInfo> m_StateInfoArr;
	std::unordered_map<std::string, MovementParameters> m_MovementParametersMap;

	//State parameters
	bool m_OnGround;
	bool m_CanJump;
	bool m_Jumping;
	float m_AirTime;
	bool m_Grabbing;
	bool m_AgainstWall;
	bool m_AgainstRightWall;
	bool m_AgainstLeftWall;
	float m_DistFromWall;
	bool m_CanDash;
	bool m_Dashing;

	//Constants
	float m_LedgeJumpTime;
	float m_MaxDistFromWallToWallJump;
};

