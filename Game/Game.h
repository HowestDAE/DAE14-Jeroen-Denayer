#pragma once
#include "BaseGame.h"
#include <vector>
#include "UIPanel.h"

//Forward declarations
class LevelEditor;
class Level;
class SoundStream;

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

	virtual bool Update( float elapsedSec ) override;
	virtual void Draw( ) const override;
	virtual bool HandleEvent(SDL_Event& e) const override;
private:
	//Functions
	void Play();
	void RunEditor();
	void Quit();
	void GoToMainMenu();

	void CreateMainMenu();
	void BindInputEvents();

	//Members
	bool m_RunGame;
	UIPanel m_MainMenu;
	LevelEditor* m_pLevelEditor;
	Level* m_pActiveLvl;
	SoundStream* m_pMusic;
};