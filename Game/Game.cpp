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

void Game::Initialize()
{
	m_LMBPressed = false;
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

	m_pActiveLvl = new Level();
	m_pCamera = new Camera(m_GameData.RENDER_RES_X, m_GameData.RENDER_RES_Y);

	Point2f pos{ 8 * 8, 2 * 8 };
	m_pMadeline = new Madeline(pos, madelinePixWidth, madelinePixHeight, m_pActiveLvl);
}

void Game::Cleanup( )
{
	delete m_pCamera;
	delete m_pMadeline;
	delete m_pActiveLvl;
}

void Game::Update( float elapsedSec )
{
	//const Uint8* pKeyStates = SDL_GetKeyboardState(nullptr);
	//if (pKeyStates[SDL_SCANCODE_LEFT]) xAxis = -1.f;
	//if (pKeyStates[SDL_SCANCODE_RIGHT]) xAxis = 1.f;
	//if (pKeyStates[SDL_SCANCODE_DOWN]) yAxis = -1.f;
	//if (pKeyStates[SDL_SCANCODE_UP]) yAxis = 1.f;
	//if (pKeyStates[SDL_SCANCODE_SPACE] || pKeyStates[SDL_SCANCODE_UP]) actions.push_back(Madeline::Action::Jumping);

	//Check inputs
	InputActions input{};

	float deadZoneX{ 0.4f };
	Sint16 xAxisSint16{ SDL_GameControllerGetAxis(m_SDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) };
	float xAxis{ float(xAxisSint16) / std::numeric_limits<Sint16>::max() };
	if (xAxis < -deadZoneX) input.dir.x = -1.f;
	else if (xAxis > deadZoneX) input.dir.x = 1.f;

	float deadZoneY{ 0.8f };
	Sint16 yAxisSint16{ SDL_GameControllerGetAxis(m_SDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) };
	float yAxis{ float(yAxisSint16) / std::numeric_limits<Sint16>::max() };
	if (yAxis < -deadZoneY) input.dir.y = 1.f;
	else if (yAxis > deadZoneY) input.dir.y = -1.f;

	if (SDL_GameControllerGetButton(m_SDLGameController, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A))
		input.jumping = true;
	if (SDL_GameControllerGetAxis(m_SDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERRIGHT))
	{
		float deadZoneRightTrigger{ 0.5f };
		Sint16 axisSint16{ SDL_GameControllerGetAxis(m_SDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERRIGHT) };
		float axis{ float(axisSint16) / std::numeric_limits<Sint16>::max() };
		if (std::abs(axis) > deadZoneRightTrigger)
			input.grabbing = true;
	}
	if (SDL_GameControllerGetButton(m_SDLGameController, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER))
		input.dashing = true;
		
	m_pMadeline->Update(elapsedSec, input);
	Point2f pos{ m_pMadeline->GetPosition() };
}

void Game::Draw( ) const
{
	ClearBackground( );
	//Upscale to screen resolution
	glPushMatrix();
	glScalef(m_GameData.RES_SCALE_X, m_GameData.RES_SCALE_Y, 1);

	m_pCamera->Aim(m_pActiveLvl->GetWidth(), m_pActiveLvl->GetHeight(), m_pMadeline->GetPosition());

	m_pActiveLvl->Draw();
	m_pMadeline->Draw();

	m_pCamera->Reset();
	glPopMatrix(); //Release upscaling matrix

	//Debug lines
	utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
	float x{ m_GameData.RES_SCALE_X * m_GameData.RENDER_RES_X };
	float y{ m_GameData.RES_SCALE_Y * m_GameData.RENDER_RES_Y };
	utils::DrawLine(Point2f{ x, 0.f }, Point2f{ x, y }, 3.f);
	utils::DrawLine(Point2f{ 0.f, y }, Point2f{ x, y }, 3.f);
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