#pragma once

class Camera
{
public:
	Camera(const Vector2f& boundary = Vector2f{0.f, 0.f}, const Vector2f& zoom = Vector2f{1.f, 1.f});

	void Aim(const Rectf& rect);
	void Zoom(int dir);
	void Offset(const Vector2f& offset);
	void Focus();
	void Reset();
private:
	Vector2f m_ScreenSize;
	Vector2f m_Boundary;
	bool m_ConstrainToBoundary;
	Vector2f m_Offset;
	Vector2f m_BaseZoom;
	Vector2f m_Zoom;
	float m_MinZoom;
};

