#include "pch.h"
#include "MultiSpriteSheet.h"
#include "Texture.h"

MultiSpriteSheet::MultiSpriteSheet(const std::string& texturePath, int rows, int cols, const std::unordered_map<std::string, SpriteSheetInfo>& spriteSheetMapping)
	: SpriteSheet(texturePath, rows, cols, 0, 0)
	, m_Mapping{ spriteSheetMapping }
	, m_CurrentSpriteSheetName{}
{
}

void MultiSpriteSheet::Update(float dt)
{
	SpriteSheetInfo& info{ m_Mapping.at(m_CurrentSpriteSheetName) };

	m_AccumulatedFrameTime += dt;
	if (m_AccumulatedFrameTime > m_FrameTime)
	{
		++m_FrameNr;
		if (m_FrameNr >= info.endFrameNr)
			m_FrameNr = info.startFrameNr;
		m_AccumulatedFrameTime -= m_FrameTime;
	}
}

void MultiSpriteSheet::SetSpriteSheetName(const std::string& name)
{
	if (name == m_CurrentSpriteSheetName)
		return;

	if (m_Mapping.find(name) == m_Mapping.end())
	{
		m_FrameNr = 0;
		return;
	}

	const SpriteSheetInfo& info{ m_Mapping.at(name) };

	m_CurrentSpriteSheetName = name;
	m_FrameNr = info.startFrameNr;
	m_FrameTime = info.frameTime;
	m_AccumulatedFrameTime = 0.f;
}
