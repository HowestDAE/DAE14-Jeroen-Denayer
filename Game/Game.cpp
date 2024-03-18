#include "pch.h"
#include "Game.h"
#include "Madeline.h"
#include "Camera.h"
#include "Level.h"

Game::Game( const Window& window ) 
	:BaseGame{ window }
{
	Initialize();
}

Game::~Game( )
{
	Cleanup( );
}

void Game::Initialize( )
{
	Rectf vp{ GetViewPort() };
	m_ActiveLvl = new Level();
	m_GameInfo.SCREEN_WIDTH				= vp.width;
	m_GameInfo.SCREEN_HEIGHT			= vp.height;
	m_GameInfo.G						= -9.81f;
	m_GameInfo.TILE_SIZE_PIX			= 8;
	m_GameInfo.WINDOW_NUM_TILES_X		= 40.f;
	m_GameInfo.WINDOW_NUM_TILES_Y		= 22.5f;
	m_GameInfo.RENDER_RES_X				= m_GameInfo.TILE_SIZE_PIX * m_GameInfo.WINDOW_NUM_TILES_X;
	m_GameInfo.RENDER_RES_Y				= m_GameInfo.TILE_SIZE_PIX * m_GameInfo.WINDOW_NUM_TILES_Y;
	m_GameInfo.RES_SCALE				= m_GameInfo.SCREEN_WIDTH	/ m_GameInfo.RENDER_RES_X;
	m_GameInfo.TILE_SIZE_PIX_SCALED		= m_GameInfo.TILE_SIZE_PIX * int(m_GameInfo.RES_SCALE);
	m_GameInfo.PIX_PER_M				= m_GameInfo.TILE_SIZE_PIX * m_GameInfo.RES_SCALE;
	m_GameInfo.activeLvl				= m_ActiveLvl;
	
	const int MADELINE_PIX_HEIGHT{ 24 };
	const int MADELINE_PIX_WIDTH{ 13 };
	m_Madeline = new Madeline(Point2f{ 150.f, 100.f }, MADELINE_PIX_WIDTH * m_GameInfo.RES_SCALE, MADELINE_PIX_HEIGHT * m_GameInfo.RES_SCALE);
}

void Game::Cleanup( )
{
	delete m_Madeline;
}

void Game::Update( float elapsedSec )
{
	// Check keyboard state
	const Uint8* pStates = SDL_GetKeyboardState(nullptr);
	if (pStates[SDL_SCANCODE_LEFT])
		m_Madeline->Move(elapsedSec, Madeline::Direction::Left);
	if (pStates[SDL_SCANCODE_RIGHT])
		m_Madeline->Move(elapsedSec, Madeline::Direction::Right);
	if (pStates[SDL_SCANCODE_UP])
		m_Madeline->Move(elapsedSec, Madeline::Direction::Up);
	if (pStates[SDL_SCANCODE_DOWN])
		m_Madeline->Move(elapsedSec, Madeline::Direction::Down);

	m_Madeline->Update(elapsedSec, m_GameInfo);
}

void Game::Draw( ) const
{
	ClearBackground( );

	m_ActiveLvl->Draw(m_GameInfo);
	m_Madeline->Draw(m_GameInfo);
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
	switch (e.keysym.sym)
	{
	case SDLK_SPACE:
		m_Madeline->Jump();
		break;
	}
}

void Game::ProcessKeyUpEvent( const SDL_KeyboardEvent& e )
{
	//std::cout << "KEYUP event: " << e.keysym.sym << std::endl;
	//switch (e.keysym.sym)
	//{
	//case SDLK_LEFT: case SDLK_RIGHT : case SDLK_UP: case SDLK_DOWN:
	//	
	//	break;
	//}
}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
}

void Game::ProcessMouseDownEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONDOWN event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
	
}

void Game::ProcessMouseUpEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONUP event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
}

void Game::ClearBackground( ) const
{
	glClearColor( 0.0f, 0.0f, 0.3f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}