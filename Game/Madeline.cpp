#include "pch.h"
#include "Madeline.h"
#include "Level.h"
#include <algorithm>
#include <limits>
#include "GameData.h"

Madeline::Madeline(int spawnRow, int spawnCol, float width, float height, const MadelineData& data)
	: m_State{ State::Idle }
	, m_Bounds{ Rectf{float(spawnCol * GameData::TILE_SIZE_PIX()), float(spawnRow * GameData::TILE_SIZE_PIX()), width, height} }
	, m_MovementDir{ Vector2i{} }
	, m_Vel{ Vector2f{} }
	, m_MaxVel{ Vector2f{} }
	, m_Acc{ Vector2f{} }
	, m_AllowDirChangeX{}
	, m_AllowDirChangeY{}
	, m_RMCAX{ RMCAInfo{true} }
	, m_RMCAY{ RMCAInfo{true} }
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
	//State parameters
	, m_OnGround{}
	, m_CanJump{}
	, m_Jumping{}
	, m_AgainstWall{}
	, m_AgainstRightWall{}
	, m_AgainstLeftWall{}
	, m_DistFromWall{}
	//Input
	, m_HoldingJump{}
	, m_HoldingGrab{}
{
}

void Madeline::Draw() const
{
	utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
	utils::FillRect(m_Bounds);

	//Visualize tiles where collision can happen
	//DrawCollision();
}

void Madeline::Update(float dt, Vector2i inputDir, const std::vector<Action>& actions)
{
	//Check input actions
	//m_MovementDir = inputDir;

	State prevState{ m_State };
	m_HoldingJump = ContainsAction(actions, Action::Jumping);
	m_HoldingGrab = ContainsAction(actions, Action::Grabbing);

	SetState(inputDir, actions);
	std::cout << "State: " << m_StateNames[int(m_State)] << std::endl;

	if (m_State != prevState)
		SetTargetVelByState();

	//If the dir changed always flip acceleration
	//Example: dir goes from -1 to 0 or 1, in both cases acc needs to be flipped
	if (m_AllowDirChangeX && m_MovementDir.x != inputDir.x)
		SetMaximumVelAndAccByDir(inputDir.x, Axis::X);
	if (m_AllowDirChangeY && m_MovementDir.y != inputDir.y)
		SetMaximumVelAndAccByDir(inputDir.y, Axis::Y);

	//std::cout << m_MaxVel.x << " ," << m_Acc.x << std::endl;
	/* Update the vel, this can happen in 2 ways
	1) Going through a fixed predefined motion using an RMCA
	2) Accelerate towards the targetVel */
	if (!m_RMCAX.completed)
		UpdateVelUsingRMCA(dt, true, m_Vel.x, m_RMCAX);
	else if (m_Vel.x != m_MaxVel.x)
		UpdateVelUsingAcc(dt, Axis::X);

	if (!m_RMCAY.completed)
		UpdateVelUsingRMCA(dt, m_HoldingJump, m_Vel.y, m_RMCAY);
	else if (m_Vel.y != m_MaxVel.y)
		UpdateVelUsingAcc(dt, Axis::Y);

	float velDistX{ m_Vel.x * GameData::PIX_PER_M() * dt };
	float velDistY{ m_Vel.y * GameData::PIX_PER_M() * dt };
	Vector2f velDist{ velDistX, velDistY };

	/* TO-DO: collision detection only checks the tiles in front of the player,
	if deltaTime/velDist is really big multiple collision tiles could be skipped over
	Can test this by holding and dragging the window a bit, no update will happen and dt will
	get really big */
	Level::RectPhysicsInfo collisionRFI{ m_Bounds, m_Vel, velDist };
	Level::CollisionInfo ci{ GameData::ActiveLvl()->MovePhysicsRect(collisionRFI) };

	//Wall detection with wider bounds to detect wall collision within a certain distance
	Rectf wallDetectionBounds{ m_Bounds.left - m_MaxDistFromWallToWallJump, m_Bounds.bottom, m_Bounds.width + 2 * m_MaxDistFromWallToWallJump, m_Bounds.height };
	const Level::RectPhysicsInfo wallDetectionRFI{ wallDetectionBounds, m_Vel, velDist};
	Level::CollisionInfo wallDetectionCI{ GameData::ActiveLvl()->DetectCollision(wallDetectionRFI, false, true, false, false) };

	//Update state bools
	m_OnGround = m_Vel.y <= 0.f && ci.collDir.down;
	//m_AgainstLeftWall = wallDetectionCI.collDir.left;
	m_AgainstWall = wallDetectionCI.collDir.x;
	m_AgainstRightWall = m_AgainstWall && wallDetectionCI.collDir.right;
	if (m_AgainstWall)
	{
		if (m_AgainstRightWall)
			m_DistFromWall = m_MaxDistFromWallToWallJump - wallDetectionCI.entryDistRight;
		else
			m_DistFromWall = m_MaxDistFromWallToWallJump - wallDetectionCI.entryDistLeft;
	}
	else m_DistFromWall = m_MaxDistFromWallToWallJump + 1;

	m_CanJump = !m_HoldingJump && (m_OnGround || m_AgainstWall);
	if (m_Jumping && (m_Vel.y <= 0.f || m_OnGround))
		m_Jumping = false;
}

/*
Should remove, only for testing purposes to place madeline in the level
*/
void Madeline::SetPosition(const Point2f& pos)
{
	m_Bounds.left = pos.x;
	m_Bounds.bottom = pos.y;
}

void Madeline::SetState(const Vector2i& inputDir, const std::vector<Action>& actions)
{
	if (m_CanJump && m_HoldingJump) //Start a specific jump
	{
		ActivateJump();
		if (m_DistFromWall == 0.f && m_HoldingGrab) //Grabbing a wall
			m_State = State::WallHopping;
		else if (m_OnGround) //Normal ground jump
			m_State = State::GroundJumping;
		else if (inputDir.x == 0) //Close to a wall and not moving left/right
			m_State = State::WallNeutralJumping;
		else //Close to a wall and moving left/right
			m_State = State::WallJumping;
	}
	else if (m_Jumping) //Still jumping
	{
		if (m_RMCAY.completed) //Jump is not controlled anymore
			m_State = State::EndingJump;
		else //jump is still controlled so keep current state/specific jump
			return;
	}
	else if (m_DistFromWall == 0.f && m_HoldingGrab) //Against the wall and grabbing
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

void Madeline::SetTargetVelByState()
{
	switch (m_State)
	{
	case State::Idle:
		SetMaximumVel(0.f, false, Axis::X);
		SetMaximumVel(m_MaxFallSpeed, false, Axis::Y, false, 0.f, GameData::G());
		break;
	case State::Running:
		SetMaximumVel(m_RunningSpeed, true, Axis::X, false, 0.1f);
		SetMaximumVel(m_MaxFallSpeed, false, Axis::Y, false, 0.f, GameData::G());
		break;
	case State::EndingJump:
		SetMaximumVel(m_RunningSpeed, true, Axis::X, false, 0.1f);
		SetMaximumVel(0.f, false, Axis::Y, false, 0.f, GameData::G() * 2);
		break;
	case State::GroundJumping:
		SetMaximumVel(m_RunningSpeed, true, Axis::X, false, 0.1f);
		SetRMCAParameters(m_GroundJumpHeight, m_GroundJumpTime, 1, m_RMCAY);
		break;
	case State::WallJumping:
	{
		float wallNormalX{ (m_AgainstRightWall) ? -1.f : 1.f };
		SetRMCAParameters(m_WallJumpDistX, m_WallJumpDistXTime, wallNormalX, m_RMCAX);
		SetRMCAParameters(m_GroundJumpHeight, m_GroundJumpTime, 1, m_RMCAY);
		break;
	}
	case State::WallHopping:
		SetMaximumVel(m_RunningSpeed, true, Axis::X, false, 0.1f);
		SetRMCAParameters(m_GroundJumpHeight, m_GroundJumpTime, 1.f, m_RMCAY);
		break;
	case State::WallNeutralJumping:
	{
		float wallNormalX{ (m_AgainstRightWall) ? -1.f : 1.f };
		SetRMCAParameters(m_WallNeutralJumpDistX, m_WallNeutralJumpDistXTime, wallNormalX, m_RMCAX);
		SetRMCAParameters(m_GroundJumpHeight, m_GroundJumpTime, 1, m_RMCAY);
		break;
	}
	case State::Falling:
		SetMaximumVel(m_RunningSpeed, true, Axis::X, false, 0.1f);
		SetMaximumVel(m_MaxFallSpeed, false, Axis::Y, false, 0.f, GameData::G());
		break;
	case State::Crouching:
		SetMaximumVel(0.f, false, Axis::X);
		SetMaximumVel(m_MaxFallSpeed, false, Axis::Y, false, 0.f, GameData::G());
		break;
	case State::WallGrabbing:
		SetMaximumVel(0.f, false, Axis::X);
		SetMaximumVel(0.f, false, Axis::Y);
		break;
	case State::WallClimbing:
		SetMaximumVel(0.f, false, Axis::X);
		SetMaximumVel(m_WallClimbingSpeed, false, Axis::Y, false, 0.1f, 0.f);
		break;
	case State::WallSliding:
		SetMaximumVel(0.f, false, Axis::X);
		SetMaximumVel(m_WallSlidingSpeed, false, Axis::Y, false, 0.1f, 0.f);
		break;
	}
}

void Madeline::SetMaximumVel(float maxVel, bool allowDirChange, Axis axis, bool setVelImmediatly, float time, float accOverride)
{
	float& maxVelRef{ (axis == Axis::X) ? m_MaxVel.x : m_MaxVel.y };
	bool& allowDirChangeRef{ (axis == Axis::X) ? m_AllowDirChangeX : m_AllowDirChangeY };
	int& movementDirRef{ (axis == Axis::X) ? m_MovementDir.x : m_MovementDir.y };
	float& velRef{ (axis == Axis::X) ? m_Vel.x : m_Vel.y };
	float& accRef{ (axis == Axis::X) ? m_Acc.x : m_Acc.y };

	maxVelRef = maxVel;
	if (setVelImmediatly)
		velRef = maxVelRef;

	allowDirChangeRef = allowDirChange;
	//Set direction based on velocity and accOverride
	if (maxVelRef > 0.f)
		movementDirRef = 1;
	else if (maxVelRef < 0.f)
		movementDirRef = -1;
	else if (accOverride == 0.f)
		movementDirRef = 0;
	else if (accOverride > 0.f)
		movementDirRef = 1;
	else //accOverride < 0.f
		movementDirRef = -1;

	//Set the acceleration needed to reach maxVel in time
	if (maxVelRef == 0.f && accOverride == 0.f && time == 0.f)
	{
		velRef = maxVelRef;
		accRef = 0.f;
	}
	else if (accOverride != 0.f)
		accRef = movementDirRef * std::abs(accOverride);
	else if (time != 0.f)
		accRef = maxVelRef / time; // a = v/t
}

void Madeline::SetRMCAParameters(float dist, float time, int dir, RMCAInfo& RMCAInfo)
{
	AccAndVel accAndVelX{ utils::AccAndVelToTravelDistInTime(dist, time) };
	RMCAInfo.completed = false;
	RMCAInfo.initVel = dir * accAndVelX.vel;
	RMCAInfo.deceleration = -dir * accAndVelX.acc;
	RMCAInfo.time = time;
	RMCAInfo.percentage = 0.f;
}

void Madeline::UpdateVelUsingAcc(float dt, Axis axis)
{
	float& velRef{ (axis == Axis::X) ? m_Vel.x : m_Vel.y };
	float& maxVelRef{ (axis == Axis::X) ? m_MaxVel.x : m_MaxVel.y };
	int& movementDirRef{ (axis == Axis::X) ? m_MovementDir.x : m_MovementDir.y };
	float& accRef{ (axis == Axis::X) ? m_Acc.x : m_Acc.y };

	velRef += accRef * dt;
	if (movementDirRef == 0 && ((accRef > 0 && velRef > 0.f) || (accRef < 0 && velRef < 0.f)))
		velRef = 0.f;
	else if ((movementDirRef > 0 && velRef > maxVelRef) || (movementDirRef < 0 && velRef < maxVelRef))
		velRef = maxVelRef;
}

void Madeline::UpdateVelUsingRMCA(float dt, bool condition, float& vel, RMCAInfo& RMCAInfo)
{
	//Set initial velocity at start of RMCA
	if (RMCAInfo.percentage == 0.f)
		vel = RMCAInfo.initVel;

	float prevPercentage{ RMCAInfo.percentage };
	RMCAInfo.percentage += dt / RMCAInfo.time;
	if (RMCAInfo.percentage > 1.f)
		RMCAInfo.percentage = 1.f;
	float deltaPercentage{ RMCAInfo.percentage - prevPercentage };
	vel += RMCAInfo.deceleration * (deltaPercentage * RMCAInfo.time);

	if (!condition || RMCAInfo.percentage == 1.f) //completed RMCA movement
		RMCAInfo.completed = true;
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

void Madeline::ActivateJump()
{
	m_Jumping = true;
	m_CanJump = false;
}

bool Madeline::ContainsAction(std::vector<Action> actions, Action action)
{
	for (Action a : actions)
		if (a == action)
			return true;
	return false;
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
