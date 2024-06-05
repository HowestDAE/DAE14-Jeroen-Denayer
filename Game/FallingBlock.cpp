#include "pch.h"
#include "FallingBlock.h"
#include "GameData.h"
#include "LevelScreen.h"
#include <sstream>

FallingBlock::FallingBlock(const TileIdx& leftBottomIdx, int rows, int cols, const std::vector<uint8_t> data)
	: PhysicsBody(PhysicsBody::Type::FallingBlock, Rectf{float(leftBottomIdx.c * GameData::TILE_SIZE_PIX()), float(leftBottomIdx.r * GameData::TILE_SIZE_PIX()), float(cols * GameData::TILE_SIZE_PIX()), float(rows * GameData::TILE_SIZE_PIX())})
	, m_LeftBottomIdx{ leftBottomIdx }
	, m_Rows{ rows }
	, m_Cols{ cols }
	, m_Data{ std::vector<uint8_t>(size_t(m_Rows * m_Cols)) }
	, m_UpdateFallTimer{ false }
	, m_FallTimer{ 2.f }
{
	PhysicsBody::Activate(false); //Start of stationary
	float dist = 3.5f;
	float time = 0.35f;
	AccAndVel fallAccVel{ utils::AccAndVelToTravelDistInTime(dist, time) };
	float gravity{ fallAccVel.acc };
	float maxFallSpeed = -30.f;
	SetMovement(Vector2f{ 0.f, maxFallSpeed }, Vector2f{ 0.f, 0.f }, Vector2f{ 0.f, gravity });
	const size_t min{ std::min(size_t(m_Rows * m_Cols), data.size()) };
	for (size_t i{}; i < min; ++i)
		m_Data[i] = data[i];

	AddOverlapRect(Vector2f{m_Bounds.left, 0.f}, m_Bounds.width, m_Bounds.bottom, PhysicsBody::Type::Madeline, false);
}

void FallingBlock::Draw(const LevelScreen* pLevelScreen) const
{
	LevelScreen::DrawTiles(m_Data, m_Rows, m_Cols, Vector2f{ m_Bounds.left, m_Bounds.bottom }, pLevelScreen);
	utils::SetColor(Color4f{ 1.f, 1.f, 1.f, 1.f });
	utils::DrawRect(m_Bounds, 2.f);
}

void FallingBlock::Update(float dt)
{
	if (m_UpdateFallTimer)
	{
		m_FallTimer -= dt;
		if (m_FallTimer < 0.f)
		{
			PhysicsBody::Activate(true);
			m_UpdateFallTimer = false;
		}
	}
}

void FallingBlock::CollisionInfoResponse(int idx, const CollisionInfo& ci)
{
	PhysicsBody::Activate(false);
}

std::string FallingBlock::String() const
{
	std::stringstream stream;
	stream << int(m_Type) << "," << m_LeftBottomIdx.r << "," << m_LeftBottomIdx.c << "," << m_Rows << "," << m_Cols;
	return stream.str();
}

void FallingBlock::Activate(bool activate)
{
	m_UpdateFallTimer = true;
}
