#pragma once
#include <vector>
#include <unordered_map>

//Forward declarations
class Texture;

class AssetManager final
{
public:
	static AssetManager& Get();
	static void Init();
	static void ReleaseDynamicMemory();

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

	enum class Dir
	{
		LevelData, LevelScreenData, Texture
	};

	static std::string GetDir(Dir dir);
	static Texture* GetTexture(const std::string& name);
	static void RemoveTexture(Texture* pTexture);
private:
	AssetManager();

	static std::vector<std::string> s_Dirs;

	static Texture* s_pDefaultTexture;
	static std::unordered_map<std::string, GLuint> s_TextureNameIdMap;
	static std::unordered_map<GLuint, TextureInfo> s_Textures;
};

