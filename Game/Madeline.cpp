#include "pch.h"
#include "Madeline.h"
#include "Level.h"
#include "Game.h"

Madeline::Madeline(Point2f pos, float width, float height, const MadelineData& data, Level* pLevel)
	: m_pLevel{ pLevel }
	, m_State{ State::Idle }
	, m_Bounds{ Rectf{pos.x, pos.y, width, height} }
	, m_MovementDir{ Vector2i{} }
	, m_Vel{ Vector2f{} }
	, m_MaxVel{ Vector2f{} }
	, m_Acc{ Vector2f{} }
	, m_AllowDirChangeX{}
	, m_AllowDirChangeY{}
	, m_AllowDirChangeDirOverride{ Vector2i{} }
	//Movement parameters
	, m_GroundJumpHeight{ data.groundJumpHeight }
	, m_GroundJumpTime{ data.groundJumpTime }
	, m_RunningSpeed{ data.runningSpeed }
	, m_WallNeutralJumpDistX{data.wallNeutralJumpDistX}
	, m_WallNeutralJumpDistXTime{data.wallNeutralJumpDistXTime}
	, m_WallJumpDistX{data.wallJumpDistX}
	, m_WallJumpDistXTime{data.wallJumpDistXTime}
	, m_MaxFallSpeed{ data.maxFallSpeed }
	, m_WallClimbingSpeed{ data.wallClimbingSpeed }
	, m_WallSlidingSpeed{ data.wallSlidingSpeed }
	, m_MaxDistFromWallToWallJump{ data.maxDistFromWallToWallJump }
	, m_DashDist{ data.dashDist }
	, m_DashDistTime{ data.dashDistTime }
	//State parameters
	, m_OnGround{}
	, m_CanJump{}
	, m_Jumping{}
	, m_AgainstWall{}
	, m_AgainstRightWall{}
	, m_AgainstLeftWall{}
	, m_DistFromWall{}
	, m_CanDash{}
	, m_Dashing{}
	//Input
	, m_HoldingJump{}
{
}

Madeline::~Madeline()
{
	m_pLevel = nullptr;
}

void Madeline::Draw() const
{
	utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
	utils::FillRect(m_Bounds);

	//Visualize tiles where collision can happen
	//DrawCollision();
}

void Madeline::Update(float dt, const Game::InputActions& input)
{
	State prevState{ m_State };
	m_HoldingJump = input.jumping;

	SetState(input);
	//std::cout << "State: " << m_StateNames[int(m_State)] << std::endl;

	if (m_State != prevState)
		SetMaxVelByState(input);

	//Update maxVel and acc according to input if allowed
	if (m_AllowDirChangeX && m_MovementDir.x != input.dir.x)
		SetMaximumVelAndAccByDir(input.dir.x, Axis::X);
	if (m_AllowDirChangeY && m_MovementDir.y != input.dir.y)
		SetMaximumVelAndAccByDir(input.dir.y, Axis::Y);

	//Move vel towards maxVel using acc
	if (m_Vel.x != m_MaxVel.x)
		UpdateVelUsingAcc(dt, Axis::X);
	if (m_Vel.y != m_MaxVel.y)
		UpdateVelUsingAcc(dt, Axis::Y);

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
	m_AgainstRightWall = m_AgainstWall && wallDetectionCI.collDir.right;
	//m_AgainstLeftWall = wallDetectionCI.collDir.left;
	if (m_AgainstWall)
	{
		if (m_AgainstRightWall)
			m_DistFromWall = m_MaxDistFromWallToWallJump - wallDetectionCI.right.entryDist;
		else
			m_DistFromWall = m_MaxDistFromWallToWallJump - wallDetectionCI.left.entryDist;
	}
	else m_DistFromWall = m_MaxDistFromWallToWallJump + 1;

	m_CanJump = !m_HoldingJump && (m_OnGround || m_AgainstWall);
	if (m_Jumping && (m_Vel.y <= 0.f || m_OnGround))
		m_Jumping = false;
	m_CanDash = !input.dashing && m_OnGround;
	if (m_Dashing && m_Vel.x == m_MaxVel.x && m_Vel.y == m_MaxVel.y)
		m_Dashing = false;
}

/*
Should remove, only for testing purposes to place madeline in the level
*/
void Madeline::SetPosition(const Point2f& pos)
{
	m_Bounds.left = pos.x;
	m_Bounds.bottom = pos.y;
}

void Madeline::SetState(const Game::InputActions& input)
{
	if (m_Dashing) //continue dash
		return; 
	else if (m_CanDash && input.dashing)
	{
		m_CanDash = false;
		m_Dashing = true;
		m_State = State::Dashing;
	}
	else if (m_Jumping)
	{
		if (m_HoldingJump)
			if ((m_State == State::WallJumping ||
				m_State == State::WallNeutralJumping ||
				m_State == State::WallHopping) &&
				(input.dir.x == m_AllowDirChangeDirOverride.x ||
					m_Vel.x == m_MaxVel.x)) //Transition to normal jump
				m_State = State::Jumping;
			else
				return; //jump is still controlled so keep current state/specific jump
		else
			m_State = State::EndingJump;
	}
	else if (m_CanJump && m_HoldingJump) //Start a specific jump
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
}

void Madeline::SetMaxVelByState(const Game::InputActions& input)
{
	switch (m_State)
	{
	case State::Idle:
	{
		float accX{ m_RunningSpeed / 0.1f };
		SetMovementParameters(Axis::X, m_Vel.x, 0.f, 0.f, accX, false);
		AccAndVel accAndVelY{ utils::AccAndVelToTravelDistInTime(m_GroundJumpHeight, m_GroundJumpTime) };
		SetMovementParameters(Axis::Y, m_Vel.y, m_MaxFallSpeed, 0.f, -accAndVelY.acc, false);
		break;
	}
	case State::Running:
	{
		float accX{ m_RunningSpeed / 0.1f };
		SetMovementParameters(Axis::X, m_Vel.x, m_RunningSpeed, 0.f, accX, true);
		AccAndVel accAndVelY{ utils::AccAndVelToTravelDistInTime(m_GroundJumpHeight, m_GroundJumpTime) };
		SetMovementParameters(Axis::Y, m_Vel.y, m_MaxFallSpeed, 0.f, -accAndVelY.acc, false);
		break;
	}
	case State::Jumping:
	{
		float accX{ m_RunningSpeed / 0.1f };
		SetMovementParameters(Axis::X, m_Vel.x, m_RunningSpeed, 0.f, accX, true);
		AccAndVel accAndVelY{ utils::AccAndVelToTravelDistInTime(m_GroundJumpHeight, m_GroundJumpTime) };
		SetMovementParameters(Axis::Y, m_Vel.y, 0.f, 0.f, -accAndVelY.acc, false);
		break;
	}
	case State::EndingJump:
	{
		float accX{ m_RunningSpeed / 0.1f };
		SetMovementParameters(Axis::X, m_Vel.x, m_RunningSpeed, 0.f, accX, true);
		AccAndVel accAndVelY{ utils::AccAndVelToTravelDistInTime(m_GroundJumpHeight, m_GroundJumpTime) };
		SetMovementParameters(Axis::Y, m_Vel.y, 0.f, 0.f, -accAndVelY.acc * 2, false);
		break;
	}
	case State::GroundJumping:
	{
		float accX{ m_RunningSpeed / 0.1f };
		SetMovementParameters(Axis::X, m_Vel.x, m_RunningSpeed, 0.f, accX, true);
		AccAndVel accAndVelY{ utils::AccAndVelToTravelDistInTime(m_GroundJumpHeight, m_GroundJumpTime) };
		SetMovementParameters(Axis::Y, accAndVelY.vel, 0.f, 0.f, -accAndVelY.acc, false);
		break;
	}
	case State::WallJumping:
	{
		int wallNormalX{ (m_AgainstRightWall) ? -1 : 1 };
		AccAndVel accAndVelX{ utils::AccAndVelToTravelDistInTime(m_WallJumpDistX, m_WallJumpDistXTime) };
		SetMovementParameters(Axis::X, wallNormalX * accAndVelX.vel, 0.f, 0.f, -wallNormalX * accAndVelX.acc, false, wallNormalX);
		AccAndVel accAndVelY{ utils::AccAndVelToTravelDistInTime(m_GroundJumpHeight, m_GroundJumpTime) };
		SetMovementParameters(Axis::Y, accAndVelY.vel, 0.f, 0.f, -accAndVelX.acc, false);
		break;
	}
	case State::WallHopping:
	{
		int wallNormalX{ (m_AgainstRightWall) ? -1 : 1 };
		float accX{ m_RunningSpeed / 0.1f };
		SetMovementParameters(Axis::X, 0.f, 0.f, 0.f, 0.f, false, wallNormalX);
		//SetMovementParameters(Axis::X, m_Vel.x, m_RunningSpeed, 0.f, accX, true);
		AccAndVel accAndVelY{ utils::AccAndVelToTravelDistInTime(m_GroundJumpHeight, m_GroundJumpTime) };
		SetMovementParameters(Axis::Y, accAndVelY.vel, 0.f, 0.f, -accAndVelY.acc, false);
		break;
	}
	case State::WallNeutralJumping:
	{
		int wallNormalX{ (m_AgainstRightWall) ? -1 : 1 };
		AccAndVel accAndVelX{ utils::AccAndVelToTravelDistInTime(m_WallNeutralJumpDistX, m_WallNeutralJumpDistXTime) };
		SetMovementParameters(Axis::X, wallNormalX * accAndVelX.vel, 0.f, 0.f, -wallNormalX * accAndVelX.acc, false, wallNormalX);
		AccAndVel accAndVelY{ utils::AccAndVelToTravelDistInTime(m_GroundJumpHeight, m_GroundJumpTime) };
		SetMovementParameters(Axis::Y, accAndVelY.vel, 0.f, 0.f, -accAndVelY.acc, false);
		break;
	}
	case State::Falling:
	{
		float accX{ m_RunningSpeed / 0.1f };
		SetMovementParameters(Axis::X, m_Vel.x, m_RunningSpeed, 0.f, accX, true);
		AccAndVel accAndVelY{ utils::AccAndVelToTravelDistInTime(m_GroundJumpHeight, m_GroundJumpTime) };
		SetMovementParameters(Axis::Y, m_Vel.y, m_MaxFallSpeed, 0.f, -accAndVelY.acc, false);
		break;
	}
	case State::Crouching:
	{
		float accX{ m_RunningSpeed / 0.1f };
		SetMovementParameters(Axis::X, m_Vel.x, 0.f, 0.f, accX, false);
		AccAndVel accAndVelY{ utils::AccAndVelToTravelDistInTime(m_GroundJumpHeight, m_GroundJumpTime) };
		SetMovementParameters(Axis::Y, m_Vel.y, m_MaxFallSpeed, 0.f, -accAndVelY.acc, false);
		break;
	}
	case State::WallGrabbing:
	{
		SetMovementParameters(Axis::X, m_Vel.x, 0.f, 0.f, 0.f, false);
		float accY{ m_MaxFallSpeed / 0.1f };
		SetMovementParameters(Axis::Y, m_Vel.y, 0.f, 0.f, accY, false);
		break;
	}
	case State::WallClimbing:
	{
		SetMovementParameters(Axis::X, m_Vel.x, 0.f, 0.f, 0.f, false);
		float accY{ m_WallClimbingSpeed / 0.1f };
		if (m_Vel.y > 0)
			accY = 0.f;
		SetMovementParameters(Axis::Y, m_Vel.y, m_WallClimbingSpeed, 0.f, accY, false);
		break;
	}
	case State::WallSliding:
	{
		SetMovementParameters(Axis::X, m_Vel.x, 0.f, 0.f, 0.f, false);
		float accY{ m_WallSlidingSpeed / 0.1f };
		SetMovementParameters(Axis::Y, m_Vel.y, m_WallSlidingSpeed, 0.f, accY, false);
		break;
	}
	case State::Dashing:
	{
		AccAndVel accAndVel{ utils::AccAndVelToTravelDistInTime(m_DashDist, m_DashDistTime) };
		SetMovementParameters(Axis::X, input.dir.x * accAndVel.vel, 0.f, 0.f, -input.dir.x * accAndVel.acc, false);
		SetMovementParameters(Axis::Y, input.dir.y * accAndVel.vel, 0.f, 0.f, -input.dir.y * accAndVel.acc, false);
		break;
	}
	}
}

void Madeline::SetMovementParameters(Axis axis, float initVel, float maxVel, float time, float accOverride, bool allowDirChange, int allowDirChangeDirOverride)
{
	float& velRef{ (axis == Axis::X) ? m_Vel.x : m_Vel.y };
	float& maxVelRef{ (axis == Axis::X) ? m_MaxVel.x : m_MaxVel.y };
	bool& allowDirChangeRef{ (axis == Axis::X) ? m_AllowDirChangeX : m_AllowDirChangeY };
	float& accRef{ (axis == Axis::X) ? m_Acc.x : m_Acc.y };
	int& movementDirRef{ (axis == Axis::X) ? m_MovementDir.x : m_MovementDir.y };
	int& allowDirChangeDirOverrideRef{ (axis == Axis::X) ? m_AllowDirChangeDirOverride.x : m_AllowDirChangeDirOverride.y };

	velRef = initVel;
	maxVelRef = maxVel;
	allowDirChangeRef = allowDirChange;
	allowDirChangeDirOverrideRef = allowDirChangeDirOverride;

	if (accOverride != 0)
		accRef = accOverride;
	else if (time != 0)
		accRef = (maxVel - initVel) / time; // a = deltaV/t
	else //No idea how to get from initVel to targetVel by given acc or time so immediatly set vel to targetVel
		velRef = maxVelRef;

	//Set direction based on acceleration
	if (accRef > 0.f)
		movementDirRef = 1;
	else if (accRef < 0.f)
		movementDirRef = -1;
	else
		movementDirRef = 0;
}

void Madeline::UpdateVelUsingAcc(float dt, Axis axis)
{
	float& velRef{ (axis == Axis::X) ? m_Vel.x : m_Vel.y };
	float& maxVelRef{ (axis == Axis::X) ? m_MaxVel.x : m_MaxVel.y };
	int& movementDirRef{ (axis == Axis::X) ? m_MovementDir.x : m_MovementDir.y };
	float& accRef{ (axis == Axis::X) ? m_Acc.x : m_Acc.y };

	velRef += accRef * dt;
	float targetVel{ movementDirRef * std::abs(maxVelRef) };
	if ((accRef > 0.f && velRef > targetVel) || (accRef < 0.f && velRef < targetVel))
		velRef = targetVel;
}

void Madeline::SetMaximumVelAndAccByDir(int inputDir, Axis axis)
{
	int& movementDirRef{ (axis == Axis::X) ? m_MovementDir.x : m_MovementDir.y };
	float& accRef{ (axis == Axis::X) ? m_Acc.x : m_Acc.y };
	float& maxVelRef{ (axis == Axis::X) ? m_MaxVel.x : m_MaxVel.y };

	movementDirRef = inputDir;

	//Point maxVel in movement direction
	if (movementDirRef > 0)
		maxVelRef = std::abs(maxVelRef);
	else if (movementDirRef < 0)
		maxVelRef = -std::abs(maxVelRef);
	else
		maxVelRef *= -1;

	//Point acceleration in direction of maxVel
	if (maxVelRef > 0.f && accRef < 0.f)
		accRef = std::abs(accRef);
	else if (maxVelRef < 0.f && accRef > 0.f)
		accRef = -std::abs(accRef);
}

Point2f Madeline::GetPosition() const
{
	return Point2f{ m_Bounds.left, m_Bounds.bottom };
}

void Madeline::DrawCollision() const
{
	//VelInfo moving{ SetVelInfo(m_Vel) };
	//TileIdx corner{ m_VelBasedCornerTiles.corner };
	//TileIdx xCorner{ m_VelBasedCornerTiles.xCorner };
	//TileIdx yCorner{ m_VelBasedCornerTiles.yCorner };

	////Collision in x can only happen when player is moving in x direction
	//if (moving.inX)
	//{
	//	int minRow{ (moving.up) ? yCorner.r : corner.r + int(moving.inY) };
	//	int maxRow{ (moving.up) ? corner.r + int(!moving.inY) : yCorner.r + 1 };
	//	//Check all tiles left or right from the player
	//	for (int row{ minRow }; row < maxRow; ++row)
	//	{
	//		TileIdx yTile{ row, corner.c };
	//		DrawCollisionTile(yTile, Color4f{ 0.f, 1.f, 0.f, 1.f });
	//	}
	//}
	////Collision in y can only happen when player is moving in y direction
	//if (moving.inY)
	//{
	//	int minCol{ (moving.right) ? xCorner.c : corner.c + int(moving.inX) };
	//	int maxCol{ (moving.right) ? corner.c + int(!moving.inX) : xCorner.c + 1 };
	//	//Check all tiles left or right from the player
	//	for (int col{ minCol }; col < maxCol; ++col)
	//	{
	//		TileIdx xTile{ corner.r, col };
	//		DrawCollisionTile(xTile, Color4f{ 1.f, 0.f, 0.f, 1.f });
	//	}
	//}

	//if (moving.inX && moving.inY)
	//{
	//	DrawCollisionTile(corner, Color4f{ 0.f, 0.f, 1.f, 1.f });
	//}
}

void Madeline::DrawCollisionTile(TileIdx tileIdx, const Color4f& color) const
{
	//Rectf tileRect{ GameData::ActiveLvl()->GetTileRect(tileIdx) };
	//int tideID{ GameData::ActiveLvl()->GetTileID(tileIdx.r, tileIdx.c) };
	//if (tideID == 1)
	//{
	//	RectCorners tileCorners{ utils::GetRectCorners(tileRect) };
	//	utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
	//	utils::DrawLine(tileCorners.leftTop, tileCorners.rightBottom, 3.f);
	//	utils::DrawLine(tileCorners.leftBottom, tileCorners.rightTop, 3.f);
	//}
	//utils::SetColor(color);
	//utils::DrawRect(tileRect, 2.f);
}
