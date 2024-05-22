#pragma once
#include <functional>

//Forward declarations
class Texture;

class Button
{
public:
	Button(const std::string& text, Vector2f pos, float height, std::function<void()> callback);
	virtual ~Button();
	Button(const Button& other) = delete;
	Button& operator=(const Button& other) = delete;
	Button(Button&& other) noexcept;
	Button& operator=(Button&& other) noexcept;

	void Draw() const;
	void Click();
	void SetLogo(const std::string& name);
	bool IsClicked(const Vector2f& mousePos);
	bool IsHovered(const Vector2f& mousePos);
	void SetHovered(bool hovered);
private:
	Vector2f m_Pos;
	float m_Width;
	float m_Height;
	Texture* m_pText;
	Texture* m_pLogo;
	std::function<void()> m_Callback;
	bool m_Hovered;
	bool m_Clicked;
};

