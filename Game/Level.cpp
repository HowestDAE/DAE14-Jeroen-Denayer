#include "pch.h"
#include "Level.h"
#include "GameData.h"

Level::Level()
	: m_Rows{ 23 }
	, m_Cols{ 40 }
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

TileIdx Level::GetTileIdxByPos(const Point2f& pos, bool offsetTop) const
{
	float fRowIdx{ pos.y / GameData::TILE_SIZE_PIX() };
	float fColIdx{ pos.x / GameData::TILE_SIZE_PIX() };
	int rowIdx{ int(fRowIdx) };
	int colIdx{ int(fColIdx) };
	//Offset row/col so positions that are perfectly on the border of the next row/col
	//are still considered to be in the prev row/col
	if (offsetTop)
	{
		if (fRowIdx - int(fRowIdx) == 0.f)
			--rowIdx;
		if (fColIdx - int(fColIdx) == 0.f)
			--colIdx;
	}
	return TileIdx{ rowIdx, colIdx };
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

Rectf Level::GetTileRect(TileIdx tileIdx) const
{
	const int tileSizePix{ GameData::TILE_SIZE_PIX() };
	int x{ tileIdx.c * tileSizePix };
	int y{ tileIdx.r * tileSizePix };
	return Rectf{ float(x), float(y), float(tileSizePix), float(tileSizePix) };
}

RectCornerTileIndices Level::GetRectCornerTileIndices(const Rectf& rect) const
{
	TileIdx leftBottomIdx{ GetTileIdxByPos(Point2f{ rect.left, rect.bottom }) };
	TileIdx rightTopIdx{ GetTileIdxByPos(Point2f{ rect.left + rect.width, rect.bottom + rect.height }, true) };
	TileIdx leftTopIdx{ rightTopIdx.r, leftBottomIdx.c };
	TileIdx rightBottomIdx{ leftBottomIdx.r, rightTopIdx.c };
	return RectCornerTileIndices{leftBottomIdx, leftTopIdx, rightTopIdx, rightBottomIdx };
}

Level::CollisionInfo Level::DetectCollision(const RectPhysicsInfo& rfi, bool addVelToPos, bool checkCollInX, bool checkCollInY, bool checkCollInVelDir) const
{
	CollisionInfo ci{};
	ci.vi = GetVelInfo(rfi.vel);
	if (checkCollInVelDir)
	{
		if (!ci.vi.inX)
			checkCollInX = false;
		if (!ci.vi.inY)
			checkCollInY = false;
	}
	Rectf prevBounds{ rfi.bounds };
	SetCollisionDir(rfi, ci, addVelToPos, checkCollInX, checkCollInY, checkCollInVelDir);
	if (!ci.collDir.x && !ci.collDir.y && !ci.collDir.corner)
		return ci;

	ci.collided = true;

	Rectf boundsCopy{ rfi.bounds };
	if (addVelToPos)
	{
		boundsCopy.left += rfi.velDist.x;
		boundsCopy.bottom += rfi.velDist.y;
	}

	RectCornerTileIndices corners{ GetRectCornerTileIndices(boundsCopy) };
	float collPosLeft{ float((corners.leftBottom.c + 1) * GameData::TILE_SIZE_PIX()) };
	float collPosRight{ float(corners.rightBottom.c * GameData::TILE_SIZE_PIX()) };
	float collPosUp{ float(corners.leftTop.r * GameData::TILE_SIZE_PIX()) };
	float collPosDown{ float((corners.leftBottom.r + 1) * GameData::TILE_SIZE_PIX()) };

	if (checkCollInVelDir)
	{
		ci.collPos.x = (ci.vi.right) ? collPosRight : collPosLeft;
		ci.collPos.y = (ci.vi.up) ? collPosUp : collPosDown;
	}

	//Calculate how far the bounds is inside of the collision
	if (ci.collDir.left)
		ci.entryDistLeft = collPosLeft - boundsCopy.left;
	if (ci.collDir.right)
		ci.entryDistRight = boundsCopy.left + boundsCopy.width - collPosRight;
	if (ci.collDir.up)
		ci.entryDistUp = boundsCopy.bottom + boundsCopy.height - collPosUp;
	if (ci.collDir.down)
		ci.entryDistDown = collPosDown - boundsCopy.bottom;

	return ci;
}

/*
Checks for collision between a Rectf and a Level
If a collision was found, the Rectf is moved to an appropriate position
The velocity is also modified!!
*/
Level::CollisionInfo Level::MovePhysicsRect(RectPhysicsInfo& rfi) const
{
	CollisionInfo ci{ DetectCollision(rfi, true, true, true, true) };
	if (!ci.collided) //No collision
	{
		rfi.bounds.left += rfi.velDist.x;
		rfi.bounds.bottom += rfi.velDist.y;
		return ci;
	}
	CollisionResponse(rfi, ci);
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

const Level::VelInfo Level::GetVelInfo(const Vector2f& vel) const
{
	bool movingLeft{ vel.x < 0.f };
	bool movingRight{ vel.x > 0.f };
	bool movingUp{ vel.y > 0.f };
	bool movingDown{ vel.y < 0.f };
	bool movingInX{ vel.x != 0.f };
	bool movingInY{ vel.y != 0.f };
	return VelInfo{movingLeft, movingRight, movingUp, movingDown, movingInX, movingInY };
}

/*
Select 3/4 corner tiles based on movement/velocity direction
*/
Level::VelBasedCornerTiles Level::GetVelBasedCornerTiles(const RectCornerTileIndices& corners, const VelInfo& moving) const
{
	int bottomRow{ corners.leftBottom.r };
	int topRow{ corners.leftTop.r };
	int leftCol{ corners.leftBottom.c };
	int rightCol{ corners.rightBottom.c };
	TileIdx corner{
		(moving.up) ? topRow : bottomRow,
		(moving.right) ? rightCol : leftCol,
	};
	TileIdx xCorner{ //The corner left or right from the corner
		corner.r,
		(moving.right) ? leftCol : rightCol
	};
	TileIdx yCorner{ //The corner up or down from the corner
		(moving.up) ? bottomRow : topRow,
		corner.c
	};
	return VelBasedCornerTiles{ corner, xCorner, yCorner };
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

void Level::SetCollisionDir(const RectPhysicsInfo& rfi, CollisionInfo& ci, bool addVelToPos, bool checkCollInX, bool checkCollInY, bool checkCollInVelDir) const
{
	//Checking left/right
	if (checkCollInX)
	{
		Rectf boundsCopy{ rfi.bounds };
		if (addVelToPos)
			boundsCopy.left += rfi.velDist.x;
		RectCornerTileIndices corners{ GetRectCornerTileIndices(boundsCopy) };
		int minRow{ corners.leftBottom.r };
		int maxRow{ corners.leftTop.r };
		if (!checkCollInVelDir || ci.vi.left)
			ci.collDir.left = CheckCollCollision(corners.leftBottom.c, minRow, maxRow);
		if (!checkCollInVelDir || ci.vi.right)
			ci.collDir.right = CheckCollCollision(corners.rightBottom.c, minRow, maxRow);
		if (ci.collDir.left || ci.collDir.right)
			ci.collDir.x = true;
	}

	//Checking up/down
	if (checkCollInY)
	{
		Rectf boundsCopy{ rfi.bounds };
		if (addVelToPos)
			boundsCopy.bottom += rfi.velDist.y;
		RectCornerTileIndices corners{ GetRectCornerTileIndices(boundsCopy) };
		int minCol{ corners.leftBottom.c };
		int maxCol{ corners.rightBottom.c };
		if (!checkCollInVelDir || ci.vi.down)
			ci.collDir.down = CheckRowCollision(corners.leftBottom.r, minCol, maxCol);
		if (!checkCollInVelDir || ci.vi.up)
			ci.collDir.up = CheckRowCollision(corners.leftTop.r, minCol, maxCol);
		if (ci.collDir.up || ci.collDir.down)
			ci.collDir.y = true;
	}

	//Checking corners
	if (checkCollInX && checkCollInY)
	{
		Rectf boundsCopy{ rfi.bounds };
		if (addVelToPos)
		{
			boundsCopy.left += rfi.velDist.x;
			boundsCopy.bottom += rfi.velDist.y;
		}
		RectCornerTileIndices corners{ GetRectCornerTileIndices(boundsCopy) };
		TileIdx cornerInVelDir{
			(ci.vi.up) ? corners.leftTop.r : corners.leftBottom.r,
			(ci.vi.right) ? corners.rightBottom.c : corners.leftBottom.c
		};
		if ((!checkCollInVelDir || corners.leftBottom == cornerInVelDir) && GetTileID(corners.leftBottom) == 1)
			ci.collDir.leftBottom = true;
		if ((!checkCollInVelDir || corners.leftTop == cornerInVelDir) && GetTileID(corners.leftTop) == 1)
			ci.collDir.leftTop = true;
		if ((!checkCollInVelDir || corners.rightTop == cornerInVelDir) && GetTileID(corners.rightTop) == 1)	
			ci.collDir.rightTop = true;
		if ((!checkCollInVelDir || corners.rightBottom == cornerInVelDir) && GetTileID(corners.rightBottom) == 1)
			ci.collDir.rightBottom = true;
		if (ci.collDir.leftBottom || ci.collDir.leftTop || ci.collDir.rightTop || ci.collDir.rightBottom)
			ci.collDir.corner = true;
	}
}

/*
Detects if the collision first occured in the x or y direction
*/
void Level::SetCornerCollisionInfo(const RectPhysicsInfo& rfi, CollisionInfo& ci) const
{
	const VelInfo& moving{ ci.vi };
	//Check if rect is flush with the collX or collY boundary
	bool xAlreadyColliding{ rfi.bounds.left == ci.collPos.x }; //Flush with wall left/right
	bool yAlreadyColliding{ rfi.bounds.bottom == ci.collPos.y }; //Flush with ground/ceiling

	float entryDistX{ (moving.right) ? ci.entryDistRight : ci.entryDistLeft };
	float entryDistY{ (moving.up) ? ci.entryDistUp : ci.entryDistDown };

	float velPercentageX{ -std::numeric_limits<float>::infinity() };
	float velPercentageY{ -std::numeric_limits<float>::infinity() };
	//Find time of collision for each axis, if statement prevents divide by zero
	if (moving.inX && !yAlreadyColliding)
		velPercentageX = entryDistX / std::abs(rfi.velDist.x);
	if (moving.inY && !xAlreadyColliding)
		velPercentageY = entryDistY / std::abs(rfi.velDist.y);

	//Shouldn't happen !!! Dist entered in x or y should always be between 0 and 1
	if (velPercentageX < 0.f && velPercentageY < 0.f)
	{
		std::cout << "Unhandled collision" << std::endl;
		return;
	}

	//Find the earliest times of collision
	if (velPercentageX > 0.f && velPercentageY < 0.f) //Pure x collision
		ci.collDir.x = true;
	else if (velPercentageY > 0.f && velPercentageX < 0.f) //Pure y collision
		ci.collDir.y = true;
	else if (velPercentageY <= velPercentageX) //Collision first happened in y
		ci.collDir.y = true;
	else //Collision first happended in x
		ci.collDir.x = true;
}

void Level::CollisionResponse(RectPhysicsInfo& rfi, CollisionInfo& ci) const
{
	bool correctXPos{}, correctYPos{};
	if (ci.collDir.corner && !ci.collDir.x && !ci.collDir.y) //Convex corner collision
	{
		SetCornerCollisionInfo(rfi, ci);
		if (ci.collDir.x) // x collided first
			correctXPos = true;
		else // y collided first
			correctYPos = true;
	}
	else if (ci.collDir.x && !ci.collDir.y) //Wall collision
		correctXPos = true;
	else if (ci.collDir.y && !ci.collDir.x) //Ground/ceiling collision
		correctYPos = true;
	else if (ci.collDir.x && ci.collDir.y) //Concave corner collision
	{
		correctXPos = true;
		correctYPos = true;
	}

	//Update input bounds x position and vel
	if (correctXPos)
	{
		rfi.bounds.left = ci.collPos.x - rfi.bounds.width * int(ci.vi.right);
		rfi.vel.x = 0.f;
	}
	else
		rfi.bounds.left += rfi.velDist.x;

	//Update input bounds y position and vel
	if (correctYPos)
	{
		rfi.bounds.bottom = ci.collPos.y - rfi.bounds.height * int(ci.vi.up);
		rfi.vel.y = 0.f;
	}
	else
		rfi.bounds.bottom += rfi.velDist.y;
}
