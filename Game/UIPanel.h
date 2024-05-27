#pragma once
#include <vector>
#include <functional>
#include "Button.h"

//Forward declarations
class Texture;

class UIPanel final
{
public:
	UIPanel(const std::string& backgroundImgName);
	~UIPanel();
	UIPanel(const UIPanel& other) = delete;
	UIPanel& operator=(const UIPanel& other) = delete;
	UIPanel(UIPanel&& other) = delete;
	UIPanel& operator=(UIPanel&& other) noexcept;

	void Draw() const;
	void Update(float dt);
	void AddButton(Button&& button);
	void AddButton(const std::string& text, Vector2f pos, float height, std::function<void()> callback);
	void LMBClicked();
	void MovingLMB();
private:
	//Functions
	void ClickButton();
	void HoverButton(int idx, bool hover);

	//Members
	Texture* m_pBgImg;
	int m_HoveredButtonIdx;
	std::vector<Button> m_Buttons;
	float m_UpdateTime;
	float m_Timer;
};

