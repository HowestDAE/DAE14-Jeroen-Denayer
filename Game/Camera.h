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

	Camera(const Point2f& screenDimensions, const Point2f& resolutionScale);

	void Aim(const TrackingInfo& info);
	void Reset();
	Point2f GetResolutionScale() const;
private:
	float m_ScreenW;
	float m_ScreenH;
	Point2f m_ResolutionScale;
	Transform m_Transform;
};

