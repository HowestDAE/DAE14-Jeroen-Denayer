#pragma once
#include <fstream>
#include <functional>
#include "SoundEffect.h"
#include "SoundStream.h"

//Forward declarations
class Texture;

class FileIO final
{
public:
	static FileIO& Get();
	FileIO(const FileIO& other) = delete;
	FileIO& operator=(const FileIO& other) = delete;
	FileIO(FileIO&& other) = delete;
	FileIO& operator=(FileIO&& other) = delete;

	enum class Dir
	{
		LevelData, LevelScreenData, Texture, Font, Sound
	};

	static std::string GetDir(Dir dir);
	static std::ifstream OpenTxtFile(const std::string& name, Dir dir);
	static void CloseFile(std::ifstream& stream);
	static void LoadIntArr(std::ifstream& fStream, std::vector<int>& vec);
	static void LoadStringArr(std::ifstream& fStream, std::vector<std::string>& vec);
	static bool LoadTexture(const std::string& name, std::vector<uint8_t>& data, int& rows, int& cols);
	static bool LoadTexture(const std::string& name, Texture*& pTexture);
	static bool LoadSound(const std::string& name, SoundStream*& pSoundStream);
	static bool LoadSound(const std::string& name, SoundEffect*& pSoundEffect);
private:
	FileIO();

	std::vector<std::string> m_Dirs;
};

