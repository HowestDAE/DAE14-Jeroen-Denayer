#pragma once
#include <vector>

class Camera
{
public:
	Camera(const Vector2f& boundary = Vector2f{0.f, 0.f}, const Vector2f& zoom = Vector2f{1.f, 1.f});

	void SetTarget(const Vector2f& boundary = Vector2f{ 0.f, 0.f }, const Vector2f& zoom = Vector2f{ 1.f, 1.f });
	void Aim(const Rectf& rect);
	void Zoom(int dir);
	void Offset(const Vector2f& offset);
	void Focus();
	void Reset();
	Vector2f GetWorldPos(Vector2f screenPos) const;
private:
	Vector2f m_ScreenSize;
	Vector2f m_Boundary;
	bool m_ConstrainToBoundary;
	Vector2f m_Offset;
	Vector2f m_BaseZoom;
	Vector2f m_Zoom;
	float m_MinZoom;
	struct Transformation
	{
		enum class Type
		{
			T, R, S
		};
		Type type;
		Vector2f value;
	};
	std::vector<Transformation> m_Transformations;
	Vector2f m_ScreenCenter;
	Vector2f m_Scaling;
	Vector2f m_PosCenter;
};

