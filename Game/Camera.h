#pragma once
#include "../Engine/Transform.h"

class Camera
{
public:
	Camera(float screenW, float screenH);

	void Aim(float levelW, float levelH, const Point2f center);
	void Reset();
private:
	float m_ScreenW;
	float m_ScreenH;
	Transform m_Transform;
};

