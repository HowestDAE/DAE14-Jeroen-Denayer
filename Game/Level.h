#pragma once
#include <vector>
#include "Game.h"

class Level final
{
public:
	Level();

	void Draw() const;
	int GetTileID(TileIdx tileIdx) const;
	int GetTileID(int row, int col) const;
	CollisionInfo DetectRectCollision(const Rectf& bounds, bool checkXDir = true, bool checkYDir = true, const Vector2f& vel = Vector2f{}, float time = 0.f, bool checkVelDir = false) const;
	CollisionInfo MovePhysicsRect(Rectf& bounds, Vector2f& vel, float time) const;
private:
	//Functions
	void FlipLevel();
	CollisionDir GetCollisionDir(const Rectf& bounds, bool checkXDir = true, bool checkYDir = true, const Vector2f& velDist = Vector2f{}, float time = 0.f, bool checkVelDir = false) const;
	void SetCollDirInfo(const Rectf& bounds, const Vector2f& velDist, CollisionInfo& ci) const;
	bool CheckRowCollision(int row, int minCol, int maxCol) const;
	bool CheckCollCollision(int col, int minRow, int maxRow) const;

	//Members
	int m_Rows;
	int m_Cols;
	int m_TileSize;
	int m_PixPerM;
	std::vector< std::vector<int8_t> > m_Data;
};

