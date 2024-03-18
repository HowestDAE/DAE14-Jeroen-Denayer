#include "pch.h"
#include "Madeline.h"
#include "Level.h"
#include <algorithm>

Madeline::Madeline(const Point2f pos, float width, float height)
	: m_Bounds{ Rectf{pos.x, pos.y, width, height} }
	, m_Speed{ 10.f }
	, m_State{ State::Idle }
	, m_JUMP_ADDED_VEL{ 30.f }
	, m_Test{ false }
{
}

void Madeline::Draw(const Game::GameInfo& gameInfo) const
{
	utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
	utils::FillRect(GetBounds());

	//Visualize tiles where collision can happen
	for (const TileIdx& tileIdx : m_CollisionTiles)
	{
		Rectf tileRect{ gameInfo.activeLvl->GetTileRect(tileIdx, gameInfo) };
		utils::SetColor(Color4f{ 0.f, 1.f, 0.f, 1.f });
		utils::DrawRect(tileRect);

		int tileID{ gameInfo.activeLvl->GetTileID(tileIdx.r, tileIdx.c) };
		if (tileID != 1) continue; //no collision
		TileCorners tileCorners{ gameInfo.activeLvl->GetTileCorners(tileIdx, gameInfo) };
		utils::SetColor(Color4f{ 0.f, 0.f, 1.f, 1.f });
		utils::DrawLine(tileCorners.leftTop, tileCorners.rightBottom, 3.f);
		utils::DrawLine(tileCorners.leftBottom, tileCorners.rightTop, 3.f);
	}
}

void Madeline::Update(float dt, const Game::GameInfo& gameInfo)
{
	//if (m_State == State::Idle && m_Vel.x == 0.f && m_Vel.y <= 0.f) return;

	Rectf prevBounds{ m_Bounds };

	float velDistX{ m_Vel.x * gameInfo.PIX_PER_M * dt };
	m_Bounds.left += velDistX;

	m_Vel.y += -(gameInfo.G * gameInfo.G) * dt;
	float velDistY{ m_Vel.y * gameInfo.PIX_PER_M * dt };
	m_Bounds.bottom += velDistY;

	//Screen boundary checking
	//if (m_Bounds.bottom < 0.f)
	//{ 
	//	m_OnGround = true;
	//	m_Vel.y = 0.f;
	//	m_Bounds.bottom = 0.f;
	//}

	//std::cout << "Vel X: " << m_Vel.x << " Y:" << m_Vel.y << std::endl;
	bool IsVelUp{ m_Vel.y > 0.0f };
	bool IsVelRight{ m_Vel.x > 0.f };
	
	TileIdx playerTileIdx{ gameInfo.activeLvl->GetTileIdxByPos(Point2f{m_Bounds.left, m_Bounds.bottom}, gameInfo) };
	Point2f playerCornerPos{ (IsVelRight) ? m_Bounds.left + m_Bounds.width : m_Bounds.left,
							 (IsVelUp) ? m_Bounds.bottom + m_Bounds.height : m_Bounds.bottom };
	//Get Tile ranges to check collision
	SetCollisionTiles(gameInfo);
	TileIdx cornerTileIdx{ m_CollisionTiles[0] };

	//bool xCollision{ false }, yCollision{ false };
	for (const TileIdx& tileIdx : m_CollisionTiles)
	{
		int tileID{ gameInfo.activeLvl->GetTileID(tileIdx.r, tileIdx.c) };
		if (tileID != 1) continue; //No collision
		int collX{ (tileIdx.c + int(!IsVelRight)) * gameInfo.TILE_SIZE_PIX_SCALED };
		int collY{ (tileIdx.r + int(!IsVelUp)) * gameInfo.TILE_SIZE_PIX_SCALED };
		
		if (tileIdx.r == cornerTileIdx.r && tileIdx.c == cornerTileIdx.c)
		{
			Rectf collisionRect{ gameInfo.activeLvl->GetTileRect(tileIdx, gameInfo) };
			std::vector<Point2f> verts{ 
				Point2f{collisionRect.left, collisionRect.bottom},
				Point2f{collisionRect.left, collisionRect.bottom + collisionRect.height},
				Point2f{collisionRect.left + collisionRect.width, collisionRect.bottom + collisionRect.height},
				Point2f{collisionRect.left + collisionRect.width, collisionRect.bottom}
			};
			utils::HitInfo hitInfo{};
			const float rayScale{ 1.2f };
			Point2f rayEnd{ playerCornerPos.x - (velDistX * rayScale), playerCornerPos.y - (velDistY * rayScale) };
			//Raycast should ALWAYS return true, player entered collision tile with certain
			//velDistX and velDistY, tracing this dist backwards gives the pos where the player
			//entered the collision tile
			if (utils::Raycast(verts, playerCornerPos, rayEnd, hitInfo))
			{
				if (hitInfo.intersectPoint.y == collY)
				{
					m_Bounds.bottom = collY - m_Bounds.height * int(IsVelUp);
					m_Vel.y = 0.f;
				}
				else if (hitInfo.intersectPoint.x == collX)
				{
					m_Bounds.left = collX - m_Bounds.width * int(IsVelRight);
					m_Vel.x = 0.f;
				}
				break;
			}
		}
		else if (tileIdx.r == cornerTileIdx.r && tileIdx.c != cornerTileIdx.c)
		{
			m_Bounds.bottom = collY - m_Bounds.height * int(IsVelUp);
			m_Vel.y = 0.f;
			break;
		}
		else if (tileIdx.c == cornerTileIdx.c && tileIdx.r != cornerTileIdx.r)
		{
			m_Bounds.left = collX - m_Bounds.width * int(IsVelRight);
			m_Vel.x = 0.f;
			break;
		}
	}
	m_Vel.x = 0.f;
	//m_Vel.y = 0.f; //Remove when enabling gravity again
}

void Madeline::Move(float dt, Direction dir)
{
	switch (dir)
	{
	case Direction::Left:
		m_Vel.x = -m_Speed;
		break;
	case Direction::Right:
		m_Vel.x = m_Speed;
		break;
	case Direction::Up:
		m_Vel.y = m_Speed;
		break;
	case Direction::Down:
		m_Vel.y = -m_Speed;
		break;
	}
}

void Madeline::Jump()
{
	m_Test = true;
	//m_Vel.y += m_JUMP_ADDED_VEL;
}

Rectf Madeline::GetBounds() const
{
	return m_Bounds;
}

void Madeline::SetCollisionTiles(const Game::GameInfo& gameInfo)
{
	m_CollisionTiles.clear();

	//Find the 4 corner tiles Madeline overlaps with
	TileIdx leftBottomIdx{ gameInfo.activeLvl->GetTileIdxByPos(Point2f{ m_Bounds.left, m_Bounds.bottom }, gameInfo) };
	TileIdx rightTopIdx{ gameInfo.activeLvl->GetTileIdxByPos(Point2f{ m_Bounds.left + m_Bounds.width, m_Bounds.bottom + m_Bounds.height }, gameInfo) };
	TileIdx leftTopIdx{ rightTopIdx.r, leftBottomIdx.c };
	TileIdx rightBottomIdx{ leftBottomIdx.r, rightTopIdx.c };

	bool IsVelUp{ m_Vel.y > 0.f };
	bool IsVelRight{ m_Vel.x > 0.f };
	//Select 3/4 corner tiles based on movement/velocity direction
	//Collision can only happen in tiles somewhere between these 3 corners
	TileIdx cornerTile{
		(IsVelUp) ? leftTopIdx.r : leftBottomIdx.r,
		(IsVelRight) ? rightTopIdx.c : leftTopIdx.c,
	};
	TileIdx xCornerTile{ //The corner tile left or right from the cornerTile
		cornerTile.r,
		(IsVelRight) ? leftTopIdx.c : rightTopIdx.c
	};
	TileIdx yCornerTile{ //The corner tile up or down from the cornerTile
		(IsVelUp) ? leftBottomIdx.r : leftTopIdx.r,
		cornerTile.c
	};

	m_CollisionTiles.push_back(cornerTile);
	//Only check tiles left or right when the player is moving in the x-direction
	if (m_Vel.x != 0.f)
	{
		bool IsCornerOnTop{ cornerTile.r > yCornerTile.r };
		int minRow{ (IsCornerOnTop) ? yCornerTile.r		: cornerTile.r };
		int maxRow{ (IsCornerOnTop) ? cornerTile.r + 1	: yCornerTile.r + 1 };
		//Add all tiles above or below the player
		for (int row{ minRow }; row < maxRow; ++row)
			m_CollisionTiles.push_back(TileIdx{ row, cornerTile.c });
	}
	//Add all tiles left or right from the player
	bool IsCornerLeft{ cornerTile.c < xCornerTile.c };
	int minCol{ (IsCornerLeft) ? cornerTile.c		: xCornerTile.c };
	int maxCol{ (IsCornerLeft) ? xCornerTile.c + 1	: cornerTile.c + 1 };
	for (int col{ minCol }; col < maxCol; ++col)
		m_CollisionTiles.push_back(TileIdx{ cornerTile.r, col });
}



//float playerY{ (IsVelUp) ? m_Bounds.bottom + m_Bounds.height : m_Bounds.bottom };
//float depthY{ abs(playerY - collY) }; //The y Distance the player is inside of the collision tile
//bool prevOnGround{ abs(depthY - velDistY) < 1.f };
//if (!prevOnGround && velDistX != 0.f)
//{
//	float velPercentageY{ 0.f };
//	if (velDistY != 0.f)
//		velPercentageY = depthY / abs(velDistY); //The percentage of velDistY that is inside the collision tile
//	m_Bounds.left += velPercentageY * velDistX;
//	//std::cout << "velPercentageY: " << velPercentageY << std::endl;
//}

//float playerX{ (IsVelRight) ? m_Bounds.left + m_Bounds.width : m_Bounds.left };
//float depthX{ abs(playerX - collX) }; //The x Distance the player is inside of the collision tile
//bool prevOnWall{ abs(depthX - velDistX) < 1.f };
//if (!prevOnWall && velDistY != 0.f)
//{
//	float velPercentageX{ 0.f };
//	if (velDistX != 0.f)
//		velPercentageX = depthX / abs(velDistX); //The percentage of velDistX that is inside the collision tile
//	m_Bounds.bottom += velPercentageX * velDistY;
//	//st