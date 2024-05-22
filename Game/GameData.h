#pragma once

//Forward declaration
class Level;

//Singleton
class GameData final
{
public:
	enum class Mode
	{
		Menu, PlayLevel, RunEditor
	};

	static GameData& Get();
	static void	Init(const Rectf& viewport, Mode mode = Mode::Menu);
	static void SetMode(Mode mode);

	GameData(const GameData& other) = delete;
	void operator=(const GameData& other) = delete;
	GameData(GameData&& other) = delete;
	void operator=(GameData&& other) = delete;

	static Mode		GetMode();
	static Rectf	VIEWPORT();
	static int		TILE_SIZE_PIX();
	static float	WINDOW_NUM_TILES_X();
	static float	WINDOW_NUM_TILES_Y();
	static float	SCREEN_WIDTH();
	static float	SCREEN_HEIGHT();
	static float	RENDER_RES_X();
	static float	RENDER_RES_Y();
	static float	RES_SCALE_X();
	static float	RES_SCALE_Y();
	static int		PIX_PER_M();
private:
	GameData();

	Mode	m_Mode;
	Rectf	m_Viewport;
	int		m_TILE_SIZE_PIX;
	float	m_WINDOW_NUM_TILES_X;
	float	m_WINDOW_NUM_TILES_Y;
	float	m_SCREEN_WIDTH;
	float	m_SCREEN_HEIGHT;
	float	m_RENDER_RES_X;
	float	m_RENDER_RES_Y;
	float	m_RES_SCALE_X;
	float	m_RES_SCALE_Y;
	int		m_PIX_PER_M;
};
