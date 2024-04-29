#include "pch.h"
#include "AssetManager.h"
#include <filesystem>
#include "Texture.h"

AssetManager::AssetManager()
	: m_Dirs{ std::vector <std::string>{ "LevelsData/", "LevelScreensData/", "Textures/" } }
	, m_pDefaultTexture{ nullptr }
	, m_TextureNameIdMap{ std::unordered_map<std::string, GLuint>{} }
	, m_Textures{ std::unordered_map<GLuint, AssetManager::TextureInfo>{} }
{
}

Texture* AssetManager::IGetTexture(const std::string& name)
{
	std::unordered_map<std::string, GLuint>::iterator it{ m_TextureNameIdMap.find(name) };
	if (it == m_TextureNameIdMap.end()) //does not yet exist
	{
		const std::string& dir{ AssetManager::GetDir(AssetManager::Dir::Texture) };
		std::string texturePath{ dir + name + ".png" };
		if (!std::filesystem::exists(texturePath))
		{
			std::cout << "AssetManager::GetTexture(): Couldn't find texture on disk: " << texturePath << std::endl;
		}
		else //create a new texture
		{
			TextureInfo textureInfo{ TextureInfo{ new Texture(texturePath), name, 1 } };
			GLuint id{ textureInfo.pTexture->GetId() };
			auto pair = m_TextureNameIdMap.emplace(name, id);
			m_Textures.emplace(id, textureInfo);
			if (pair.second) //.seond is true if emplace() was succesful
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

std::string AssetManager::GetDir(Dir dir)
{
	return Get().m_Dirs[int(dir)];
}

Texture* AssetManager::GetTexture(const std::string& name)
{
	return Get().IGetTexture(name);
}

void AssetManager::RemoveTexture(Texture* pTexture)
{
	Get().RemoveTexture(pTexture);
}
