#include "pch.h"
#include "AssetManager.h"
#include <filesystem>
#include "Texture.h"
#include "FileIO.h"

AssetManager::AssetManager()
	: m_pDefaultTexture{ nullptr }
	, m_TextureNameIdMap{ std::unordered_map<std::string, GLuint>{} }
	, m_Textures{ std::unordered_map<GLuint, AssetManager::TextureInfo>{} }
{
}

Texture* AssetManager::IGetTexture(const std::string& name)
{
	std::unordered_map<std::string, GLuint>::iterator it{ m_TextureNameIdMap.find(name) };
	if (it == m_TextureNameIdMap.end()) //does not yet exist
	{
		Texture* pTexture{};
		if (FileIO::LoadTexture(name, pTexture))
		{
			TextureInfo textureInfo{ TextureInfo{ pTexture, name, 1 } };
			GLuint id{ textureInfo.pTexture->GetId() };
			auto pair = m_TextureNameIdMap.emplace(name, id);
			m_Textures.emplace(id, textureInfo);
			if (pair.second) //.second is true if emplace() was succesful
				it = pair.first; //.first is iterator to element
		}
	}
	else //texture already exists
	{
		GLuint id{ m_TextureNameIdMap[name] };
		++m_Textures[id].refCount;
	}

	if (it != m_TextureNameIdMap.end())
	{
		GLuint id{ m_TextureNameIdMap[name] };
		return m_Textures[id].pTexture;
	}
	else
		return m_pDefaultTexture;
}

void AssetManager::IRemoveTexture(Texture* pTexture)
{
	GLuint id{ pTexture->GetId() };
	std::unordered_map<GLuint, TextureInfo>::iterator it{ m_Textures.find(id) };
	if (it != m_Textures.end())
	{
		--m_Textures[id].refCount;
		if (m_Textures[id].refCount == 0) //delete texture if nothing refers to it anymore
		{
			delete m_Textures[id].pTexture;
			m_Textures.erase(id);
			m_TextureNameIdMap.erase(m_Textures[id].name);
		}
	}
}

AssetManager& AssetManager::Get()
{
	static AssetManager instance;
	return instance;
}

void AssetManager::Cleanup()
{
	AssetManager& am{ Get() };
	delete am.m_pDefaultTexture;

	for (std::pair<const GLuint, TextureInfo>& textureInfo : am.m_Textures)
		delete textureInfo.second.pTexture;
}

Texture* AssetManager::GetTexture(const std::string& name)
{
	return Get().IGetTexture(name);
}

void AssetManager::GetTextures(const std::vector<std::string>& textureNames, std::vector<Texture*>& textureArr)
{
	for (const std::string& textureName : textureNames)
		textureArr.push_back(GetTexture(textureName));
}

void AssetManager::RemoveTexture(Texture* pTexture)
{
	Get().IRemoveTexture(pTexture);
}
