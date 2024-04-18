#pragma once
#include "BaseGame.h"
#include <vector>

//Forward declarations
class Level;
class Camera;
class InputManager;

class Game final: public BaseGame
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	// http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-override
	~Game();

	void Update( float elapsedSec ) override;
	void Draw( ) const override;

	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e ) override;
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e ) override;
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e ) override;

	struct GameData
	{
		float	G;
		int		TILE_SIZE_PIX;
		float	WINDOW_NUM_TILES_X;
		float	WINDOW_NUM_TILES_Y;
		float	SCREEN_WIDTH;
		float	SCREEN_HEIGHT;
		float	RENDER_RES_X;
		float	RENDER_RES_Y;
		float	RES_SCALE_X;
		float	RES_SCALE_Y;
		int		PIX_PER_M;
	};
private:
	//Functions
	void ClearBackground( ) const;

	//Members
	GameData m_GameData;
	Camera* m_pCamera;
	Level* m_pActiveLvl;
	InputManager* m_pInputManager;

	bool m_LMBPressed;
};