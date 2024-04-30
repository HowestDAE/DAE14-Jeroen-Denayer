#include "pch.h"
#include "Camera.h"
#include "Vector2f.h"
#include "GameData.h"
#include "Transform.h"

Camera::Camera(const Vector2f& boundary, const Vector2f& zoom)
	: m_ScreenSize{ Vector2f{GameData::RENDER_RES_X(), GameData::RENDER_RES_Y()}}
	, m_Boundary{}
	, m_ConstrainToBoundary{}
	, m_Offset{}
	, m_BaseZoom{}
	, m_Zoom{}
	, m_MinZoom{ 0.25f }
{
	SetTarget(boundary, zoom);
}

void Camera::SetTarget(const Vector2f& boundary, const Vector2f& zoom)
{
	m_Boundary = boundary;
	m_ConstrainToBoundary = boundary != Vector2f{ 0.f, 0.f };
	m_Offset = Vector2f{};
	m_BaseZoom = zoom;
	m_Zoom = zoom;
}

void Camera::Aim(const Rectf& rect)
{
	//Center the pos on the screen
	m_ScreenCenter = Vector2f{ GameData::SCREEN_WIDTH() / 2, GameData::SCREEN_HEIGHT() / 2 };
	glPushMatrix();
	glTranslatef(m_ScreenCenter.x, m_ScreenCenter.y, 0.f);

	//Scale around the origin
	m_Scaling = m_Zoom;
	glPushMatrix();
	glScalef(m_Scaling.x, m_Scaling.y, 1.f);

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
	m_PosCenter = offset;
	glPushMatrix();
	glTranslatef(m_PosCenter.x, m_PosCenter.y, 0.f);
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

Vector2f Camera::GetWorldPos(Vector2f screenPos) const
{
	screenPos += -m_ScreenCenter;
	screenPos *= 1 / m_Scaling;
	screenPos += -m_PosCenter;
	return screenPos;
}
