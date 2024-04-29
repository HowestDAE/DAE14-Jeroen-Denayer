#include "pch.h"
#include "Camera.h"
#include "Vector2f.h"
#include "GameData.h"
#include "Transform.h"

Camera::Camera(const Vector2f& boundary, const Vector2f& zoom)
	: m_ScreenSize{ Vector2f{GameData::RENDER_RES_X(), GameData::RENDER_RES_Y()}}
	, m_Boundary{ boundary }
	, m_ConstrainToBoundary{ boundary != Vector2f{0.f, 0.f} }
	, m_Offset{}
	, m_BaseZoom{ zoom }
	, m_Zoom{ zoom }
	, m_MinZoom{ 0.25f }
{
}

void Camera::Aim(const Rectf& rect)
{
	//Center the pos on the screen
	Transform centerScreen{ Vector2f{GameData::SCREEN_WIDTH() / 2, GameData::SCREEN_HEIGHT() / 2}, 0.f, Vector2f{1.f, 1.f} };
	centerScreen.ApplyTransformation();

	//Scale around the origin
	Transform scaling{ Vector2f{}, 0.f, m_Zoom };
	scaling.ApplyTransformation();

	//Center the pos around the origin
	Vector2f screenLeftBottom{ rect.left + rect.width / 2 - m_ScreenSize.x / 2, rect.bottom + rect.height / 2 - m_ScreenSize.y / 2 };
	screenLeftBottom -= m_Offset / m_Zoom;

	if (m_ConstrainToBoundary)
	{
		if (screenLeftBottom.x < 0)
			screenLeftBottom.x = 0;
		else if (screenLeftBottom.x + m_ScreenSize.x > m_Boundary.x)
			screenLeftBottom.x = m_Boundary.x - m_ScreenSize.x;
		if (screenLeftBottom.y < 0)
			screenLeftBottom.y = 0;
		else if (screenLeftBottom.y + m_ScreenSize.y > m_Boundary.y)
			screenLeftBottom.y = m_Boundary.y - m_ScreenSize.y;
	}

	Vector2f offset{-screenLeftBottom - m_ScreenSize / 2};
	Transform centerRect{ offset, 0.f, Vector2f{1.f, 1.f} };
	centerRect.ApplyTransformation();
}

void Camera::Zoom(int dir)
{
	m_Zoom.x *= (dir == 1) ? 1.25f : 0.75f;
	if (m_Zoom.x < m_MinZoom)
		m_Zoom.x = m_MinZoom;
	m_Zoom.y *= (dir == 1) ? 1.25f : 0.75f;
	if (m_Zoom.y < m_MinZoom)
		m_Zoom.y = m_MinZoom;
}

void Camera::Offset(const Vector2f& offset)
{
	m_Offset += offset;
}

void Camera::Focus()
{
	m_Offset = Vector2f{};
	m_Zoom = m_BaseZoom;
}

void Camera::Reset()
{
	glPopMatrix(); //Pop RectCentering
	glPopMatrix(); //Pop Zooming
	glPopMatrix(); //Pop ScreenCentering
}