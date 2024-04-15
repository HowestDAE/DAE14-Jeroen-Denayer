#pragma once

class PhysicsRect
{
public:
	PhysicsRect(const Rectf& bounds, const Vector2f& vel, const Vector2f acc);

	void Update(float dt);
private:
	void UpdateAxis(float dt, float& targetVel, float& vel, float& acc);

	Rectf m_Bounds;
	Vector2f m_TargetVel;
	Vector2f m_Vel;
	Vector2f m_Acc;
};

