#pragma once
#include <vector>
#include "Game.h"

class Level final
{
public:
	Level();

	void Draw() const;
	TileIdx GetTileIdxByPos(const Point2f& pos, bool offsetTop = false) const;
	int GetTileID(int row, int col) const;
	Rectf GetTileRect(TileIdx tileIdx) const;
	RectCornerTileIndices GetRectCornerTileIndices(const Rectf& rect) const;
private:
	void FlipLevel();

	int m_Rows;
	int m_Cols;
	std::vector< std::vector<int8_t> > m_Data;
};

