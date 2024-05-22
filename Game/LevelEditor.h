#pragma once
#include <vector>

//Forward declarations
class Level;
class LevelScreen;
class Camera;

class LevelEditor final
{
public:
	LevelEditor();
	~LevelEditor();
	LevelEditor(const LevelEditor& other) = delete;
	LevelEditor& operator=(const LevelEditor& other) = delete;
	LevelEditor(LevelEditor&& other) = delete;
	LevelEditor& operator=(LevelEditor&& other) = delete;

	void Draw() const;
	void Update(float dt);
private:
	enum class Mode
	{
		ModeSelect, RunLevel, CreateLevel, EditLevel, CreateLevelScreen, EditLevelScreen
	};

	//Callback functions
	void KeyPressed();
	void ClickedLMB();
	void ScrollingMMB();
	void DraggingMMB();
	void EditCurrentMode();

	//Functions
	void SetDefaultMode();
	void ScrollThroughModes();
	void CreateLevel();
	//void EditLevel();
	void CreateLevelScreen();
	void SelectLevelScreenToEdit();

	void SetSelectedMode(Mode mode, bool enter = false);
	void EnterSelectedMode();

	void CreateDirIfDoesntExist(const std::string& dir);
	bool IsFileOpen(std::ofstream& file);
	void CloseFile(std::ofstream&& file);
	void PrintHeader(const std::string& text);

	//Members
	Mode m_DefaultMode;
	Mode m_Mode;
	Mode m_SelectedMode;
	std::vector<std::string> m_ModeNames;

	Camera* m_pCamera;
	Level* m_pCurLevel;
	LevelScreen* m_pCurLevelScreen;
};

