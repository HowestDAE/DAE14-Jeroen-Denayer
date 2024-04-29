#include "pch.h"
#include "Game.h"
#include "LevelEditor.h"
#include "Level.h"
#include "InputManager.h"
#include "AssetManager.h"
#include "GameData.h"
#include "Transform.h"

Game::Game( const Window& window ) 
	: BaseGame{ window }
	, m_Mode{ Mode::PlayLevel }
	, m_pActiveLvl{ nullptr }
	, m_pLevelEditor{ nullptr }
{
	Rectf viewport{ GetViewPort() };

	GameData::Init(viewport);
	InputManager::Init(m_SDLGameController);

	switch (m_Mode)
	{
	case Mode::PlayLevel:
		m_pActiveLvl = new Level();
		break;
	case Mode::RunEditor:
		m_pLevelEditor = new LevelEditor();
		break;
	}
}

Game::~Game( )
{
	delete m_pLevelEditor;
	delete m_pActiveLvl;
	AssetManager::Cleanup();
}

void Game::Update( float elapsedSec )
{
	//const Uint8* pKeyStates = SDL_GetKeyboardState(nullptr);
	//if (pKeyStates[SDL_SCANCODE_LEFT]) xAxis = -1.f;
	//if (pKeyStates[SDL_SCANCODE_RIGHT]) xAxis = 1.f;
	//if (pKeyStates[SDL_SCANCODE_DOWN]) yAxis = -1.f;
	//if (pKeyStates[SDL_SCANCODE_UP]) yAxis = 1.f;
	//if (pKeyStates[SDL_SCANCODE_SPACE] || pKeyStates[SDL_SCANCODE_UP]) actions.push_back(Madeline::Action::Jumping);
	
	InputManager::Update();

	switch (m_Mode)
	{
	case Mode::PlayLevel:
		m_pActiveLvl->Update(elapsedSec);
		break;
	case Mode::RunEditor:
		m_pLevelEditor->Update(elapsedSec);
		break;
	}

	InputManager::Reset();
}

void Game::Draw( ) const
{
	switch (m_Mode)
	{
	case Mode::PlayLevel:
		m_pActiveLvl->Draw();
		break;
	case Mode::RunEditor:
		m_pLevelEditor->Draw();
		break;
	}

	Rectf vp{ GetViewPort() };

	utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
	utils::DrawLine(Point2f{ vp.width / 2, 0.f }, Point2f{ vp.width / 2, vp.height }, 2.f);
	utils::DrawLine(Point2f{ 0.f, vp.height / 2 }, Point2f{ vp.width, vp.height / 2 }, 2.f);
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	InputManager::ProcessKeyDownEvent(e);
}

void Game::ProcessKeyUpEvent( const SDL_KeyboardEvent& e )
{
	InputManager::ProcessKeyUpEvent(e);
}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
	InputManager::ProcessMouseMotionEvent(e);
}

void Game::ProcessMouseDownEvent( const SDL_MouseButtonEvent& e )
{
	InputManager::ProcessMouseDownEvent(e);
}

void Game::ProcessMouseUpEvent( const SDL_MouseButtonEvent& e )
{
	InputManager::ProcessMouseUpEvent(e);
}

void Game::ProcessMouseWheelEvent(int direction)
{
	InputManager::ProcessMouseWheelEvent(direction);
}