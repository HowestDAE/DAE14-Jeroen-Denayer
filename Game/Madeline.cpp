#include "pch.h"
#include "Madeline.h"
#include "Level.h"
#include "Game.h"
#include "MultiSpriteSheet.h"

Madeline::Madeline(Point2f pos, float width, float height, Level* pLevel)
	: m_pMultiSpriteSheet{ nullptr }
	, m_pLevel{ pLevel }
	, m_State{ State::Idle }
	, m_pStateInfo{ nullptr }
	, m_Bounds{ Rectf{pos.x, pos.y, width, height} }
	, m_Vel{ Vector2f{} }
	, m_Acc{ Vector2f{} }
	, m_AllowDirChange{}
	//State parameters
	, m_OnGround{}
	, m_CanJump{}
	, m_Jumping{}
	, m_AgainstWall{}
	, m_AgainstRightWall{}
	, m_AgainstLeftWall{}
	, m_DistFromWall{}
	, m_MaxDistFromWallToWallJump{ 2.f }
	, m_CanDash{}
	, m_Dashing{}
{
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
		{"Stationary", {false, 0.f, 0.f, 0.f, true, false, false}},
		{"Running", {true, 0.f, runningSpeed, runningAcc, true, false, false}},
		{"Gravity", {true, 0.f, maxFallSpeed, gravity, false, false, false}},
		{"DoubleGraivty", {true, 0.f, maxFallSpeed, gravity * 2, false, false, false}},
		{"GroundJumpingY", {false, groundJumpAccVel.vel, maxFallSpeed, gravity, false, false, false}},
		{"WallJumpingX", {false, wallJumpAccVel.vel, 0.f, wallJumpAccVel.acc, false, true, false}},
		{"WallJumpingY", {false, wallJumpAccVel.vel, 0.f, wallJumpAccVel.acc, false, false, false}},
		{"WallHoppingY", {false, groundJumpAccVel.vel, 0.f, gravity, false, false, false}},
		{"NeutralJumpingX", {false, wallNeutralJumpAccVelX.vel, 0.f, wallNeutralJumpAccVelX.acc, false, true, false}},
		{"NeutralJumpingY", {false, groundJumpAccVel.vel, 0.f, groundJumpAccVel.acc, false, false, false}},
		{"WallGrabbingY", {true, 0.f, 0.f, wallGrabbingAcc, false, false, false}},
		{"WallClimbingY", {false, 0.f, wallClimbingSpeed, wallClimbingAcc, false, false, false}},
		{"WallSlidingY", {true, 0.f, wallSlidingSpeed, wallSlidingAcc, false, false, false}},
		{"Dashing", {false, dashAccVel.vel, 0.f, dashAccVel.acc, false, false, true}}
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

	m_pMultiSpriteSheet = new MultiSpriteSheet("Textures/MadelineSpritesheet.png", 14, 14,
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
	);
	if (!m_pMultiSpriteSheet)
	{
		std::cout << "Failed to load madeline texture" << std::endl;
	}
}

Madeline::~Madeline()
{
	m_pLevel = nullptr;
	delete m_pMultiSpriteSheet;
	m_pMultiSpriteSheet = nullptr;
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
}

void Madeline::Update(float dt, const Game::InputActions& input)
{
	SetState(input);
	//std::cout << "State: " << m_pStateInfo->name << std::endl;
	
	UpdateVel(dt, m_Vel.x, m_Acc.x, input.dir.x, *m_pStateInfo->x);
	UpdateVel(dt, m_Vel.y, m_Acc.y, input.dir.y, *m_pStateInfo->y);
	UpdateMultiSpriteSheet(dt);

	/* TO-DO: collision detection only checks the tiles in front of the player,
	if deltaTime/velDist is really big multiple collision tiles could be skipped over
	Can test this by holding and dragging the window a bit, no update will happen and dt will
	get really big */
	CollisionInfo ci{ m_pLevel->MovePhysicsRect(m_Bounds, m_Vel, dt) };

	//Wall detection with wider bounds to detect wall collision within a certain distance
	Rectf wallDetectionBounds{ m_Bounds.left - m_MaxDistFromWallToWallJump, m_Bounds.bottom + m_Bounds.height / 3, m_Bounds.width + 2 * m_MaxDistFromWallToWallJump, m_Bounds.height / 3 };
	CollisionInfo wallDetectionCI{ m_pLevel->DetectRectCollision(wallDetectionBounds, true, false) };

	//Update state bools
	m_OnGround = m_Vel.y <= 0.f && ci.collDir.down;
	m_AgainstWall = wallDetectionCI.collDir.x;
	m_AgainstRightWall = wallDetectionCI.collDir.right;
	m_AgainstLeftWall = wallDetectionCI.collDir.left;
	if (m_AgainstWall)
	{
		if (m_AgainstRightWall)
			m_DistFromWall = m_MaxDistFromWallToWallJump - wallDetectionCI.right.entryDist;
		else
			m_DistFromWall = m_MaxDistFromWallToWallJump - wallDetectionCI.left.entryDist;
	}
	else m_DistFromWall = m_MaxDistFromWallToWallJump + 1;

	m_CanJump = !input.jumping && (m_OnGround || m_AgainstWall);
	if (m_Jumping && (m_Vel.y <= 0.f || m_OnGround))
		m_Jumping = false;
	if (!m_CanDash && !input.dashing && m_OnGround)
		m_CanDash = true;
	if (m_Dashing && m_Vel.x == m_pStateInfo->x->maxSpeed && m_Vel.y == m_pStateInfo->y->maxSpeed)
		m_Dashing = false;
}

void Madeline::SetState(const Game::InputActions& input)
{
	State prevState{ m_State };
	if (m_CanDash && input.dashing)
	{
			m_CanDash = false;
			m_Dashing = true;
			m_State = State::Dashing;
	}
	else if (m_Dashing) //continue dash
		return; 
	else if (m_CanJump && input.jumping) //Start a specific jump
	{
		m_Jumping = true;
		m_CanJump = false;
		if (m_DistFromWall == 0.f && input.grabbing) //Grabbing a wall
			m_State = State::WallHopping;
		else if (m_OnGround) //Normal ground jump
			m_State = State::GroundJumping;
		else if (input.dir.x == 0) //Close to a wall and not moving left/right
			m_State = State::WallNeutralJumping;
		else //Close to a wall and moving left/right
			m_State = State::WallJumping;
	}
	else if (m_Jumping)
	{
		if (input.jumping)
			if ((m_State == State::WallJumping ||
				m_State == State::WallNeutralJumping ||
				m_State == State::WallHopping) &&
				(m_Vel.x == m_pStateInfo->x->maxSpeed ||
				m_Vel.x > 0 && input.dir.x > 0 || m_Vel.x < 0 && input.dir.x < 0)) //Transition to normal jump
				m_State = State::Jumping;
			else
				return; //jump is still controlled so keep current state/specific jump
		else
			m_State = State::EndingJump;
	}
	else if (m_DistFromWall == 0.f && input.grabbing) //Against the wall and grabbing
	{
		if (input.dir.y == 0) //Not moving up/down
			m_State = State::WallGrabbing;
		else if (input.dir.y > 0)//Climbing up a wall
			m_State = State::WallClimbing;
		else //Sliding down a wall
			m_State = State::WallSliding;
	}
	else if (m_OnGround && input.dir.x == 0 && input.dir.y < 0)
		m_State = State::Crouching;
	else if (m_OnGround && input.dir.x != 0)
		m_State = State::Running;
	else if (!m_OnGround && !m_Jumping)
		m_State = State::Falling;
	else
		m_State = State::Idle;


	if (m_State == prevState)
		return;

	//To-Do: make section below cleaner/maybe wrap into separate function

	//Update State info
	int stateIdx{ int(m_State) };
	m_pStateInfo = &m_StateInfoArr[stateIdx];

	//Apply boolean modifiers
	if (!m_pStateInfo->x->useCurVelAsInitial)
		m_Vel.x = m_pStateInfo->x->initVel;
	int wallDir{ (m_AgainstRightWall) ? -1 : 1 };
	if (m_pStateInfo->x->multiplyInitVelByWallDir)
		m_Vel.x *= wallDir;
	if (m_pStateInfo->x->multiplyInitVelByInputDir)
		m_Vel.x *= input.dir.x;

	if (!m_pStateInfo->y->useCurVelAsInitial)
		m_Vel.y = m_pStateInfo->y->initVel;
	if (m_pStateInfo->y->multiplyInitVelByInputDir)
		m_Vel.y *= input.dir.y;

	//Set the MultiSpriteSheet to the correct animation
	m_pMultiSpriteSheet->SetSpriteSheetName(m_pStateInfo->animation);
	//Flip texture if needed
	if (input.dir.x != 0)
	{
		if (m_AgainstWall)
			m_pMultiSpriteSheet->Flip(m_AgainstLeftWall);
		else
			m_pMultiSpriteSheet->Flip(false);

		if (m_State == State::Dashing)
		{
			m_pMultiSpriteSheet->Flip(input.dir.x < 0);
		}
	}
}

void Madeline::UpdateVel(float dt, float& vel, float& acc, float dir, MovementParameters& movementInfo)
{
	float targetVel{ movementInfo.maxSpeed };
	if (movementInfo.allowDirChange)
		targetVel *= dir;
	if (vel != targetVel)
	{
		//Point acc in direction of targetVel
		if (targetVel > 0.f || (targetVel == 0.f && vel < 0.f))
			acc = std::abs(movementInfo.acc);
		else // if (targetVel < 0.f || (targetVel == 0.f && vel > 0.f))
			acc = -std::abs(movementInfo.acc);

		vel += acc * dt;
		if ((acc > 0.f && vel > targetVel) || (acc < 0.f && vel < targetVel))
			vel = targetVel;
	}
}

void Madeline::UpdateMultiSpriteSheet(float dt)
{
	if (m_pStateInfo->x->allowDirChange)
	{
		bool flipX{ false };
		if (m_AgainstLeftWall)
			flipX = true;
		if (m_Acc.x < 0)
			flipX = true;
		m_pMultiSpriteSheet->Flip(flipX);
	}
	m_pMultiSpriteSheet->Update(dt);
}

Point2f Madeline::GetPosition() const
{
	return Point2f{ m_Bounds.left, m_Bounds.bottom };
}
