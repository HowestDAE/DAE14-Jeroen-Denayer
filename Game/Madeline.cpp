#include "pch.h"
#include "Madeline.h"
#include "Game.h"
#include "InputManager.h"
#include "AssetManager.h"

Madeline::Madeline(const Point2f& pos, float width, float height)
	: PhysicsBody(PhysicsBody::Type::Madeline, Vector2f{pos.x, pos.y} )
	, m_Alive{ true }
	, m_MultiSpriteSheet{ MultiSpriteSheet{ "MadelineSpritesheet", 14, 14,
		std::unordered_map<std::string, MultiSpriteSheet::SpriteSheetInfo>
			{
				{ "Climbing", { 0, 15, 0.1f } },
				{ "Crouching", {15, 16, 0.1f} },
				{ "Dangling", {16, 26, 0.1f} },
				{ "Dashing", {26, 30, 0.1f} },
				{ "Falling", {30, 38, 0.1f} },
				{ "FallPose", {38, 49, 0.1f} },
				{ "IdleChinScratching", {49, 73, 0.1f} },
				{ "IdleCoughing", {73, 85, 0.1f} },
				{ "IdleLookingAround", {85, 97, 0.1f} },
				{ "IdleStanding", {97, 106, 0.1f} },
				{ "JumpingFast", {106, 110, 0.1f} },
				{ "JumpingSlow", {110, 114, 0.1f} },
				{ "LaunchRecover", {114, 125, 0.1f} },
				{ "Launch", {125, 133, 0.1f} },
				{ "LookUp", {133, 141, 0.1f} },
				{ "Push", {141, 157, 0.1f} },
				{ "RunFast", {157, 169, 0.1f} },
				{ "RunSlow", {169, 181, 0.1f} },
				{ "Walk", {181, 193, 0.1f} }
			}
		}
	}
	, m_State{ State::Idle }
	, m_PrevState{ State::Idle }
	, m_StateInfoArr{
		std::vector<StateInfo>{
			{"Idle"				, "IdleStanding", nullptr, nullptr, ""},
			{"Running"			, "RunFast"		, nullptr, nullptr, ""},
			{"Jumping"			, "JumpingFast"	, nullptr, nullptr, ""},
			{"EndingJump"		, "JumpingSlow"	, nullptr, nullptr, ""},
			{"GroundJumping"	, "JumpingFast"	, nullptr, nullptr, "Jump"},
			{"WallJumping"		, "JumpingFast"	, nullptr, nullptr, "WallJump"},
			{"WallHopping"		, "JumpingFast"	, nullptr, nullptr, "WallHop"},
			{"NeutralJumping"	, "JumpingFast"	, nullptr, nullptr, "WallJump"},
			{"Falling"			, "Falling"		, nullptr, nullptr, ""},
			{"Crouching"		, "Crouching"	, nullptr, nullptr, ""},
			{"WallGrabbing"		, "Climbing"	, nullptr, nullptr, "Grab"},
			{"WallClimbing"		, "Climbing"	, nullptr, nullptr, ""},
			{"WallSliding"		, "Climbing"	, nullptr, nullptr, "WallSlide"},
			{"Dashing"			, "Dashing"		, nullptr, nullptr, "Dash"}
		}
	}
	//State parameters
	, m_OnGround{}
	, m_CanJump{}
	, m_Jumping{}
	, m_AirTime{}
	, m_Grabbing{}
	, m_AgainstWall{}
	, m_AgainstRightWall{}
	, m_AgainstLeftWall{}
	, m_DistFromWall{}
	, m_CanDash{}
	, m_Dashing{}
	//Constants
	, m_LedgeJumpTime{ 0.15f }
	, m_MaxDistFromWallToWallJump{ 2.f }
{
	AddOverlapRect(Vector2f{ m_Pos.x, m_Pos.y }, width, height,
		std::unordered_map<Type, TypeInfo>{
			{ Type::Level, TypeInfo{ true } },
			{ Type::FallingBlock, TypeInfo{true} },
			{ Type::LevelScreenGate, TypeInfo{false} },
			{ Type::Spike, TypeInfo{true} },
			{ Type::Badeline, TypeInfo{false} },
			{ Type::DashCrystal, TypeInfo{false} }},
		true);
	AddOverlapRect(Vector2f{ m_Pos.x - m_MaxDistFromWallToWallJump, m_Pos.y + height / 3 }, width + 2 * m_MaxDistFromWallToWallJump, height / 3, std::unordered_map<Type, TypeInfo>{{ Type::Level, TypeInfo{ false } }, { Type::FallingBlock, TypeInfo{false} }}, true);

	float groundJumpHeight = 3.5f;
	float groundJumpTime = 0.35f;
	AccAndVel groundJumpAccVel{ utils::AccAndVelToTravelDistInTime(groundJumpHeight, groundJumpTime) };
	float gravity{ groundJumpAccVel.acc };
	float runningSpeed = 12.f;
	float runningAcc{ runningSpeed / 0.1f };
	float wallNeutralJumpDistX = 2.f;
	float wallNeutralJumpDistXTime = 0.15f;
	AccAndVel wallNeutralJumpAccVelX{ utils::AccAndVelToTravelDistInTime(wallNeutralJumpDistX, wallNeutralJumpDistXTime) };
	float wallJumpDist = groundJumpHeight;
	float wallJumpDistTime = groundJumpTime;
	AccAndVel wallJumpAccVel{ utils::AccAndVelToTravelDistInTime(wallJumpDist, wallJumpDistTime) };
	float maxFallSpeed = -30.f;
	float wallGrabbingAcc{ maxFallSpeed / 0.25f };
	float wallClimbingSpeed = 8.f;
	float wallClimbingAcc{ wallClimbingSpeed / 0.1f };
	float wallSlidingSpeed = -12.f;
	float wallSlidingAcc{ wallSlidingSpeed / 0.1f };
	float dashDist = 2.f * 4;
	float dashDistTime = 0.5f;
	AccAndVel dashAccVel{ utils::AccAndVelToTravelDistInTime(dashDist, dashDistTime) };

	m_MovementParametersMap = std::unordered_map<std::string, MovementParameters>{
		{"Stationary",		{0.f, 0.f, 0.f, true, true, false, false}},
		{"Running",			{runningSpeed, runningAcc, 0.f, false, true, false, false}},
		{"Gravity",			{maxFallSpeed, gravity, 0.f, false, false, false, false}},
		{"DoubleGravity",	{maxFallSpeed, gravity * 2, 0.f, false, false, false, false}},
		{"GroundJumpingY",	{maxFallSpeed, gravity, groundJumpAccVel.vel, true, false, false, false}},
		{"WallJumpingX",	{0.f, wallJumpAccVel.acc, wallJumpAccVel.vel, true, false, true, false}},
		{"WallJumpingY",	{0.f, wallJumpAccVel.acc, wallJumpAccVel.vel, true, false, false, false}},
		{"WallHoppingY",	{0.f, gravity, groundJumpAccVel.vel, true, false, false, false}},
		{"NeutralJumpingX", {0.f, wallNeutralJumpAccVelX.acc, wallNeutralJumpAccVelX.vel, true, false, true, false}},
		{"NeutralJumpingY", {0.f, groundJumpAccVel.acc, groundJumpAccVel.vel, true, false, false, false}},
		{"WallGrabbingY",	{0.f, wallGrabbingAcc, 0.f, false, false, false, false}},
		{"WallClimbingY",	{wallClimbingSpeed, wallClimbingAcc, 0.f, true, false, false, false}},
		{"WallSlidingY",	{wallSlidingSpeed, wallSlidingAcc, 0.f, false, false, false, false}},
		{"Dashing",			{0.f, dashAccVel.acc, dashAccVel.vel, true, false, false, true}}
	};

	m_StateInfoArr[0].x = &m_MovementParametersMap["Stationary"];
	m_StateInfoArr[0].y = &m_MovementParametersMap["Gravity"];
	m_StateInfoArr[1].x = &m_MovementParametersMap["Running"];
	m_StateInfoArr[1].y = &m_MovementParametersMap["Gravity"];
	m_StateInfoArr[2].x = &m_MovementParametersMap["Running"];
	m_StateInfoArr[2].y = &m_MovementParametersMap["Gravity"];
	m_StateInfoArr[3].x = &m_MovementParametersMap["Running"];
	m_StateInfoArr[3].y = &m_MovementParametersMap["DoubleGravity"];
	m_StateInfoArr[4].x = &m_MovementParametersMap["Running"];
	m_StateInfoArr[4].y = &m_MovementParametersMap["GroundJumpingY"];
	m_StateInfoArr[5].x = &m_MovementParametersMap["WallJumpingX"];
	m_StateInfoArr[5].y = &m_MovementParametersMap["WallJumpingY"];
	m_StateInfoArr[6].x = &m_MovementParametersMap["Stationary"];
	m_StateInfoArr[6].y = &m_MovementParametersMap["WallHoppingY"];
	m_StateInfoArr[7].x = &m_MovementParametersMap["NeutralJumpingX"];
	m_StateInfoArr[7].y = &m_MovementParametersMap["NeutralJumpingY"];
	m_StateInfoArr[8].x = &m_MovementParametersMap["Running"];
	m_StateInfoArr[8].y = &m_MovementParametersMap["Gravity"];
	m_StateInfoArr[9].x = &m_MovementParametersMap["Stationary"];
	m_StateInfoArr[9].y = &m_MovementParametersMap["Gravity"];
	m_StateInfoArr[10].x = &m_MovementParametersMap["Stationary"];
	m_StateInfoArr[10].y = &m_MovementParametersMap["WallGrabbingY"];
	m_StateInfoArr[11].x = &m_MovementParametersMap["Stationary"];
	m_StateInfoArr[11].y = &m_MovementParametersMap["WallClimbingY"];
	m_StateInfoArr[12].x = &m_MovementParametersMap["Stationary"];
	m_StateInfoArr[12].y = &m_MovementParametersMap["WallSlidingY"];
	m_StateInfoArr[13].x = &m_MovementParametersMap["Dashing"];
	m_StateInfoArr[13].y = &m_MovementParametersMap["Dashing"];
}

Madeline::~Madeline()
{
}

void Madeline::Draw(const LevelScreen* pLevelScreen) const
{
	float frameSize{ 32.f };
	const Rectf& rect{ m_OverlapRects[0].rect };
	Rectf dstRect{
		rect.left + rect.width / 2 - frameSize / 2,
		rect.bottom,
		frameSize, frameSize
	};
	m_MultiSpriteSheet.Draw(dstRect);
}

void Madeline::Update(float dt)
{
	SetStateParameters(dt);

	m_PrevState = m_State;
	SetState();

	if (m_State != m_PrevState)
		InitialiseState();

	UpdateState(dt);

}

void Madeline::CollisionInfoResponse(int overlapRectIdx, const CollisionInfo& ci, Type type, const PhysicsBody* pCollisionBody)
{
	if (overlapRectIdx == 0)
	{
		switch (type)
		{
		case PhysicsBody::Type::Spike:
			AssetManager::PlaySoundEffect("Death");
			m_Alive = false;
			break;
		case PhysicsBody::Type::Level:
		{
			bool prevOnGround{ m_OnGround };
			m_OnGround = m_Vel.y <= 0.f && ci.collDir.down;
			if (!prevOnGround && m_OnGround)
				AssetManager::PlaySoundEffect("GroundHit");
			break;
		}
		case PhysicsBody::Type::DashCrystal:
			m_CanDash = true;
			break;
		case PhysicsBody::Type::Badeline:
			//m_State = State::Jumping;
			//m_Jumping = true;
			//float xDist{ -200.f };
			//float yDist{ 100.f };
			//float time{ 1.f };
			//AccAndVel accVelX{ utils::AccAndVelToTravelDistInTime(xDist, time) };
			//AccAndVel accVelY{ utils::AccAndVelToTravelDistInTime(yDist, time) };
			//float angle{ std::atan2f(yDist, xDist) };
			//float acc{ 100.f };
			//SetMovement(Vector2f{ xDist, yDist }, Vector2f{ 0.f, 0.f }, Vector2f{ acc * cosf(angle), acc * sinf(angle) });
			break;
		}
	}
	else if (overlapRectIdx == 1)
	{
		m_AgainstWall = ci.collDir.x;
		m_AgainstRightWall = ci.collDir.right;
		m_AgainstLeftWall = ci.collDir.left;
		if (m_AgainstWall)
		{
			float entryDist{ m_AgainstRightWall ? ci.right.entryDist : ci.left.entryDist };
			m_DistFromWall = m_MaxDistFromWallToWallJump - entryDist;
		}
		else m_DistFromWall = m_MaxDistFromWallToWallJump + 1;
	}
}

bool Madeline::IsAlive() const
{
	return m_Alive;
}

void Madeline::SetAlive(bool alive)
{
	m_Alive = alive;
}

void Madeline::SetState()
{
	Vector2i inputDir{ InputManager::GetControllerInfo().leftJoystickDir };
	if (!m_Dashing && m_CanDash && InputManager::IsGameActionTriggered(InputManager::GameAction::Dash))
	{
		m_CanDash = false;
		m_Dashing = true;
		m_State = State::Dashing;
	}
	else if (m_Dashing) //continue dash
		return; 
	else if (m_CanJump && InputManager::IsGameActionTriggered(InputManager::GameAction::Jump)) //Start a specific jump
	{
		m_Jumping = true;
		m_CanJump = false;
		if (m_DistFromWall == 0.f && InputManager::IsGameActionTriggered(InputManager::GameAction::Grab)) //Grabbing a wall
			m_State = State::WallHopping;
		else if (m_OnGround) //Normal ground jump
			m_State = State::GroundJumping;
		else if (inputDir.x == 0) //Close to a wall and not moving left/right
			m_State = State::WallNeutralJumping;
		else if (m_DistFromWall < m_MaxDistFromWallToWallJump)//Close to a wall and moving left/right
			m_State = State::WallJumping;
		else //ledge jump
			m_State = State::GroundJumping;

	}
	else if (m_Jumping)
	{
		if (InputManager::IsGameActionTriggered(InputManager::GameAction::Jump))
			if ((m_State == State::WallJumping ||
				m_State == State::WallNeutralJumping ||
				m_State == State::WallHopping) &&
				(m_Vel.x == m_StateInfoArr[int(m_State)].x->maxSpeed ||
				m_Vel.x > 0 && inputDir.x > 0 || m_Vel.x < 0 && inputDir.x < 0)) //Transition to normal jump
				m_State = State::Jumping;
			else
				return; //jump is still controlled so keep current state/specific jump
		else
			m_State = State::EndingJump;
	}
	else if (m_Grabbing)
	{
		if (inputDir.y == 0) //Not moving up/down
			m_State = State::WallGrabbing;
		else if (inputDir.y > 0)//Climbing up a wall
			m_State = State::WallClimbing;
		else //Sliding down a wall
			m_State = State::WallSliding;
	}
	else if (m_OnGround && inputDir.x == 0 && inputDir.y < 0)
		m_State = State::Crouching;
	else if (m_OnGround && inputDir.x != 0)
		m_State = State::Running;
	else if (!m_OnGround && !m_Jumping)
		m_State = State::Falling;
	else
		m_State = State::Idle;
}

void Madeline::SetStateParameters(float dt)
{
	float epsilon(0.5f); //in pixels
	//Against the wall and grabbing
	m_Grabbing = m_DistFromWall < epsilon && InputManager::IsGameActionTriggered(InputManager::GameAction::Grab);
	if (!m_OnGround && !m_Grabbing)
		m_AirTime += dt;
	else
		m_AirTime = 0;

	//Can jump and not on ground or against wall or airTime > ledgeJumpTime will set m_CanJump = false or,
	//Can't jump or no jump input but on ground or against wall will set m_CanJump = true
	m_CanJump = (m_CanJump || !InputManager::IsGameActionTriggered(InputManager::GameAction::Jump)) && (m_OnGround || m_AgainstWall) || (m_CanJump && m_AirTime <= m_LedgeJumpTime);
	//m_Jumping will go from true to false if m_Jumping is true and vel <= 0 or m_OnGround
	m_Jumping &= (m_Vel.y > 0.f && !m_OnGround);
	//m_CanDash will go from false to true if no dash input and m_OnGround
	m_CanDash |= (!InputManager::IsGameActionTriggered(InputManager::GameAction::Dash) && m_OnGround);
	//m_Dashing will go from true to false if vel x and y have reached the maxSpeed
	m_Dashing &= !(m_Vel.x == m_StateInfoArr[int(m_State)].x->maxSpeed && m_Vel.y == m_StateInfoArr[int(m_State)].y->maxSpeed);
}

void Madeline::InitialiseState()
{
	//std::cout << "State: " << m_StateInfoArr[int(m_State)].name << std::endl;

	//Remove previous state SoundEffect
	if (m_StateInfoArr[int(m_State)].soundEffect != "")
		AssetManager::RemoveSoundEffect(m_StateInfoArr[int(m_State)].soundEffect);

	int stateIdx{ int(m_State) };
	Vector2i inputDir{ InputManager::GetControllerInfo().leftJoystickDir };
	ApplyMovementParameters(m_TargetVel.x, m_Vel.x, m_Acc.x, *m_StateInfoArr[int(m_State)].x, inputDir.x);
	ApplyMovementParameters(m_TargetVel.y, m_Vel.y, m_Acc.y, *m_StateInfoArr[int(m_State)].y, inputDir.y);

	//Set the MultiSpriteSheet to the correct animation
	m_MultiSpriteSheet.SetSpriteSheetName(m_StateInfoArr[int(m_State)].animation);

	// To-Do: clean up flipping texture part
	//Flip texture if needed
	if (inputDir.x != 0)
	{
		if (m_AgainstWall)
			m_MultiSpriteSheet.Flip(m_AgainstLeftWall);
		else
			m_MultiSpriteSheet.Flip(false);

		if (m_State == State::Dashing)
		{
			m_MultiSpriteSheet.Flip(inputDir.x < 0);
		}
	}

	//Play Sound
	if (m_StateInfoArr[int(m_State)].soundEffect != "")
		AssetManager::PlaySoundEffect(m_StateInfoArr[int(m_State)].soundEffect);
}

void Madeline::UpdateState(float dt)
{
	Vector2i dir{ InputManager::GetControllerInfo().leftJoystickDir };
	if (m_StateInfoArr[int(m_State)].x->allowDirChange)
	{
		m_TargetVel.x = m_StateInfoArr[int(m_State)].x->maxSpeed * dir.x;
		bool flipX{ m_AgainstLeftWall || m_Acc.x < 0 };
		m_MultiSpriteSheet.Flip(flipX);
	}

	if (m_StateInfoArr[int(m_State)].y->allowDirChange)
		m_TargetVel.y = m_StateInfoArr[int(m_State)].y->maxSpeed * dir.y;

	m_MultiSpriteSheet.Update(dt);
}

void Madeline::ApplyMovementParameters(float& targetVel, float& vel, float& acc, const MovementParameters& movementParameters, int inputDir)
{
	targetVel = movementParameters.maxSpeed;
	acc = movementParameters.acc;

	//Apply boolean modifiers
	if (movementParameters.SetInitVel)
		vel = movementParameters.initVel;

	int wallDir{ (m_AgainstRightWall) ? -1 : 1 };
	if (movementParameters.multiplyInitVelByWallDir)
		vel *= wallDir;
	else if (movementParameters.multiplyInitVelByInputDir)
		vel *= inputDir;
}