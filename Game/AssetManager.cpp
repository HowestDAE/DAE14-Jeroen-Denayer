#include "pch.h"
#include "AssetManager.h"
#include <filesystem>
#include "Texture.h"
#include "FileIO.h"
#include <sstream>
#include "SoundStream.h"

AssetManager::AssetManager()
	: m_pDefaultTexture{ nullptr }
	, m_TextureNameIdMap{ std::unordered_map<std::string, GLuint>{} }
	, m_Textures{ std::unordered_map<GLuint, AssetManager::TextureInfo>{} }
	, m_pSoundStream{ nullptr }
	, m_SoundEffects{ std::unordered_map<std::string, SoundEffect>{} }
{
	m_pDefaultTexture = new Texture("Default.png");
}

AssetManager::~AssetManager()
{
	AssetManager& am{ Get() };
	delete am.m_pDefaultTexture;

	for (std::pair<const GLuint, TextureInfo>& textureInfo : am.m_Textures)
		delete textureInfo.second.pTexture;

	delete m_pSoundStream;
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
	if (!pTexture)
		return;

	GLuint id{ pTexture->GetId() };
	std::unordered_map<GLuint, TextureInfo>::iterator it{ m_Textures.find(id) };
	if (it != m_Textures.end())
	{
		--m_Textures[id].refCount;
		if (m_Textures[id].refCount == 0) //delete texture if nothing refers to it anymore
		{
			delete m_Textures[id].pTexture;
			m_TextureNameIdMap.erase(m_Textures[id].name);
			m_Textures.erase(id);
		}
	}
}

AssetManager& AssetManager::Get()
{
	static AssetManager instance;
	return instance;
}

Texture* AssetManager::GetTextureFromText(const std::string& text, const std::string& fontName, int ptSize, const Color4f& color)
{
	std::stringstream fontPath;
	fontPath << FileIO::GetDir(FileIO::Dir::Font) << fontName;
	return new Texture(text, fontPath.str(), ptSize, color);
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

bool AssetManager::PlaySoundStream(const std::string& name)
{
	AssetManager& am{ Get() };
	if (am.m_pSoundStream)
		delete am.m_pSoundStream;
	FileIO::LoadSound(name, am.m_pSoundStream);
	if (am.m_pSoundStream)
		am.m_pSoundStream->Play(true);
	return am.m_pSoundStream;
}

bool AssetManager::PlaySoundEffect(const std::string& name)
{
	AssetManager& am{ Get() };
	bool succes{ false };
	if (am.m_SoundEffects.find(name) != am.m_SoundEffects.end())
	{
		am.m_SoundEffects.at(name).Play(false);
	}
	else
	{
		SoundEffect* pSoundEffect{};
		FileIO::LoadSound(name, pSoundEffect);
		if (pSoundEffect)
		{
			am.m_SoundEffects.emplace(name, std::move(*pSoundEffect));
			am.m_SoundEffects.at(name).Play(false);
			succes = true;
		}
	}
	return succes;
}

void AssetManager::RemoveSoundEffect(const std::string& name)
{
	AssetManager& am{ Get() };
	if (am.m_SoundEffects.find(name) != am.m_SoundEffects.end())
	{
		am.m_SoundEffects.erase(name);
	}
}
