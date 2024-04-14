#include "pch.h"
#include "SpriteSheet.h"
#include "Texture.h"

SpriteSheet::SpriteSheet(const std::string& texturePath, int rows, int cols, int numFrames, float frameTime)
	: m_pTexture{ nullptr }
	, m_Rows{ rows }
	, m_Cols{ cols }
	, m_NumFrames{ numFrames }
	, m_FrameSize{}
	, m_FrameNr{}
	, m_FrameTime{ frameTime }
	, m_AccumulatedFrameTime{}
{
	m_pTexture = new Texture(texturePath);
	if (!m_pTexture)
		std::cout << "Could load " << texturePath << std::endl;
	else
		m_FrameSize = int(m_pTexture->GetWidth() / m_Cols);
}

SpriteSheet::~SpriteSheet()
{
	delete m_pTexture;
}

void SpriteSheet::Draw(const Rectf& dstRect) const
{
	int row{ m_FrameNr / m_Cols };
	int col{ m_FrameNr % m_Cols };
	Rectf srcRect{ float(col * m_FrameSize), float(row * m_FrameSize), float(m_FrameSize), float(m_FrameSize) };
	m_pTexture->Draw(dstRect, srcRect);
}

void SpriteSheet::Update(float dt)
{
	m_AccumulatedFrameTime += dt;
	if (m_AccumulatedFrameTime > m_FrameTime)
	{
		++m_FrameNr %= m_NumFrames;
		m_AccumulatedFrameTime -= m_FrameTime;
	}
}

void SpriteSheet::Flip(bool flipX)
{
	m_pTexture->Flip(flipX);
}
