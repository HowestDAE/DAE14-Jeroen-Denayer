#include "pch.h"
#include "Madeline.h"
#include "Level.h"
#include <algorithm>
#include <limits>
#include "GameData.h"

Madeline::Madeline(const Point2f pos, float width, float height)
	: m_State{ State::Idle }
	, m_Bounds{ Rectf{pos.x, pos.y, width, height} }
	, m_Vel{ Vector2f{} }
	, m_RunningSpeed{ 14.f }
	, m_Jump_Speed{ sqrtf(abs(2 * GameData::G() * GameData::MADELINE_JUMP_HEIGHT())) }
	, m_Max_Fall_Speed{ -10.f }
	, m_OnGround{}
	, m_AgainstWall{}
	, m_MovingRight{}
	, m_MovingUp{}
	, m_MovingInX{}
	, m_MovingInY{}
	, m_VelBasedCornerTiles{ VelBasedCornerTiles{} }
{
}

void Madeline::Draw() const
{
	utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
	utils::FillRect(GetBounds());

	//Visualize tiles where collision can happen
	//DrawCollision(gameInfo);
}

void Madeline::Update(float dt, std::vector<Action> actions)
{
	Rectf prevBounds{ m_Bounds };
	std::cout << "CanJump: " << m_CanJump << " Jumping: " << m_Jumping << std::endl;
	if (ContainsAction(actions, Action::MovingLeft) && !ContainsAction(actions, Action::MovingRight))
		m_Vel.x = -m_RunningSpeed;
	if (ContainsAction(actions, Action::MovingRight) && !ContainsAction(actions, Action::MovingLeft))
		m_Vel.x = m_RunningSpeed;
	if (m_CanJump || m_Jumping)
		UpdateJumpVel(dt, ContainsAction(actions, Action::Jumping));

	float velDistX{ m_Vel.x * GameData::PIX_PER_M() * dt };
	m_Vel.y += GameData::G() * dt;
	//if (m_Vel.y < m_Max_Fall_Speed)
	//	m_Vel.y = m_Max_Fall_Speed;
	float velDistY{ m_Vel.y * dt * GameData::PIX_PER_M() };
	m_Bounds.left += velDistX;
	m_Bounds.bottom += velDistY;

	m_MovingRight = velDistX > 0.f;
	m_MovingUp = velDistY > 0.f;
	m_MovingInX = velDistX != 0.f;
	m_MovingInY = velDistY != 0.f;

	SetVelBasedCornerTiles();

	//No collision or state changes can possibly happen
	//TO-Do check how this works when G is enabled cause velDistY will never be 0.f except during perhaps dashes??
	//if (velDistX == 0.f && velDistY == 0.f)
	//	return;

	bool xCollision{}, yCollision{}, cornerCollision{};
	float collX{ float((m_VelBasedCornerTiles.cornerTile.c + int(!m_MovingRight)) * GameData::TILE_SIZE_PIX_SCALED()) };
	float collY{ float((m_VelBasedCornerTiles.cornerTile.r + int(!m_MovingUp)) * GameData::TILE_SIZE_PIX_SCALED()) };
	CheckCollisionDirection(xCollision, yCollision, cornerCollision);

	//std::cout << "X: " << xCollision << ", Y: " << yCollision << ", Corner: " << cornerCollision << std::endl;
	if (cornerCollision && !xCollision && !yCollision) //Convex corner collision
	{
		float normalX{}, normalY{};
		float collisionTime{ GetCollisionInfo(prevBounds, Vector2f{ velDistX, velDistY }, collX, collY, normalX, normalY) };
		if (normalX != 0.f)
		{
			m_Bounds.left = collX - m_Bounds.width * int(m_MovingRight);
			yCollision = false;
		}
		if (normalY != 0.f)
		{
			m_Bounds.bottom = collY - m_Bounds.height * int(m_MovingUp);
			xCollision = false;
		}
	}
	else if (xCollision && !yCollision) //Wall collision
		m_Bounds.left = collX - m_Bounds.width * int(m_MovingRight);
	else if (yCollision && !xCollision) //Ground / ceiling collision
		m_Bounds.bottom = collY - m_Bounds.height * int(m_MovingUp);
	else if (xCollision && yCollision) //Concave corner collision
	{
		m_Bounds.left = collX - m_Bounds.width * int(m_MovingRight);
		m_Bounds.bottom = collY - m_Bounds.height * int(m_MovingUp);
	}

	//Set state
	m_OnGround = !m_MovingUp && yCollision;
	m_AgainstWall = xCollision;
	if (m_OnGround && !m_MovingInX) m_State = State::Idle;
	else if (m_OnGround && m_MovingInX) m_State = State::Running;
	else if (!m_OnGround && m_AgainstWall && m_MovingInX) m_State = State::Hanging;
	else if (!m_OnGround && m_MovingUp) m_State = State::Jumping;
	else if (!m_OnGround && !m_MovingUp) m_State = State::Falling;

	m_Vel.x = 0.f;
	if (yCollision)
		m_Vel.y = 0.f;

	if (m_OnGround)
	{
		ResetJump();
	}
	//m_Vel.y = 0.f; //Remove when enabling gravity again
}

void Madeline::UpdateJumpVel(float dt, bool jumpKeyPressed)
{
	if (jumpKeyPressed)
	{
		if (!m_Jumping)
		{
			m_Vel.y = m_Jump_Speed;
			m_CanJump = false;
			m_Jumping = true;
		}
		m_PercentageInJump += dt / GameData::MADELINE_JUMP_TIME();
		if (m_PercentageInJump >= 1.f) //reached max jump height
			m_Jumping = false;
	}
	else if (m_Jumping)
	{
		if (m_Vel.y > 0.f)
			m_Vel.y *= 0.75f; //Decrease vel over time to slow down
		else
			m_Jumping = false;
	}
}

void Madeline::ResetJump()
{
	m_PercentageInJump = 0.f;
	m_Jumping = false;
	m_CanJump = true;
}

void Madeline::CheckCollisionDirection(bool& xCollision, bool& yCollision, bool& cornerCollision) const
{
	TileIdx cornerTile{ m_VelBasedCornerTiles.cornerTile };
	TileIdx xCornerTile{ m_VelBasedCornerTiles.xCornerTile };
	TileIdx yCornerTile{ m_VelBasedCornerTiles.yCornerTile };

	if (m_MovingInX && m_MovingInY)
	{
		int tideID{ GameData::ActiveLvl()->GetTileID(cornerTile.r, cornerTile.c) };
		if (tideID == 1)
			cornerCollision = true;
	}

	//Collision in x can only happen when player is moving in x direction
	if (m_MovingInX)
	{
		int minRow{ (m_MovingUp) ? yCornerTile.r : cornerTile.r + int(m_MovingInY) };
		int maxRow{ (m_MovingUp) ? cornerTile.r + int(!m_MovingInY) : yCornerTile.r + 1 };
		//Check all tiles left or right from the player
		for (int row{ minRow }; row < maxRow; ++row)
		{
			int tideID{ GameData::ActiveLvl()->GetTileID(row, cornerTile.c) };
			if (tideID != 1) continue;
			xCollision = true;
			break;
		}
	}
	//Collision in y can only happen when player is moving in y direction
	if (m_MovingInY)
	{
		int minCol{ (m_MovingRight) ? xCornerTile.c : cornerTile.c + int(m_MovingInX) };
		int maxCol{ (m_MovingRight) ? cornerTile.c + int(!m_MovingInX) : xCornerTile.c + 1 };
		//Check all tiles left or right from the player
		for (int col{ minCol }; col < maxCol; ++col)
		{
			int tideID{ GameData::ActiveLvl()->GetTileID(cornerTile.r, col) };
			if (tideID != 1) continue;
			yCollision = true;
			break;
		}
	}
}

float Madeline::GetCollisionInfo(const Rectf& b1, const Vector2f vel, const float collX, const float collY, float& normalX, float& normalY) const
{
	float xPos{ (m_MovingRight) ? b1.left + b1.width : b1.left };
	float yPos{ (m_MovingUp) ? b1.bottom + b1.height : b1.bottom };
	//Check if b1 is flush with b2 in x or y
	//Equality check only return true when x or y was reset to collX or collY prev frame
	bool xAlreadyColliding{ xPos == collX };
	bool yAlreadyColliding{ yPos == collY };

	//Find the distance between the objects on the near side for both x and y
	float entryDistX{ std::abs(xPos - collX) };
	float entryDistY{ std::abs(yPos - collY) };

	float velPercentageX{ -std::numeric_limits<float>::infinity() };
	float velPercentageY{ -std::numeric_limits<float>::infinity() };
	//Find time of collision for each axis, if statement prevents divide by zero
	if (!yAlreadyColliding && m_MovingInX)
		velPercentageX = entryDistX / std::abs(vel.x);
	if (!xAlreadyColliding && m_MovingInY)
		velPercentageY = entryDistY / std::abs(vel.y);

	//A small floating point error can occur when dividing entryDist / absVel when 
	//the 2 boxes are perfectly against eachother in x or y
	float epsilon{ 0.001f };
	if (velPercentageX > 1.f && velPercentageX < 1.f + epsilon)
		velPercentageX = 1.f;
	if (velPercentageY > 1.f && velPercentageY < 1.f + epsilon)
		velPercentageY = 1.f;

	//If both % are -inf then no collision happenend
	if (velPercentageX < 0.f && velPercentageY < 0.f) return 1.f;
	//Shouldn't happen !!! Dist entered in x or y should always be lower then velX or velY
	else if (velPercentageX > 1.f && velPercentageY > 1.f)
	{
		std::cout << "Unhandled collision" << std::endl;
	}

	bool xCollision{}, yCollision{};
	//Find the earliest times of collision
	float entryTime{ 1.f };
	if (velPercentageX <= 1.f && (velPercentageY < 0.f || velPercentageY > 1.f))
	{
		entryTime = velPercentageX;
		xCollision = true;
	}
	else if (velPercentageY <= 1.f && (velPercentageX < 0.f || velPercentageX > 1.f))
	{
		entryTime = velPercentageY;
		yCollision = true;
	}
	else if (velPercentageY <= velPercentageX)
	{
		entryTime = velPercentageY;
		yCollision = true;
	}
	else
	{
		entryTime = velPercentageX;
		xCollision = true;
	}

	// calculate normal of collided surface
	if (xCollision)
		normalX = (vel.x > 0.f) ? -1.f : 1.f;
	else
		normalY = (vel.y > 0.f) ? -1.f : 1.f;

	return entryTime;
}

Rectf Madeline::GetBounds() const
{
	return m_Bounds;
}

void Madeline::SetVelBasedCornerTiles()
{
	RectCornerTileIndices corners{ GameData::ActiveLvl()->GetRectCornerTileIndices(m_Bounds) };

	//Select 3/4 corner tiles based on movement/velocity direction
	TileIdx cornerTile{
		(m_MovingUp) ? corners.leftTop.r : corners.leftBottom.r,
		(m_MovingRight) ? corners.rightTop.c : corners.leftTop.c,
	};
	TileIdx xCornerTile{ //The corner tile left or right from the cornerTile
		cornerTile.r,
		(m_MovingRight) ? corners.leftTop.c : corners.rightTop.c
	};
	TileIdx yCornerTile{ //The corner tile up or down from the cornerTile
		(m_MovingUp) ? corners.leftBottom.r : corners.leftTop.r,
		cornerTile.c
	};
	m_VelBasedCornerTiles.cornerTile = cornerTile;
	m_VelBasedCornerTiles.xCornerTile = xCornerTile;
	m_VelBasedCornerTiles.yCornerTile = yCornerTile;
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
	TileIdx cornerTile{ m_VelBasedCornerTiles.cornerTile };
	TileIdx xCornerTile{ m_VelBasedCornerTiles.xCornerTile };
	TileIdx yCornerTile{ m_VelBasedCornerTiles.yCornerTile };

	if (m_MovingInX && m_MovingInY)
	{
		DrawCollisionTile(cornerTile, Color4f{ 0.f, 0.f, 1.f, 1.f });
	}

	//Collision in x can only happen when player is moving in x direction
	if (m_MovingInX)
	{
		int minRow{ (m_MovingUp) ? yCornerTile.r : cornerTile.r + int(m_MovingInY) };
		int maxRow{ (m_MovingUp) ? cornerTile.r + int(!m_MovingInY) : yCornerTile.r + 1 };
		//Check all tiles left or right from the player
		for (int row{ minRow }; row < maxRow; ++row)
		{
			TileIdx yTile{ row, cornerTile.c };
			DrawCollisionTile(yTile, Color4f{ 0.f, 1.f, 0.f, 1.f });
		}
	}
	//Collision in y can only happen when player is moving in y direction
	if (m_MovingInY)
	{
		int minCol{ (m_MovingRight) ? xCornerTile.c : cornerTile.c + int(m_MovingInX) };
		int maxCol{ (m_MovingRight) ? cornerTile.c + int(!m_MovingInX) : xCornerTile.c + 1 };
		//Check all tiles left or right from the player
		for (int col{ minCol }; col < maxCol; ++col)
		{
			TileIdx xTile{ cornerTile.r, col };
			DrawCollisionTile(xTile, Color4f{ 1.f, 0.f, 0.f, 1.f });
		}
	}
}

void Madeline::DrawCollisionTile(TileIdx tileIdx, const Color4f& color) const
{
	Rectf tileRect{ GameData::ActiveLvl()->GetTileRect(tileIdx) };
	utils::SetColor(color);
	utils::DrawRect(tileRect, 2.f);
	int tideID{ GameData::ActiveLvl()->GetTileID(tileIdx.r, tileIdx.c) };
	if (tideID == 1)
	{
		RectCorners tileCorners{ utils::GetRectCorners(tileRect) };
		utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
		utils::DrawLine(tileCorners.leftTop, tileCorners.rightBottom, 3.f);
		utils::DrawLine(tileCorners.leftBottom, tileCorners.rightTop, 3.f);
	}
}