#pragma once
#include <vector>
#include "Game.h"

class Level final
{
public:
	struct VelBasedCornerTiles
	{
		TileIdx corner;
		TileIdx xCorner;
		TileIdx yCorner;
	};

	struct VelInfo
	{
		bool left;
		bool right;
		bool up;
		bool down;
		bool inX;
		bool inY;
	};

	struct CollisionDir
	{
		bool x;
		bool left;
		bool right;
		bool y;
		bool up;
		bool down;
		bool corner;
		bool leftBottom;
		bool leftTop;
		bool rightTop;
		bool rightBottom;
	};

	struct RectPhysicsInfo
	{
		Rectf& bounds;
		Vector2f& vel;
		const Vector2f velDist;
	};

	struct CollisionInfo
	{
		bool collided;
		CollisionDir collDir;
		Point2f collPos;
		float entryDistLeft;
		float entryDistRight;
		float entryDistUp;
		float entryDistDown;
		VelInfo vi;
	};

	Level();

	void Draw() const;
	TileIdx GetTileIdxByPos(const Point2f& pos, bool offsetTop = false) const;
	int GetTileID(TileIdx tileIdx) const;
	int GetTileID(int row, int col) const;
	Rectf GetTileRect(TileIdx tileIdx) const;
	RectCornerTileIndices GetRectCornerTileIndices(const Rectf& rect) const;

	CollisionInfo DetectCollision(const RectPhysicsInfo& rfi, bool addVelToPos = false, bool checkCollInX = true, bool checkCollInY = true, bool checkCollInVelDir = false) const;
	CollisionInfo MovePhysicsRect(RectPhysicsInfo& rfi) const;
	const VelInfo GetVelInfo(const Vector2f& vel) const;
private:
	//Functions
	void FlipLevel();
	VelBasedCornerTiles GetVelBasedCornerTiles(const RectCornerTileIndices& corners, const VelInfo& moving) const;
	void SetCollisionDir(const RectPhysicsInfo& rfi, CollisionInfo& ci, bool addVelToPos = false, bool checkCollInX = true, bool checkCollInY = true, bool checkCollInVelDir = false) const;
	void SetCornerCollisionInfo(const RectPhysicsInfo& rfi, CollisionInfo& ci ) const;
	void CollisionResponse(RectPhysicsInfo& rec, CollisionInfo& ci) const;
	bool CheckRowCollision(int row, int minCol, int maxCol) const;
	bool CheckCollCollision(int col, int minRow, int maxRow) const;

	//Members
	int m_Rows;
	int m_Cols;
	std::vector< std::vector<int8_t> > m_Data;
};

