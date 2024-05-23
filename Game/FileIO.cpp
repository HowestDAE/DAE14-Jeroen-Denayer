#include "pch.h"
#include "FileIO.h"
#include <sstream>
#include "AssetManager.h"
#include <filesystem>
#include "Texture.h"

FileIO::FileIO()
	: m_Dirs{ std::vector <std::string>{ "LevelsData/", "LevelScreensData/", "Textures/", "Fonts/", "Sound/" }}
{
}

FileIO& FileIO::Get()
{
    static FileIO instance;
    return instance;
}

std::string FileIO::GetDir(Dir dir)
{
	return Get().m_Dirs[int(dir)];
}

std::ifstream FileIO::OpenTxtFile(const std::string& name, Dir dir)
{
	const std::string& dirName{ FileIO::GetDir(FileIO::Dir::LevelScreenData) };
	std::string filePath{ dirName + name + ".txt" };
	std::ifstream stream{ filePath };
	if (!stream)
		std::cout << "FileIO::OpenTxtFile(): Couldn't open file: " << filePath << std::endl;
	return stream;
}

void FileIO::CloseFile(std::ifstream& stream)
{
	stream.close();
}

void FileIO::LoadIntArr(std::ifstream& fStream, std::vector<int>& vec)
{
	std::string line{};
	std::getline(fStream, line);
	std::stringstream sstream{ line };
	std::string name{};
	sstream >> name; //extract name
	int value{};
	while (sstream.rdbuf()->in_avail()) //nr characters it can still read
	{
		sstream >> value;
		vec.push_back(value);
	}
}

void FileIO::LoadStringArr(std::ifstream& fStream, std::vector<std::string>& vec)
{
	std::string line{};
	std::getline(fStream, line);
	std::stringstream sstream{ line };
	std::string s{};
	sstream >> s; //extract name
	while (sstream.rdbuf()->in_avail()) //nr characters it can still read
	{
		sstream >> s;
		vec.push_back(s);
	}
}

bool FileIO::LoadTexture(const std::string& name, std::vector<uint8_t>& data, int& rows, int& cols)
{
	bool succes{ false };
	const std::string& dir{ GetDir(Dir::LevelScreenData) };
	std::string path{ dir + name + ".bmp" };
	SDL_Surface* pSurface{ IMG_Load(path.c_str()) };
	if (pSurface)
	{
		succes = true;
		rows = pSurface->h;
		cols = pSurface->w;
		//Reserve only necessary space for level
		data = std::vector<uint8_t>(rows * cols);

		const Uint8 Bpp = pSurface->format->BytesPerPixel;
		for (int i{}; i < rows * cols; ++i)
		{
			TileIdx tileIdx{ utils::GetTileIdxFromIdx(i, rows, cols) };
			Uint8* pPixel = (Uint8*)pSurface->pixels + tileIdx.r * pSurface->pitch + tileIdx.c * Bpp;
			TileIdx dstTileIdx{rows - 1 - tileIdx.r, tileIdx.c};
			int dstIdx{ utils::GetIdxFromTileIdx(dstTileIdx, rows, cols) };
			data[dstIdx] = *pPixel; //rows - 1 - i to flip vertically
		}
	}
	return succes;
}

bool FileIO::LoadTexture(const std::string& name, Texture*& pTexture)
{
	bool succes{ false };
	std::stringstream path{};
	path << FileIO::GetDir(FileIO::Dir::Texture) << name << ".png";
	if (!std::filesystem::exists(path.str()))
	{
		std::cout << "FileIO::LoadTexture(): Couldn't find texture on disk: " << path.str() << std::endl;
	}
	else
	{
		if (pTexture)
			std::cout << "FileIO::LoadTexture(): pTexture already points to something" << std::endl;
		else
		{
			pTexture = new Texture(path.str());
			succes = true;
		}
	}
	return succes;
}

bool FileIO::LoadSound(const std::string& name, SoundStream*& pSoundStream)
{
	bool succes{ false };
	std::stringstream path{};
	path << FileIO::GetDir(FileIO::Dir::Sound) << name << ".mp3";
	if (!std::filesystem::exists(path.str()))
	{
		std::cout << "FileIO::LoadSound(): Couldn't find sound on disk: " << path.str() << std::endl;
	}
	else
	{
		pSoundStream = new SoundStream(path.str());
		succes = true;
	}
	return succes;
}

bool FileIO::LoadSound(const std::string& name, SoundEffect*& pSoundEffect)
{
	bool succes{ false };
	std::stringstream path{};
	path << FileIO::GetDir(FileIO::Dir::Sound) << name << ".wav";
	if (!std::filesystem::exists(path.str()))
	{
		std::cout << "FileIO::LoadSound(): Couldn't find sound on disk: " << path.str() << std::endl;
	}
	else
	{
		pSoundEffect = new SoundEffect(path.str());
		succes = true;
	}
	return succes;
}
