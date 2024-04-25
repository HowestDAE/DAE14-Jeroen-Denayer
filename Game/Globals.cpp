#include "pch.h"
#include "Globals.h"

std::string AssetManager::m_LevelsDataDir{"LevelsData/"};
std::string AssetManager::m_LevelScreensDataDir{"LevelScreensData/"};
std::string AssetManager::m_TextureDir{ "Textures/" };

AssetManager::Globals()
{
}

Globals& AssetManager::Get()
{
    static Globals instance;
    return instance;
}

std::string AssetManager::GetDir(Dir dir)
{
    Globals& g{ Get() };
    switch (dir)
    {
    case Dir::LevelData:
        return g.m_LevelsDataDir;
        break;
    case Dir::LevelScreenData:
        return g.m_LevelScreensDataDir;
        break;
    case Dir::Texture:
        return g.m_TextureDir;
        break;
    }
    return std::string();
}
