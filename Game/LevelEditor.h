#pragma once
#include <vector>

//Forward declarations
class Level;
class LevelScreen;
class Camera;

class LevelEditor final
{
public:
	LevelEditor(const Rectf& viewport);
	~LevelEditor();

	void Draw() const;
	void Update(float dt);
private:
	void PressedEnter();
	void PressedEscape();
	void ClickedLMB();
	void ScrollThroughModes();

	enum class Mode
	{
		ModeSelect, RunLevel, CreateLevel, EditLevel, CreateLevelScreen, EditLevelScreen
	};

	//Functions
	void SetModeSelect();
	void CreateLevel();
	//void EditLevel();
	void CreateLevelScreen();
	void EditLevelScreen();
	void SelectLevelScreenToEdit();

	void SetSelectedMode(Mode mode, bool enter = false);
	void EnterSelectedMode();

	void CreateDirIfDoesntExist(const std::string& dir);
	bool IsFileOpen(std::ofstream& file);
	void CloseFile(std::ofstream&& file);
	void PrintHeader(const std::string& text);

	//Members
	Rectf m_Viewport;
	Point2f m_ResolutionScale;
	Camera* m_pCamera;

	int m_NUM_MODES;
	Mode m_Mode;
	Mode m_SelectedMode;
	std::vector<std::string> m_MODE_NAMES;
	bool m_EnteredMode;
	Level* m_pCurLevel;
	LevelScreen* m_pCurLevelScreen;
};
