#include "pch.h"
#include "Madeline.h"
#include "Game.h"
#include "MultiSpriteSheet.h"
#include "InputManager.h"

Madeline::Madeline(Point2f pos, float width, float height, InputManager* pInputManager)
	: PhysicsBody(Rectf{pos.x, pos.y, width, height})
	, m_pInputManager{ pInputManager }
	, m_State{ State::Idle }
	, m_pStateInfo{ nullptr }
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
	AddOverlapRect(Vector2f{m_Bounds.left - m_MaxDistFromWallToWallJump, m_Bounds.bottom + m_Bounds.height / 3 }, m_Bounds.width + 2 * m_MaxDistFromWallToWallJump, m_Bounds.height / 3);

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

	m_StateInfoArr = std::vector<StateInfo>{
		{"Idle"				, "IdleStanding", &m_MovementParametersMap["Stationary"]		, &m_MovementParametersMap["Gravity"]},
		{"Running"			, "RunFast"		, &m_MovementParametersMap["Running"]			, &m_MovementParametersMap["Gravity"]},
		{"Jumping"			, "JumpingFast"	, &m_MovementParametersMap["Running"]			, &m_MovementParametersMap["Gravity"]},
		{"EndingJump"		, "JumpingSlow"	, &m_MovementParametersMap["Running"]			, &m_MovementParametersMap["DoubleGravity"]},
		{"GroundJumping"	, "JumpingFast"	, &m_MovementParametersMap["Running"]			, &m_MovementParametersMap["GroundJumpingY"]},
		{"WallJumping"		, "JumpingFast"	, &m_MovementParametersMap["WallJumpingX"]		, &m_MovementParametersMap["WallJumpingY"]},
		{"WallHopping"		, "JumpingFast"	, &m_MovementParametersMap["Stationary"]		, &m_MovementParametersMap["WallHoppingY"]},
		{"NeutralJumping"	, "JumpingFast"	, &m_MovementParametersMap["NeutralJumpingX"]	, &m_MovementParametersMap["NeutralJumpingY"]},
		{"Falling"			, "Falling"		, &m_MovementParametersMap["Running"]			, &m_MovementParametersMap["Gravity"]},
		{"Crouching"		, "Crouching"	, &m_MovementParametersMap["Stationary"]		, &m_MovementParametersMap["Gravity"]},
		{"WallGrabbing"		, "Climbing"	, &m_MovementParametersMap["Stationary"]		, &m_MovementParametersMap["WallGrabbingY"]},
		{"WallClimbing"		, "Climbing"	, &m_MovementParametersMap["Stationary"]		, &m_MovementParametersMap["WallClimbingY"]},
		{"WallSliding"		, "Climbing"	, &m_MovementParametersMap["Stationary"]		, &m_MovementParametersMap["WallSlidingY"]},
		{"Dashing"			, "Dashing"		, &m_MovementParametersMap["Dashing"]			, &m_MovementParametersMap["Dashing"]}
	};

	m_pStateInfo = &m_StateInfoArr[0];

	m_pMultiSpriteSheet = new MultiSpriteSheet{ "Textures/MadelineSpritesheet.png", 14, 14,
		std::unordered_map<std::string, MultiSpriteSheet::SpriteSheetInfo>{
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
	};
}

Madeline::~Madeline()
{
	delete m_pMultiSpriteSheet;
	m_pMultiSpriteSheet = nullptr;

	//Below not really necessary
	m_pStateInfo = nullptr;
	for (StateInfo& stateInfo : m_StateInfoArr)
	{
		stateInfo.x = nullptr;
		stateInfo.y = nullptr;
	}
}

void Madeline::Draw() const
{
	//utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
	//utils::FillRect(m_Bounds);

	float frameSize{ 32.f };
	Rectf dstRect{
		m_Bounds.left + m_Bounds.width / 2 - frameSize / 2,
		m_Bounds.bottom,
		frameSize, frameSize
	};
	m_pMultiSpriteSheet->Draw(dstRect);

	const Rectf& wallRect{ m_OverlapRects[0] };
	utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
	utils::DrawRect(m_Bounds);
	utils::DrawRect(wallRect);
}

void Madeline::Update(float dt)
{
	SetStateParameters(dt);

	State prevState{ m_State };
	SetState();

	if (m_State != prevState)
		InitialiseState();

	UpdateState(dt);

	std::cout << "State: " << m_pStateInfo->name << std::endl;
}

void Madeline::CollisionInfoResponse(int idx, const CollisionInfo& ci)
{
	switch (CollisionRectNames(idx))
	{
	case CollisionRectNames::Body:
		m_OnGround = m_Vel.y <= 0.f && ci.collDir.down;
		break;
	case CollisionRectNames::WallDetection:
		m_AgainstWall = ci.collDir.x;
		m_AgainstRightWall = ci.collDir.right;
		m_AgainstLeftWall = ci.collDir.left;
		if (m_AgainstWall)
		{
			float entryDist{ m_AgainstRightWall ? ci.right.entryDist : ci.left.entryDist };
			m_DistFromWall = m_MaxDistFromWallToWallJump - entryDist;
		}
		else m_DistFromWall = m_MaxDistFromWallToWallJump + 1;
		break;
	}
}

void Madeline::SetState()
{
	Vector2i inputDir{ m_pInputManager->GetDir() };
	if (m_CanDash && m_pInputManager->IsDashing())
	{
			m_CanDash = false;
			m_Dashing = true;
			m_State = State::Dashing;
	}
	else if (m_Dashing) //continue dash
		return; 
	else if (m_CanJump && m_pInputManager->IsJumping()) //Start a specific jump
	{
		m_Jumping = true;
		m_CanJump = false;
		if (m_DistFromWall == 0.f && m_pInputManager->IsGrabbing()) //Grabbing a wall
			m_State = State::WallHopping;
		else if (m_OnGround) //Normal ground jump
			m_State = State::GroundJumping;
		else if (inputDir.x == 0) //Close to a wall and not moving left/right
			m_State = State::WallNeutralJumping;
		else //Close to a wall and moving left/right
			m_State = State::WallJumping;
	}
	else if (m_Jumping)
	{
		if (m_pInputManager->IsJumping())
			if ((m_State == State::WallJumping ||
				m_State == State::WallNeutralJumping ||
				m_State == State::WallHopping) &&
				(m_Vel.x == m_pStateInfo->x->maxSpeed ||
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
	m_Grabbing = m_DistFromWall < epsilon && m_pInputManager->IsGrabbing();

	if (!m_OnGround && !m_Grabbing)
		m_AirTime += dt;
	else
		m_AirTime = 0;

	//Can jump and not on ground or against wall or airTime > ledgeJumpTime will set m_CanJump = false or,
	//Can't jump or no jump input but on ground or against wall will set m_CanJump = true
	m_CanJump = (m_CanJump || !m_pInputManager->IsJumping()) && (m_OnGround || m_AgainstWall) || (m_CanJump && m_AirTime <= m_LedgeJumpTime);
	//m_Jumping will go from true to false if m_Jumping is true and vel <= 0 or m_OnGround
	m_Jumping &= (m_Vel.y > 0.f && !m_OnGround);
	//m_CanDash will go from false to true if no dash input and m_OnGround
	m_CanDash |= (!m_pInputManager->IsDashing() && m_OnGround);
	//m_Dashing will go from true to false if vel x and y have reached the maxSpeed
	m_Dashing &= !(m_Vel.x == m_pStateInfo->x->maxSpeed && m_Vel.y == m_pStateInfo->y->maxSpeed);
}

void Madeline::InitialiseState()
{
	int stateIdx{ int(m_State) };
	m_pStateInfo = &m_StateInfoArr[stateIdx];

	Vector2i inputDir{ m_pInputManager->GetDir() };
	ApplyMovementParameters(m_TargetVel.x, m_Vel.x, m_Acc.x, *m_pStateInfo->x, inputDir.x);
	ApplyMovementParameters(m_TargetVel.y, m_Vel.y, m_Acc.y, *m_pStateInfo->y, inputDir.y);

	//Set the MultiSpriteSheet to the correct animation
	m_pMultiSpriteSheet->SetSpriteSheetName(m_pStateInfo->animation);

	// To-Do: clean up flipping texture part
	//Flip texture if needed
	if (inputDir.x != 0)
	{
		if (m_AgainstWall)
			m_pMultiSpriteSheet->Flip(m_AgainstLeftWall);
		else
			m_pMultiSpriteSheet->Flip(false);

		if (m_State == State::Dashing)
		{
			m_pMultiSpriteSheet->Flip(inputDir.x < 0);
		}
	}
}

void Madeline::UpdateState(float dt)
{
	Vector2i dir{ m_pInputManager->GetDir() };
	if (m_pStateInfo->x->allowDirChange)
	{
		m_TargetVel.x = m_pStateInfo->x->maxSpeed * dir.x;
		bool flipX{ m_AgainstLeftWall || m_Acc.x < 0 };
		m_pMultiSpriteSheet->Flip(flipX);
	}

	if (m_pStateInfo->y->allowDirChange)
		m_TargetVel.y = m_pStateInfo->y->maxSpeed * dir.y;

	m_pMultiSpriteSheet->Update(dt);
}

void Madeline::ApplyMovementParameters(float& targetVel, float& vel, float& acc, MovementParameters& movementParameters, float inputDir)
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
