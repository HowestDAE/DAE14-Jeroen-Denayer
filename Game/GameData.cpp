#include "pch.h"
#include "GameData.h"

GameData GameData::s_Instance;

GameData::GameData()
	: m_SCREEN_WIDTH{}
	, m_SCREEN_HEIGHT{}
	, m_G{}
	, m_TILE_SIZE_PIX{}
	, m_WINDOW_NUM_TILES_X{}
	, m_WINDOW_NUM_TILES_Y{}
	, m_RENDER_RES_X{}
	, m_RENDER_RES_Y{}
	, m_RES_SCALE_X{}
	, m_RES_SCALE_Y{}
	, m_PIX_PER_M{}
	, m_pActiveLvl{}
{
}

GameData& GameData::Get()
{
    return s_Instance;
}

void GameData::Cleanup()
{
	GameData& gameData{ Get() };
	//DON'T DELETE m_pActiveLvl here
	//m_pActiveLvl is deleted in game class
	gameData.m_pActiveLvl = nullptr;
}

void GameData::SetGameData(float screenWidth, float screenHeight)
{
	GameData& gameData{ Get() };
	gameData.m_G					= -9.81f;
	gameData.m_TILE_SIZE_PIX		= 8;
	gameData.m_WINDOW_NUM_TILES_X	= 40.f;
	gameData.m_WINDOW_NUM_TILES_Y	= 22.5f;
    gameData.m_SCREEN_WIDTH			= screenWidth;
    gameData.m_SCREEN_HEIGHT		= screenHeight;
	gameData.m_RENDER_RES_X			= gameData.m_TILE_SIZE_PIX * gameData.m_WINDOW_NUM_TILES_X;
	gameData.m_RENDER_RES_Y			= gameData.m_TILE_SIZE_PIX * gameData.m_WINDOW_NUM_TILES_Y;
	gameData.m_RES_SCALE_X			= gameData.m_SCREEN_WIDTH / gameData.m_RENDER_RES_X;
	gameData.m_RES_SCALE_Y			= gameData.m_SCREEN_HEIGHT / gameData.m_RENDER_RES_Y;
	gameData.m_PIX_PER_M			= gameData.m_TILE_SIZE_PIX;
}

void GameData::SetActiveLevel(Level& level)
{
	Get().m_pActiveLvl = &level;
}

void GameData::SetGravity(float g)
{
	Get().m_G = g;
}

float GameData::G() { return Get().m_G; }

int GameData::TILE_SIZE_PIX() { return Get().m_TILE_SIZE_PIX; }

float GameData::WINDOW_NUM_TILES_X() { return Get().m_WINDOW_NUM_TILES_X; }

float GameData::WINDOW_NUM_TILES_Y() { return Get().m_WINDOW_NUM_TILES_Y; }

float GameData::SCREEN_WIDTH() { return Get().m_SCREEN_WIDTH; }

float GameData::SCREEN_HEIGHT() { return Get().m_SCREEN_HEIGHT; }

float GameData::RENDER_RES_X() { return Get().m_RENDER_RES_X; }

float GameData::RENDER_RES_Y() { return Get().m_RENDER_RES_Y; }

float GameData::RES_SCALE_X() { return Get().m_RES_SCALE_X; }

float GameData::RES_SCALE_Y() { return Get().m_RES_SCALE_Y; }

int GameData::PIX_PER_M() { return Get().m_PIX_PER_M; }

const Level* GameData::ActiveLvl() { return Get().m_pActiveLvl; }
