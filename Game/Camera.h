#pragma once
#include "Transform.h"

class Camera
{
public:
	struct TrackingInfo
	{
		float levelWidth;
		float levelHeight;
		Point2f center;
	};

	Camera(float screenW, float screenH);

	void Aim(const TrackingInfo& info);
	void Reset();
private:
	float m_ScreenW;
	float m_ScreenH;
	Transform m_Transform;
};

