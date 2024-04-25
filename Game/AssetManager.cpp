#include "pch.h"
#include "AssetManager.h"
#include <filesystem>
#include "Texture.h"

std::vector <std::string> AssetManager::s_Dirs{ "LevelsData/", "LevelScreensData/", "Textures/" };
Texture* AssetManager::s_pDefaultTexture{ nullptr };
std::unordered_map<std::string, GLuint> AssetManager::s_TextureNameIdMap{};
std::unordered_map<GLuint, AssetManager::TextureInfo> AssetManager::s_Textures{};

AssetManager::AssetManager()
{
}

AssetManager& AssetManager::Get()
{
	static AssetManager instance;
	return instance;
}

void AssetManager::Init()
{
	delete s_pDefaultTexture;

	for (std::pair<const GLuint, TextureInfo>& textureInfo : s_Textures)
		delete textureInfo.second.pTexture;
}

void AssetManager::ReleaseDynamicMemory()
{
}

std::string AssetManager::GetDir(Dir dir)
{
	return Get().s_Dirs[int(dir)];
}

Texture* AssetManager::GetTexture(const std::string& name)
{
	std::unordered_map<std::string, GLuint>::iterator it{ s_TextureNameIdMap.find(name) };
	if (it == s_TextureNameIdMap.end()) //does not yet exist
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
			auto pair = s_TextureNameIdMap.emplace(name, id);
			s_Textures.emplace(id, textureInfo);
			if (pair.second) //.seond is true if emplace() was succesful
				it = pair.first; //.first is iterator to element
		}
	}
	else //texture already exists
	{
		GLuint id{ s_TextureNameIdMap[name] };
		++s_Textures[id].refCount;
	}

	if (it != s_TextureNameIdMap.end())
	{
		GLuint id{ s_TextureNameIdMap[name] };
		return s_Textures[id].pTexture;
	}
	else
		return s_pDefaultTexture;
}

void AssetManager::RemoveTexture(Texture* pTexture)
{
	GLuint id{ pTexture->GetId() };
	std::unordered_map<GLuint, TextureInfo>::iterator it{ s_Textures.find(id) };
	if (it != s_Textures.end())
	{
		--s_Textures[id].refCount;
		if (s_Textures[id].refCount == 0) //delete texture if nothing refers to it anymore
		{
			delete s_Textures[id].pTexture;
			s_Textures.erase(id);
			s_TextureNameIdMap.erase(s_Textures[id].name);
		}
	}
}
