#pragma once
#include "BaseGame.h"
#include <vector>

//Forward declarations
class Madeline;
class Level;
class Camera;

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

	struct GameInfo
	{
		float SCREEN_WIDTH;
		float SCREEN_HEIGHT;
		float G;
		int	  TILE_SIZE_PIX;
		float WINDOW_NUM_TILES_X;
		float WINDOW_NUM_TILES_Y;
		float RENDER_RES_X;
		float RENDER_RES_Y;
		float RES_SCALE;
		int   TILE_SIZE_PIX_SCALED;
		int   PIX_PER_M;
		Level* activeLvl;
	};

private:
	//Functions
	void Initialize();
	void Cleanup( );
	void ClearBackground( ) const;

	//Const Members

	//Members
	GameInfo m_GameInfo;
	Level* m_ActiveLvl;
	Madeline* m_Madeline;
};