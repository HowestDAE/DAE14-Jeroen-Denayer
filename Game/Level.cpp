#include "pch.h"
#include "Level.h"
#include "GameData.h"

Level::Level()
	: m_Rows{ 23 }
	, m_Cols{ 40 }
	, m_TileSize{ 8 }
	, m_PixPerM{ 8 }
{
	//TO-DO Load level data directly from disk

	//Reserve only necessary space for level
	m_Data.reserve(m_Rows);
	for (int r{}; r < m_Rows; ++r)
	{
		m_Data.push_back(std::vector<int8_t>{});
		m_Data[r].reserve(m_Cols);
	}
	//m_Data = {
	//	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	//	{1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	//	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	//	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	//	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	//	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
	//	{1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
	//	{1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
	//	{1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//	{1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//	{1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
	//	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
	//	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
	//	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	//	{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	//	{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	//	{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	//	{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	//	{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	//	{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1},
	//	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1},
	//	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1}
	//};

	m_Data = {
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
		{1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
		{1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
		{1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
		{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
		{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
		{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
	};

	FlipLevel();
}

void Level::Draw() const
{
	const float TILE_SIZE{ float(GameData::TILE_SIZE_PIX()) };
	for (int r{ }; r < m_Data.size(); ++r)
	{
		for (int c{}; c < m_Data[r].size(); ++c)
		{
			int value{ m_Data[r][c] };
			if (value == 1)
				utils::SetColor(Color4f{ 0.5f, 0.5f, 0.5f, 1.f });
			else if (value == 0)
				utils::SetColor(Color4f{ 0.f, 0.f, 0.f, 1.f });

			float x{ c * TILE_SIZE };
			float y{ r * TILE_SIZE };
			utils::FillRect(x, y, TILE_SIZE, TILE_SIZE);
		}
	}

	//Draw debug grid lines
	utils::SetColor(Color4f{ 0.25f, 0.25f, 0.25f, 1.f });
	for (int row{ }; row < m_Rows; ++row)
	{
		float y{ row * TILE_SIZE };
		utils::DrawLine(Point2f{ 0.0f, y }, Point2f{ GameData::RENDER_RES_X(), y });
	}
	for (int col{}; col < m_Cols; ++col)
	{
		float x{ col * TILE_SIZE };
		utils::DrawLine(Point2f{ x, 0.0f }, Point2f{ x, GameData::RENDER_RES_Y() });
	}
}

int Level::GetTileID(TileIdx tileIdx) const
{
	return GetTileID(tileIdx.r, tileIdx.c);
}

int Level::GetTileID(int row, int col) const
{	
	if (row >= 0 && row < m_Rows && col >= 0 && col < m_Cols) return m_Data[row][col];
	else return -1;
}

CollisionInfo Level::DetectRectCollision(const Rectf& bounds, bool checkXDir, bool checkYDir, const Vector2f& vel, float time, bool checkVelDir) const
{
	CollisionInfo ci{};
	ci.vIn = utils::GetVelInfo(vel);
	Vector2f velDist{ vel.x * m_PixPerM * time, vel.y * m_PixPerM * time };

	ci.collDir = GetCollisionDir(bounds, checkXDir, checkYDir, velDist, time, checkVelDir);
	if (!ci.collDir.x && !ci.collDir.y && !ci.collDir.corner)
		return ci;

	ci.collided = true;
	Rectf movedBounds{ bounds.left + velDist.x, bounds.bottom + velDist.y, bounds.width, bounds.height };
	SetCollDirInfo(movedBounds, velDist, ci);
	return ci;
}

/*
Checks for collision between a Rectf and a Level
If a collision was found, the Rectf is moved to an appropriate position
The velocity is also modified!!
*/
CollisionInfo Level::MovePhysicsRect(Rectf& bounds, Vector2f& vel, float time) const
{
	CollisionInfo ci{ DetectRectCollision(bounds, true, true, vel, time, true) };

	//Set position and vel based on collision information
	if (ci.collDir.x)
	{
		vel.x = 0.f;
		if (ci.collDir.left && !ci.collDir.right)
			bounds.left = ci.left.collPos;
		else if (ci.collDir.right && !ci.collDir.left)
			bounds.left = ci.right.collPos - bounds.width;
		// else left && right -> squished form both sides
	}
	else
		bounds.left += vel.x * m_PixPerM * time;

	if (ci.collDir.y)
	{
		vel.y = 0.f;
		if (ci.collDir.up && !ci.collDir.down)
			bounds.bottom = ci.up.collPos - bounds.height;
		else if (ci.collDir.down && !ci.collDir.up)
			bounds.bottom = ci.down.collPos;
		// else up && down -> squished from both sides
	}
	else
		bounds.bottom += vel.y * m_PixPerM * time;

	return ci;
}

void Level::FlipLevel()
{
	std::vector< std::vector<int8_t> > dataCopy{ m_Data };
	for (int row{}; row < m_Data.size(); ++row)
	{
		m_Data[row] = dataCopy[m_Data.size() - 1 - row];
	}
}

bool Level::CheckCollCollision(int col, int minRow, int maxRow) const
{
	for (int row{ minRow }; row < maxRow + 1; ++row)
		if (GetTileID(row, col) == 1)
			return true;
	return false;
}

bool Level::CheckRowCollision(int row, int minCol, int maxCol) const
{
	for (int col{ minCol }; col < maxCol + 1; ++col)
		if (GetTileID(row, col) == 1)
			return true;
	return false;
}

CollisionDir Level::GetCollisionDir(const Rectf& bounds, bool checkXDir, bool checkYDir, const Vector2f& velDist, float time, bool checkVelDir) const
{
	CollisionDir collDir{};
	VelInfo moving{ utils::GetVelInfo(velDist) };
	bool checkCollInX{ checkXDir && (!checkVelDir || moving.inX) };
	bool checkCollInY{ checkYDir && (!checkVelDir || moving.inY) };

	//Checking left/right
	if (checkCollInX)
	{
		Rectf boundsCopy{ bounds };
		boundsCopy.left += velDist.x;
		RectCornersTileIdx corners{ utils::GetRectCornersTileIdx(boundsCopy, m_TileSize) };
		int minRow{ corners.leftBottom.r };
		int maxRow{ corners.leftTop.r };
		if (!checkVelDir || moving.left)
			collDir.left = CheckCollCollision(corners.leftBottom.c, minRow, maxRow);
		if (!checkVelDir || moving.right)
			collDir.right = CheckCollCollision(corners.rightBottom.c, minRow, maxRow);
		if (collDir.left || collDir.right)
			collDir.x = true;
	}

	//Checking up/down
	if (checkCollInY)
	{
		Rectf boundsCopy{ bounds };
		boundsCopy.bottom += velDist.y;
		RectCornersTileIdx corners{ utils::GetRectCornersTileIdx(boundsCopy, m_TileSize) };
		int minCol{ corners.leftBottom.c };
		int maxCol{ corners.rightBottom.c };
		if (!checkVelDir || moving.down)
			collDir.down = CheckRowCollision(corners.leftBottom.r, minCol, maxCol);
		if (!checkVelDir || moving.up)
			collDir.up = CheckRowCollision(corners.leftTop.r, minCol, maxCol);
		if (collDir.up || collDir.down)
			collDir.y = true;
	}

	//Checking corners
	if (checkCollInX && checkCollInY)
	{
		Rectf boundsCopy{ bounds };
		boundsCopy.left += velDist.x;
		boundsCopy.bottom += velDist.y;
		RectCornersTileIdx corners{ utils::GetRectCornersTileIdx(boundsCopy, m_TileSize) };
		TileIdx cornerInVelDir{
			(moving.up) ? corners.leftTop.r : corners.leftBottom.r,
			(moving.right) ? corners.rightBottom.c : corners.leftBottom.c
		};
		if ((!checkVelDir || corners.leftBottom == cornerInVelDir) && GetTileID(corners.leftBottom) == 1)
			collDir.leftBottom = true;
		if ((!checkVelDir || corners.leftTop == cornerInVelDir) && GetTileID(corners.leftTop) == 1)
			collDir.leftTop = true;
		if ((!checkVelDir || corners.rightTop == cornerInVelDir) && GetTileID(corners.rightTop) == 1)
			collDir.rightTop = true;
		if ((!checkVelDir || corners.rightBottom == cornerInVelDir) && GetTileID(corners.rightBottom) == 1)
			collDir.rightBottom = true;
		if (collDir.leftBottom || collDir.leftTop || collDir.rightTop || collDir.rightBottom)
			collDir.corner = true;
	}

	return collDir;
}

void Level::SetCollDirInfo(const Rectf& bounds, const Vector2f& velDist, CollisionInfo& ci) const
{
	RectCornersTileIdx corners{ utils::GetRectCornersTileIdx(bounds, m_TileSize) };
	//Calculate collision position and entryDist if collision happened in a direction
	if (ci.collDir.left || ci.collDir.leftBottom || ci.collDir.leftTop)
	{
		ci.left.collPos = float((corners.leftBottom.c + 1) * m_TileSize);
		ci.left.entryDist = ci.left.collPos - bounds.left;
		ci.left.lambda = std::min(ci.left.entryDist / std::abs(velDist.x), 1.f);
	}
	if (ci.collDir.right || ci.collDir.rightBottom || ci.collDir.rightTop)
	{
		ci.right.collPos = float(corners.rightBottom.c * m_TileSize);
		ci.right.entryDist = bounds.left + bounds.width - ci.right.collPos;
		ci.right.lambda = std::min(ci.right.entryDist / std::abs(velDist.x), 1.f);
	}
	if (ci.collDir.up || ci.collDir.leftTop || ci.collDir.rightTop)
	{
		ci.up.collPos = float(corners.leftTop.r * m_TileSize);
		ci.up.entryDist = bounds.bottom + bounds.height - ci.up.collPos;
		ci.up.lambda = std::min(ci.up.entryDist / std::abs(velDist.y), 1.f);
	}
	if (ci.collDir.down || ci.collDir.leftBottom || ci.collDir.rightBottom)
	{
		ci.down.collPos = float((corners.leftBottom.r + 1) * m_TileSize);
		ci.down.entryDist = ci.down.collPos - bounds.bottom;
		ci.down.lambda = std::min(ci.down.entryDist / std::abs(velDist.y), 1.f);
	}

	///* If the input rect is moving annd it collided only with a corner,
	//detect which axis collided first */
	if (ci.vIn.inX && ci.vIn.inY && ci.collDir.corner && !ci.collDir.x && !ci.collDir.y) //Convex corner collision
	{
		float lambdaX{ (ci.vIn.right) ? ci.right.lambda : ci.left.lambda };
		float lambdaY{ (ci.vIn.up) ? ci.up.lambda : ci.down.lambda };
		if (lambdaY <= lambdaX && lambdaY > 0.f)
		{
			ci.collDir.y = true;
			ci.collDir.up = ci.vIn.up;
			ci.collDir.down = ci.vIn.down;
		}
		else if (lambdaX < lambdaY && lambdaX > 0.f)//lambdaX > lambdaY
		{
			ci.collDir.x = true;
			ci.collDir.left = ci.vIn.left;
			ci.collDir.right = ci.vIn.right;
		}
	}
}