#include "pch.h"
#include "Game.h"
#include "LevelEditor.h"
#include "Level.h"
#include "InputManager.h"
#include "AssetManager.h"
#include "GameData.h"
#include "Button.h"
#include <functional>
#include "SoundStream.h"

Game::Game( const Window& window ) 
	: BaseGame{ window }
	, m_RunGame{ true }
	, m_MainMenu{ UIPanel("MainMenuBg") }
	, m_pActiveLvl{ nullptr }
	, m_pLevelEditor{ nullptr }
	, m_pMusic{ nullptr }
{
	Rectf viewport{ GetViewPort() };

	CreateMainMenu();
	GameData::Init(viewport, GameData::Mode::Menu);

	switch (GameData::GetMode())
	{
	case GameData::Mode::Menu:
		GoToMainMenu();
		break;
	case GameData::Mode::PlayLevel:
		Play();
		break;
	case GameData::Mode::RunEditor:
		RunEditor();
		break;
	}

	BindInputEvents();
}

Game::~Game( )
{
	if (m_pLevelEditor)
		delete m_pLevelEditor;
	if (m_pActiveLvl)
		delete m_pActiveLvl;

	delete m_pMusic;
}

bool Game::Update( float elapsedSec )
{
	//const Uint8* pKeyStates = SDL_GetKeyboardState(nullptr);
	//if (pKeyStates[SDL_SCANCODE_LEFT]) xAxis = -1.f;
	//if (pKeyStates[SDL_SCANCODE_RIGHT]) xAxis = 1.f;
	//if (pKeyStates[SDL_SCANCODE_DOWN]) yAxis = -1.f;
	//if (pKeyStates[SDL_SCANCODE_UP]) yAxis = 1.f;
	//if (pKeyStates[SDL_SCANCODE_SPACE] || pKeyStates[SDL_SCANCODE_UP]) actions.push_back(Madeline::Action::Jumping);
	
	InputManager::Update();

	switch (GameData::GetMode())
	{
	case GameData::Mode::Menu:
		m_MainMenu.Update(elapsedSec);
		break;
	case GameData::Mode::PlayLevel:
		m_pActiveLvl->Update(elapsedSec);
		break;
	case GameData::Mode::RunEditor:
		bool update = m_pLevelEditor->Update(elapsedSec);
		if (!update)
			GoToMainMenu();
		break;
	}

	InputManager::Reset();

	return m_RunGame;
}

void Game::Draw( ) const
{
	switch (GameData::GetMode())
	{
	case GameData::Mode::Menu:
		m_MainMenu.Draw();
		break;
	case GameData::Mode::PlayLevel:
		m_pActiveLvl->Draw();
		break;
	case GameData::Mode::RunEditor:
		m_pLevelEditor->Draw();
		break;
	}

	////Draw center lines
	//Rectf vp{ GetViewPort() };
	//utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
	//utils::DrawLine(Point2f{ vp.width / 2, 0.f }, Point2f{ vp.width / 2, vp.height }, 2.f);
	//utils::DrawLine(Point2f{ 0.f, vp.height / 2 }, Point2f{ vp.width, vp.height / 2 }, 2.f);
}

bool Game::HandleEvent(SDL_Event& e) const
{
	return InputManager::HandleEvent(e);
}

void Game::Play()
{
	m_pActiveLvl = new Level();
	if (!m_pActiveLvl->IsValid())
	{
		delete m_pActiveLvl;
		m_pActiveLvl = nullptr;
	}
	else
		GameData::SetMode(GameData::Mode::PlayLevel);

	AssetManager::PlaySoundStream("ThemeReflection", 0.7f);
}

void Game::RunEditor()
{
	m_pLevelEditor = new LevelEditor();
	GameData::SetMode(GameData::Mode::RunEditor);
}

void Game::Quit()
{
	m_RunGame = false;
}

void Game::GoToMainMenu()
{
	if (m_pActiveLvl)
	{
		delete m_pActiveLvl;
		m_pActiveLvl = nullptr;
	}
	if (m_pLevelEditor)
	{
		delete m_pLevelEditor;
		m_pLevelEditor = nullptr;
	}

	AssetManager::PlaySoundStream("ThemeMainMenu");
	GameData::SetMode(GameData::Mode::Menu);
	m_MainMenu.MovingLMB();
}

void Game::CreateMainMenu()
{
	std::function<void(void)> fPlay = std::bind(&Game::Play, this);
	Button playButton("Play", Vector2f{ 100.f, 400.f }, 50.f, fPlay);
	playButton.SetLogo("Play");
	m_MainMenu.AddButton(std::move(playButton));

	std::function<void(void)> fEditor = std::bind(&Game::RunEditor, this);
	Button editorButton("Editor", Vector2f{ 100.f, 330.f }, 50.f, fEditor);
	editorButton.SetLogo("Editor");
	m_MainMenu.AddButton(std::move(editorButton));

	std::function<void(void)> fQuit = std::bind(&Game::Quit, this);
	Button quitButton("Quit", Vector2f{ 100.f, 260.f }, 50.f, fQuit);
	quitButton.SetLogo("Return");
	m_MainMenu.AddButton(std::move(quitButton));
}

void Game::BindInputEvents()
{
	std::function<void(void)> fClickedLMB = std::bind(&UIPanel::LMBClicked, &m_MainMenu);
	InputManager::RegisterCallback(InputManager::MouseEvent::ClickedLMB, fClickedLMB, GameData::Mode::Menu);

	std::function<void(void)> fMovingLMB = std::bind(&UIPanel::MovingLMB, &m_MainMenu);
	InputManager::RegisterCallback(InputManager::MouseEvent::MovingLMB, fMovingLMB, GameData::Mode::Menu);

	std::function<void(void)> fPressedEscape = std::bind(&Game::GoToMainMenu, this);
	InputManager::RegisterCallback(InputManager::Key::Escape, fPressedEscape, GameData::Mode::PlayLevel);
}
