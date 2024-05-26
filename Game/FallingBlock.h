#pragma once
#include "PhysicsBody.h"
#include <vector>

//Forward declarations
class Texture;
class LevelScreen;

class FallingBlock final
	:public PhysicsBody
{
public:
	FallingBlock(const TileIdx& leftBottomIdx, int rows, int cols, const std::vector<uint8_t> data);
	FallingBlock(const FallingBlock& other) = delete;
	FallingBlock& operator=(const FallingBlock& other) = delete;
	FallingBlock(FallingBlock&& other) = delete;
	FallingBlock& operator=(FallingBlock&& other) = delete;

	virtual void Draw(const LevelScreen* pLevelScreen = nullptr) const override;
	virtual void Update(float dt) override;
	virtual void CollisionInfoResponse(int idx, const CollisionInfo& ci) override;

	std::string String() const;
private:
	//Members
	TileIdx m_LeftBottomIdx;
	int m_Rows;
	int m_Cols;
	std::vector<uint8_t> m_Data;
};

