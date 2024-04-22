#include "pch.h"
#include "Camera.h"
#include "Vector2f.h"

Camera::Camera(float screenW, float screenH)
	: m_ScreenW{ screenW }
	, m_ScreenH{ screenH }
	, m_Transform{}
{
}

void Camera::Aim(const TrackingInfo& info)
{
	//Center the camera on the player in x and y
	Point2f bottomLeft{ info.center.x - m_ScreenW / 2, info.center.y - m_ScreenH / 2 };
	if (bottomLeft.x < 0) bottomLeft.x = 0.f;
	else if (bottomLeft.x + m_ScreenW > info.levelWidth) bottomLeft.x = info.levelWidth - m_ScreenW;
	if (bottomLeft.y < 0) bottomLeft.y = 0.f;
	else if (bottomLeft.y + m_ScreenH > info.levelHeight) bottomLeft.y = info.levelHeight - m_ScreenH;

	m_Transform.Position = Vector2f{ -bottomLeft.x, -bottomLeft.y };
	m_Transform.ApplyTransformation();
}

void Camera::Reset()
{
	m_Transform.ResetTransformation();
}
