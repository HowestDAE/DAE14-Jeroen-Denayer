#pragma once
#include <vector>
#include <unordered_map>

//Forward declarations
class Texture;

class AssetManager final
{
public:
	static AssetManager& Get();
	static void Cleanup();

	AssetManager(const AssetManager& other) = delete;
	AssetManager& operator=(const AssetManager& other) = delete;

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

	static Texture* GetTexture(const std::string& name);
	static void GetTextures(const std::vector<std::string>& textureNames, std::vector<Texture*>& textureArr);
	static void RemoveTexture(Texture* pTexture);
private:
	AssetManager();
	Texture* IGetTexture(const std::string& name);
	void IRemoveTexture(Texture* pTexture);

	Texture* m_pDefaultTexture;
	std::unordered_map<std::string, GLuint> m_TextureNameIdMap;
	std::unordered_map<GLuint, TextureInfo> m_Textures;
};

