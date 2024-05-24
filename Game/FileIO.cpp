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
	std::stringstream path;
	path << FileIO::GetDir(FileIO::Dir::LevelScreenData) << name << ".txt";
	std::ifstream stream{ path.str()};
	if (!stream)
		std::cout << "FileIO::OpenTxtFile(): Couldn't open file: " << path.str() << std::endl;
	return stream;
}

std::ofstream FileIO::WriteTxtFile(const std::string& name, Dir dir)
{
	std::stringstream path;
	path << FileIO::GetDir(FileIO::Dir::LevelScreenData) << name << ".txt";
	std::ofstream stream{ path.str() };
	if (!stream)
		std::cout << "FileIO::OpenTxtFile(): Couldn't open file: " << path.str() << std::endl;
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

void FileIO::WriteIntArr(std::stringstream& sStream, std::vector<int>& vec)
{
	for (int i{}; i < vec.size(); ++i)
	{
		if (i != 0)
			sStream << " ";
		sStream << vec[i];
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

void FileIO::WriteStringArr(std::stringstream& sStream, std::vector<std::string>& vec)
{
	for (int i{}; i < vec.size(); ++i)
	{
		if (i != 0)
			sStream << " ";
		sStream << vec[i];
	}
}

void FileIO::LoadVector2fArr(std::ifstream& fStream, std::vector<Vector2f>& vec)
{
	std::string line{};
	std::getline(fStream, line);
	std::stringstream sstream{ line };
	std::string s{};
	sstream >> s; //extract name
	while (sstream.rdbuf()->in_avail()) //nr characters it can still read
	{
		sstream >> s;
		std::stringstream posStream{ s }; //put Vector2f in separate stream
		Vector2f pos{};
		std::string value{};
		std::getline(posStream, value, ',');
		pos.x = std::stof(value);
		std::getline(posStream, value, ',');
		pos.y = std::stof(value);
		vec.push_back(pos);
	}
}

void FileIO::WriteVector2fArr(std::stringstream& sStream, std::vector<Vector2f>& vec)
{
	for (int i{}; i < vec.size(); ++i)
	{
		if (i != 0)
			sStream << " ";
		sStream << vec[i].x << "," << vec[i].y;
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
