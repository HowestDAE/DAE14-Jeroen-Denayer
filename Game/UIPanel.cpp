#include "pch.h"
#include "UIPanel.h"
#include "AssetManager.h"
#include "Texture.h"
#include "InputManager.h"

UIPanel::UIPanel(const std::string& backgroundImgName)
	: m_pBgImg{ AssetManager::GetTexture("MainMenuBg") }
	, m_HoveredButtonIdx{0}
	, m_Buttons{ std::vector<Button>{} }
{
	HoverButton(m_HoveredButtonIdx, true);

	std::function<void(void)> fClickButton = std::bind(&UIPanel::ClickButton, this);
	InputManager::RegisterCallback(InputManager::Key::Enter, fClickButton, GameData::Mode::Menu);
}

UIPanel::~UIPanel()
{
	AssetManager::RemoveTexture(m_pBgImg);
}

UIPanel& UIPanel::operator=(UIPanel&& other) noexcept
{
	if (this != &other)
	{
		m_HoveredButtonIdx = other.m_HoveredButtonIdx;

		delete m_pBgImg;
		m_pBgImg = other.m_pBgImg;

		m_Buttons.clear();
		for (Button& button : other.m_Buttons)
			m_Buttons.push_back(std::move(button));
	}
	return *this;
}

void UIPanel::Draw() const
{
	m_pBgImg->Draw();
	for (const Button& button : m_Buttons)
		button.Draw();
}

void UIPanel::Update(float dt)
{
	const int y{ InputManager::GetControllerInfo().leftJoystickDir.y };
	if (y != 0)
		HoverButton(m_HoveredButtonIdx, false);

	if (y == 1 && m_HoveredButtonIdx == -1)
		m_HoveredButtonIdx = 0;
	else if (y == 1 && m_HoveredButtonIdx > 0)
		m_HoveredButtonIdx -= 1;
	else if (y == -1 && m_HoveredButtonIdx < m_Buttons.size() - 1)
		m_HoveredButtonIdx += 1;

	HoverButton(m_HoveredButtonIdx, true);
}

void UIPanel::AddButton(Button&& button)
{
	m_Buttons.push_back(std::move(button));
}

void UIPanel::AddButton(const std::string& text, Vector2f pos, float height, std::function<void()> callback)
{
	m_Buttons.push_back(Button(text, pos, height, callback));
}

void UIPanel::LMBClicked()
{
	Vector2f mousePos{ InputManager::GetMouseInfo().pos };
	for (Button& button : m_Buttons)
	{
		bool isClicked{ button.IsClicked(mousePos) };
		if (isClicked)
			break;
	}
}

void UIPanel::MovingLMB()
{
	if (m_HoveredButtonIdx != -1)
	{
		m_Buttons[m_HoveredButtonIdx].SetHovered(false);
		m_HoveredButtonIdx = -1;
	}

	Vector2f mousePos{ InputManager::GetMouseInfo().pos };
	for (int i{}; i < m_Buttons.size(); ++i)
	{
		Button& button{ m_Buttons[i] };
		bool isHovered{ button.IsHovered(mousePos) };
		if (isHovered)
		{
			m_HoveredButtonIdx = i;
			break;
		}
	}
}

void UIPanel::ClickButton()
{
	if (m_HoveredButtonIdx > -1 && m_HoveredButtonIdx < m_Buttons.size())
		m_Buttons[m_HoveredButtonIdx].Click();
}

void UIPanel::HoverButton(int idx, bool hover)
{
	if (idx > -1 && idx < m_Buttons.size())
		m_Buttons[idx].SetHovered(hover);
}
