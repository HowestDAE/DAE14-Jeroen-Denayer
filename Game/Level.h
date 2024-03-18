#pragma once
#include <vector>
#include "Game.h"

class Level final
{
public:
	Level();

	void Draw(const Game::GameInfo& gameInfo) const;
	TileIdx GetTileIdxByPos(const Point2f& pos, const Game::GameInfo& gameInfo) const;
	int GetTileID(int row, int col) const;
	Rectf GetTileRect(TileIdx tileIdx, const Game::GameInfo& gameInfo) const;
	TileCorners GetTileCorners(TileIdx tileIdx, const Game::GameInfo& gameInfo) const;
private:
	void FlipLevel();

	int m_Rows;
	int m_Cols;
	std::vector< std::vector<int> > m_Data;
};

