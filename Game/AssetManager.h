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

	enum class Dir
	{
		LevelData, LevelScreenData, Texture
	};

	static std::string GetDir(Dir dir);
	static Texture* GetTexture(const std::string& name);
	static void RemoveTexture(Texture* pTexture);
private:
	AssetManager();
	Texture* IGetTexture(const std::string& name);
	void IRemoveTexture(Texture* pTexture);

	std::vector<std::string> m_Dirs;
	Texture* m_pDefaultTexture;
	std::unordered_map<std::string, GLuint> m_TextureNameIdMap;
	std::unordered_map<GLuint, TextureInfo> m_Textures;
};

