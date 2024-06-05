#pragma once
#include <vector>
#include <unordered_map>
#include "SoundEffect.h"

//Forward declarations
class Texture;
class SoundStream;

class AssetManager final
{
public:
	static AssetManager& Get();

	AssetManager(const AssetManager& other) = delete;
	AssetManager& operator=(const AssetManager& other) = delete;
	AssetManager(AssetManager&& other) = delete;
	AssetManager& operator=(AssetManager&& other) = delete;

	enum class DataType
	{
		Texture
	};

	struct TextureInfo
	{
		Texture* pTexture;
		std::string name;
		int refCount;
	};

	static Texture* GetTextureFromText(const std::string& text, const std::string& fontName, int ptSize, const Color4f& color);
	static Texture* GetTexture(const std::string& name);
	static void GetTextures(const std::vector<std::string>& textureNames, std::vector<Texture*>& textureArr);
	static void RemoveTexture(Texture* pTexture);
	static bool PlaySoundStream(const std::string& name, float volumePercentage = 1.f);
	static bool PlaySoundEffect(const std::string& name);
	static void RemoveSoundEffect(const std::string& name);
private:
	AssetManager();
	~AssetManager();
	Texture* IGetTexture(const std::string& name);
	void IRemoveTexture(Texture* pTexture);

	Texture* m_pDefaultTexture;
	std::unordered_map<std::string, GLuint> m_TextureNameIdMap;
	std::unordered_map<GLuint, TextureInfo> m_Textures;
	SoundStream* m_pSoundStream;
	std::unordered_map<std::string, SoundEffect> m_SoundEffects;
};

