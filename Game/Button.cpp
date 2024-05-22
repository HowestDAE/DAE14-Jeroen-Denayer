#include "pch.h"
#include "Button.h"
#include "AssetManager.h"
#include "Texture.h"

Button::Button(const std::string& text, Vector2f pos, float height, std::function<void()> callback)
	: m_Pos{ pos }
	, m_Width{}
	, m_Height{ height }
	, m_pText{ AssetManager::GetTextureFromText(text, "DIN-Light.otf", height, Color4f{0.85f, 0.85f, 1.f, 1.f}) }
	, m_pLogo{ nullptr }
	, m_Callback{ callback }
	, m_Hovered{ false }
	, m_Clicked{ false }
{ 
	m_Width = m_pText->GetWidth() * (m_Height / m_pText->GetHeight());
}

Button::~Button()
{
	AssetManager::RemoveTexture(m_pText);
	if (m_pLogo)
		AssetManager::RemoveTexture(m_pLogo);
}

Button::Button(Button&& other) noexcept
	: m_Pos{ other.m_Pos }
	, m_Width{ other.m_Width }
	, m_Height{ other.m_Height }
	, m_pText{ other.m_pText }
	, m_pLogo{ other.m_pLogo }
	, m_Callback{ other.m_Callback }
	, m_Hovered{ other.m_Hovered }
	, m_Clicked{ other.m_Clicked }
{
	other.m_pText = nullptr;
	other.m_pLogo = nullptr;
}

Button& Button::operator=(Button&& other) noexcept
{
	if (this != &other)
	{
		m_Pos = other.m_Pos;
		m_Width = other.m_Width;
		m_Height = other.m_Height;
		AssetManager::RemoveTexture(m_pText);
		m_pText = other.m_pText;
		other.m_pText = nullptr;
		AssetManager::RemoveTexture(m_pLogo);
		m_pLogo = other.m_pLogo;
		other.m_pLogo = nullptr;
		m_Callback = other.m_Callback;
		m_Hovered = other.m_Hovered;
		m_Clicked = other.m_Clicked;
	}
	return *this;
}

void Button::Draw() const
{
	if (m_pLogo)
		m_pLogo->Draw(Rectf{ m_Pos.x - m_Height, m_Pos.y, m_Height, m_Height });
	m_pText->Draw(Rectf{ m_Pos.x, m_Pos.y, m_Width, m_Height });
	if (m_Hovered)
	{
		utils::SetColor(Color4f{ 1.f, 1.f, 1.f, 1.f });
		utils::DrawRect(Rectf{ m_Pos.x, m_Pos.y, m_Width, m_Height });
	}
}

void Button::Click()
{
	m_Callback();
}

void Button::SetLogo(const std::string& name)
{
	m_pLogo = AssetManager::GetTexture(name);
}

bool Button::IsClicked(const Vector2f& mousePos)
{
	m_Clicked = utils::IsPointInRect(Point2f{ mousePos.x, mousePos.y }, Rectf{ m_Pos.x, m_Pos.y, m_Width, m_Height });
	if (m_Clicked)
		m_Callback();
	return m_Clicked;
}

bool Button::IsHovered(const Vector2f& mousePos)
{
	m_Hovered = utils::IsPointInRect(Point2f{ mousePos.x, mousePos.y }, Rectf{ m_Pos.x, m_Pos.y, m_Width, m_Height });
	return m_Hovered;
}

void Button::SetHovered(bool hovered)
{
	m_Hovered = hovered;
}
