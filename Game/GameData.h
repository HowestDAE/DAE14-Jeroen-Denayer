#pragma once

//Forward declaration
class Level;

//Singleton
class GameData
{
public:
	GameData(const GameData&) = delete; //Remove copy constructor
	void operator=(const GameData&) = delete; //remove assingment operator

	static GameData& Get();
	static void		Cleanup();
	static void		SetGameData(float screenWidth, float screenHeight);
	static void		SetActiveLevel(Level& level);

	//static float
	static float	MADELINE_JUMP_HEIGHT();
	static float	MADELINE_JUMP_TIME();
	static float 	G();
	static int		TILE_SIZE_PIX();
	static float	WINDOW_NUM_TILES_X();
	static float	WINDOW_NUM_TILES_Y();
	static float	SCREEN_WIDTH();
	static float	SCREEN_HEIGHT();
	static float	RENDER_RES_X();
	static float	RENDER_RES_Y();
	static float	RES_SCALE();
	static int		TILE_SIZE_PIX_SCALED();
	static int		PIX_PER_M();
	static const Level*	ActiveLvl();
private:
	GameData(); //private constructor

	static GameData s_Instance;

	float	m_MADELINE_JUMP_HEIGHT;
	float	m_MADELINE_JUMP_TIME;
	float	m_G;
	int		m_TILE_SIZE_PIX;
	float	m_WINDOW_NUM_TILES_X;
	float	m_WINDOW_NUM_TILES_Y;
	float	m_SCREEN_WIDTH;
	float	m_SCREEN_HEIGHT;
	float	m_RENDER_RES_X;
	float	m_RENDER_RES_Y;
	float	m_RES_SCALE;
	int		m_TILE_SIZE_PIX_SCALED;
	int		m_PIX_PER_M;
	Level* m_pActiveLvl;
};
