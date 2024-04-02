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

void Game::Initialize()
{
	m_LMBPressed = false;
	Rectf vp{ GetViewPort() };
	GameData::SetGameData(vp.width, vp.height);

	m_pActiveLvl = new Level();
	GameData::SetActiveLevel(*m_pActiveLvl);

	float madelinePixWidth{ 12 };
	float madelinePixHeight{ 16 };
	//Setting constants for madelines movement
	Madeline::MadelineData madelineData{};
	madelineData.groundJumpHeight = 3.5f;
	madelineData.groundJumpTime = 0.35f;
	AccAndVel madelineJumpAccAndVel{ utils::AccAndVelToTravelDistInTime(madelineData.groundJumpHeight, madelineData.groundJumpTime) };
	
	madelineData.runningSpeed = 12.f;
	madelineData.wallNeutralJumpDistX = 2.f;
	madelineData.wallNeutralJumpDistXTime = 0.15f;
	madelineData.wallJumpDistX = 3.5f;
	madelineData.wallJumpDistXTime = 0.35f;
	madelineData.maxFallSpeed = -30.f;
	madelineData.wallClimbingSpeed = 8.f;
	madelineData.wallSlidingSpeed = -12.f;
	madelineData.maxDistFromWallToWallJump = 2.f;
	//Calculate gravity so that madeline can move jumpHeight in exactly jumpTime
	GameData::SetGravity(-madelineJumpAccAndVel.acc);

	m_pMadeline = new Madeline(1, 6, madelinePixWidth, madelinePixHeight, madelineData);
}

void Game::Cleanup( )
{
	delete m_pMadeline;
	delete m_pActiveLvl;
	GameData::Cleanup();
}

void Game::Update( float elapsedSec )
{
	//Check inputs
	int dirX{}, dirY{};
	std::vector<Madeline::Action> actions{};

	//const Uint8* pKeyStates = SDL_GetKeyboardState(nullptr);
	//if (pKeyStates[SDL_SCANCODE_LEFT]) xAxis = -1.f;
	//if (pKeyStates[SDL_SCANCODE_RIGHT]) xAxis = 1.f;
	//if (pKeyStates[SDL_SCANCODE_DOWN]) yAxis = -1.f;
	//if (pKeyStates[SDL_SCANCODE_UP]) yAxis = 1.f;
	//if (pKeyStates[SDL_SCANCODE_SPACE] || pKeyStates[SDL_SCANCODE_UP]) actions.push_back(Madeline::Action::Jumping);

	float deadZoneX{ 0.4f };
	Sint16 xAxisSint16{ SDL_GameControllerGetAxis(m_SDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) };
	float xAxis{ float(xAxisSint16) / std::numeric_limits<Sint16>::max() };
	if (xAxis < -deadZoneX) dirX = -1.f;
	else if (xAxis > deadZoneX) dirX = 1.f;
	else dirX = 0.f;

	float deadZoneY{ 0.8f };
	Sint16 yAxisSint16{ SDL_GameControllerGetAxis(m_SDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) };
	float yAxis{ float(yAxisSint16) / std::numeric_limits<Sint16>::max() };
	if (yAxis < -deadZoneY) dirY = 1.f;
	else if (yAxis > deadZoneY) dirY = -1.f;
	else dirY = 0.f;

	if (SDL_GameControllerGetButton(m_SDLGameController, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A))
		actions.push_back(Madeline::Action::Jumping);
	if (SDL_GameControllerGetAxis(m_SDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERRIGHT))
	{
		float deadZoneRightTrigger{ 0.5f };
		Sint16 axisSint16{ SDL_GameControllerGetAxis(m_SDLGameController, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERRIGHT) };
		float axis{ float(axisSint16) / std::numeric_limits<Sint16>::max() };
		if (std::abs(axis) > deadZoneRightTrigger)
			actions.push_back(Madeline::Action::Grabbing);
	}
	Vector2i dir{ dirX, dirY};
		
	m_pMadeline->Update(elapsedSec, dir, actions);
}

void Game::Draw( ) const
{
	ClearBackground( );

	glPushMatrix();
	glScalef(GameData::RES_SCALE(), GameData::RES_SCALE(), 1);

	m_pActiveLvl->Draw();
	m_pMadeline->Draw();

	glPopMatrix();
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
	Point2f mousePos{ float(e.x), float(e.y) };
	if (m_LMBPressed)
		m_pMadeline->SetPosition(mousePos);
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