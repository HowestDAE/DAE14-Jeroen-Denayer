#include "pch.h"
#include "Game.h"
#include "Madeline.h"
#include "Camera.h"
#include "Level.h"
#include "GameData.h"

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
	m_pActiveLvl = new Level();

	GameData::SetGameData(vp.width, vp.height);
	GameData::SetActiveLevel(*m_pActiveLvl);
	
	float MADELINE_PIX_WIDTH{ 12 * GameData::RES_SCALE() };
	float MADELINE_PIX_HEIGHT{ 16 * GameData::RES_SCALE() };
	m_pMadeline = new Madeline(Point2f{ 150.f, 100.f }, MADELINE_PIX_WIDTH, MADELINE_PIX_HEIGHT);
}

void Game::Cleanup( )
{
	delete m_pMadeline;
	delete m_pActiveLvl;
	GameData::Cleanup();
}

void Game::Update( float elapsedSec )
{
	// Check keyboard state
	const Uint8* pKeyStates = SDL_GetKeyboardState(nullptr);
	std::vector<Madeline::Action> actions{};
	if (pKeyStates[SDL_SCANCODE_LEFT]) actions.push_back(Madeline::Action::MovingLeft);
	if (pKeyStates[SDL_SCANCODE_RIGHT]) actions.push_back(Madeline::Action::MovingRight);
	if (pKeyStates[SDL_SCANCODE_UP] || pKeyStates[SDL_SCANCODE_SPACE]) actions.push_back(Madeline::Action::Jumping);
	if (pKeyStates[SDL_SCANCODE_DOWN]) actions.push_back(Madeline::Action::MovingDown);
	m_pMadeline->Update(elapsedSec, actions);
}

void Game::Draw( ) const
{
	ClearBackground( );

	m_pActiveLvl->Draw();
	m_pMadeline->Draw();
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
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