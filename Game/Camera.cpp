#include "pch.h"
#include "Camera.h"
#include "Vector2f.h"

Camera::Camera(float screenW, float screenH)
	: m_ScreenW{ screenW }
	, m_ScreenH{ screenH }
	, m_Transform{}
{
}

void Camera::Aim(float levelW, float levelH, const Point2f center)
{
	//Center the camera on the player in x and y
	Point2f bottomLeft{ center.x - m_ScreenW / 2, center.y - m_ScreenH / 2 };
	if (bottomLeft.x < 0) bottomLeft.x = 0.f;
	else if (bottomLeft.x + m_ScreenW > levelW) bottomLeft.x = levelW - m_ScreenW;
	if (bottomLeft.y < 0) bottomLeft.y = 0.f;
	else if (bottomLeft.y + m_ScreenH > levelH) bottomLeft.y = levelH - m_ScreenH;

	m_Transform.Position = Vector2f{ -bottomLeft.x, -bottomLeft.y };
	m_Transform.ApplyTransformation();
}

void Camera::Reset()
{
	m_Transform.ResetTransformation();
}
