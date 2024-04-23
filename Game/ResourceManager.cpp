#include "pch.h"
#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
	m_TextureNames = std::unordered_set<std::string>{
		
	};
}

ResourceManager::~ResourceManager()
{
	for (Texture* pTexture : m_pTextures)
		delete pTexture;
}
