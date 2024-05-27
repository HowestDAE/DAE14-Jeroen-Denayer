#include "pch.h"
#include "DashCrystal.h"
#include "Texture.h"
#include "AssetManager.h"
#include <sstream>
#include "AssetManager.h"

DashCrystal::DashCrystal(const Vector2f& pos)
	:PhysicsBody(PhysicsBody::Type::DashCrystal, Rectf{pos.x - 8.f, pos.y - 8.f, 16.f, 16.f})
	, m_pDashCrystal{ AssetManager::GetTexture("DashCrystal") }
	, m_pDashCrystalOutline{ AssetManager::GetTexture("DashCrystalOutline") }
	, m_Timer{ 0.f }
	, m_RespawnTime{ 5.f }
{
}

DashCrystal::~DashCrystal()
{
	AssetManager::RemoveTexture(m_pDashCrystal);
	AssetManager::RemoveTexture(m_pDashCrystalOutline);
}

void DashCrystal::Draw(const LevelScreen* pLevelScreen) const
{
	if (m_Active)
		m_pDashCrystal->Draw(m_Bounds);
	else
		m_pDashCrystalOutline->Draw(m_Bounds);
}

void DashCrystal::Update(float dt)
{
	if (!m_Active)
	{
		m_Timer -= dt;
		if (m_Timer < 0.f)
		{
			m_Active = true;
			m_Timer = m_RespawnTime;
		}
	}
}

void DashCrystal::CollisionInfoResponse(int idx, const CollisionInfo& ci)
{
	if (m_Active)
	{
		m_Active = false;
		AssetManager::PlaySoundEffect("DashCrystal");
	}
}

std::string DashCrystal::String() const
{
	std::stringstream stream;
	stream << int(m_Type) << "," << m_Bounds.left << "," << m_Bounds.bottom;
	return stream.str();
}
