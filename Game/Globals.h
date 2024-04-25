#pragma once
#include <string>

class Globals final
{
public:
	static Globals& Get();

	Globals(const Globals& other) = delete;
	Globals& operator=(const Globals& other) = delete;
	Globals(Globals&& other) = delete;
	Globals& operator=(Globals&& other) = delete;

	enum class Dir
	{
		LevelData, LevelScreenData, Texture
	};

	static std::string GetDir(Dir dir);
private:
	Globals();

	static std::string m_LevelsDataDir;
	static std::string m_LevelScreensDataDir;
	static std::string m_TextureDir;
};

