#include "pch.h"
#include "Game.h"
#include "Camera.h"
#include "Level.h"
#include "InputManager.h"
#include "Test.h"

Game::Game( const Window& window ) 
	: BaseGame{ window }
	, m_GameData{}
	, m_pInputManager{ new InputManager(m_SDLGameController) }
	, m_LMBPressed{ false }
{
	Rectf vp{ GetViewPort() };

	//Setting Madeline parameters
	float madelinePixWidth{ 8 };
	float madelinePixHeight{ 16 };
	AccAndVel madelineJumpAccAndVel{ utils::AccAndVelToTravelDistInTime(3.5f, 0.35f) };

	//Setting game parameters
	m_GameData.G = -madelineJumpAccAndVel.acc;
	m_GameData.TILE_SIZE_PIX = 8;
	m_GameData.WINDOW_NUM_TILES_X = 40.f;
	m_GameData.WINDOW_NUM_TILES_Y = 22.5f;
	m_GameData.SCREEN_WIDTH = vp.width;
	m_GameData.SCREEN_HEIGHT = vp.height;
	m_GameData.RENDER_RES_X = m_GameData.TILE_SIZE_PIX * m_GameData.WINDOW_NUM_TILES_X;
	m_GameData.RENDER_RES_Y = m_GameData.TILE_SIZE_PIX * m_GameData.WINDOW_NUM_TILES_Y;
	m_GameData.RES_SCALE_X = m_GameData.SCREEN_WIDTH / m_GameData.RENDER_RES_X;
	m_GameData.RES_SCALE_Y = m_GameData.SCREEN_HEIGHT / m_GameData.RENDER_RES_Y;
	m_GameData.PIX_PER_M = m_GameData.TILE_SIZE_PIX;

	m_pCamera = new Camera(m_GameData.RENDER_RES_X, m_GameData.RENDER_RES_Y);
	m_pActiveLvl = new Level(m_pInputManager, m_pCamera);
}

Game::~Game( )
{
	delete m_pCamera;
	delete m_pActiveLvl;
	delete m_pInputManager;
}

void Game::Update( float elapsedSec )
{
	//const Uint8* pKeyStates = SDL_GetKeyboardState(nullptr);
	//if (pKeyStates[SDL_SCANCODE_LEFT]) xAxis = -1.f;
	//if (pKeyStates[SDL_SCANCODE_RIGHT]) xAxis = 1.f;
	//if (pKeyStates[SDL_SCANCODE_DOWN]) yAxis = -1.f;
	//if (pKeyStates[SDL_SCANCODE_UP]) yAxis = 1.f;
	//if (pKeyStates[SDL_SCANCODE_SPACE] || pKeyStates[SDL_SCANCODE_UP]) actions.push_back(Madeline::Action::Jumping);
	m_pInputManager->Update();
	m_pActiveLvl->Update(elapsedSec);
}

void Game::Draw( ) const
{
	ClearBackground();
	//Upscale to screen resolution
	glPushMatrix();
	glScalef(m_GameData.RES_SCALE_X, m_GameData.RES_SCALE_Y, 1);

	m_pActiveLvl->Draw();

	m_pCamera->Reset();
	glPopMatrix(); //Release upscaling matrix
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
	switch ( e.button )
	{
	case SDL_BUTTON_LEFT:
		m_LMBPressed = true;
		break;
	}
}

void Game::ProcessMouseUpEvent( const SDL_MouseButtonEvent& e )
{
	switch ( e.button )
	{
	case SDL_BUTTON_LEFT:
		m_LMBPressed = false;
		break;
	}
}

void Game::ClearBackground( ) const
{
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}