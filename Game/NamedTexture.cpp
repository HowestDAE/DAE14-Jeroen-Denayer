#include "pch.h"
#include "NamedTexture.h"
#include "Texture.h"

NamedTexture::NamedTexture(const std::string& name)
	: m_Name{ name }
	, m_Texture{ Globals::GetDataManager().GetTexture(name) }
{
}

NamedTexture::~NamedTexture()
{
	Globals::GetDataManager().RemoveTexture(m_Name);
}

NamedTexture::operator Texture& ()
{
	return m_Texture;
}
