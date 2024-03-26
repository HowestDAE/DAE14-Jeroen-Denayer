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
	, m_RES_SCALE{}
	, m_TILE_SIZE_PIX_SCALED{}
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
	gameData.m_MADELINE_JUMP_HEIGHT = 3.5f; //3.5
	gameData.m_MADELINE_JUMP_TIME	= 0.35f; //Time to reach max jump height
	//gameData.m_G					= -9.81f;
	//Calculate gravity based on madeline's jump height and time to reach that jump height
	gameData.m_G					= -2 * gameData.m_MADELINE_JUMP_HEIGHT / (gameData.m_MADELINE_JUMP_TIME * gameData.m_MADELINE_JUMP_TIME);
	gameData.m_TILE_SIZE_PIX		= 8;
	gameData.m_WINDOW_NUM_TILES_X	= 40.f;
	gameData.m_WINDOW_NUM_TILES_Y	= 22.5f;
    gameData.m_SCREEN_WIDTH			= screenWidth;
    gameData.m_SCREEN_HEIGHT		= screenHeight;
	gameData.m_RENDER_RES_X			= gameData.m_TILE_SIZE_PIX * gameData.m_WINDOW_NUM_TILES_X;
	gameData.m_RENDER_RES_Y			= gameData.m_TILE_SIZE_PIX * gameData.m_WINDOW_NUM_TILES_Y;
	gameData.m_RES_SCALE			= gameData.m_SCREEN_WIDTH / gameData.m_RENDER_RES_X;
	gameData.m_TILE_SIZE_PIX_SCALED = gameData.m_TILE_SIZE_PIX * int(gameData.m_RES_SCALE);
	gameData.m_PIX_PER_M			= gameData.m_TILE_SIZE_PIX_SCALED;
}

void GameData::SetActiveLevel(Level& level)
{
	GameData& gameData{ Get() };
	gameData.m_pActiveLvl = &level;
}

float GameData::MADELINE_JUMP_HEIGHT() { return Get().m_MADELINE_JUMP_HEIGHT; }

float GameData::MADELINE_JUMP_TIME() { return Get().m_MADELINE_JUMP_TIME; }

float GameData::G() { return Get().m_G; }

int GameData::TILE_SIZE_PIX() { return Get().m_TILE_SIZE_PIX; }

float GameData::WINDOW_NUM_TILES_X() { return Get().m_WINDOW_NUM_TILES_X; }

float GameData::WINDOW_NUM_TILES_Y() { return Get().m_WINDOW_NUM_TILES_Y; }

float GameData::SCREEN_WIDTH() { return Get().m_SCREEN_WIDTH; }

float GameData::SCREEN_HEIGHT() { return Get().m_SCREEN_HEIGHT; }

float GameData::RENDER_RES_X() { return Get().m_RENDER_RES_X; }

float GameData::RENDER_RES_Y() { return Get().m_RENDER_RES_Y; }

float GameData::RES_SCALE() { return Get().m_RES_SCALE; }

int GameData::TILE_SIZE_PIX_SCALED() { return Get().m_TILE_SIZE_PIX_SCALED; }

int GameData::PIX_PER_M() { return Get().m_PIX_PER_M; }

const Level* GameData::ActiveLvl() { return Get().m_pActiveLvl; }
