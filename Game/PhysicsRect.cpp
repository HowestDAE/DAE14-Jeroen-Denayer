#include "pch.h"
#include "PhysicsRect.h"

PhysicsRect::PhysicsRect(const Rectf& bounds, const Vector2f& vel, const Vector2f acc)
	: m_Bounds{ bounds }
	, m_Vel{ vel }
	, m_Acc{ acc }
{
}

void PhysicsRect::Update(float dt)
{
	UpdateAxis(dt, m_TargetVel.x, m_Vel.x, m_Acc.x);
	UpdateAxis(dt, m_TargetVel.y, m_Vel.y, m_Acc.y);
}

void PhysicsRect::UpdateAxis(float dt, float& targetVel, float& vel, float& acc)
{
	if (vel != targetVel)
	{
		//Point acc in direction of targetVel
		if (targetVel > 0.f || (targetVel == 0.f && vel < 0.f))
			acc = std::abs(acc);
		else // if (targetVel < 0.f || (targetVel == 0.f && vel > 0.f))
			acc = -std::abs(acc);

		vel += acc * dt;
		if ((acc > 0.f && vel > targetVel) || (acc < 0.f && vel < targetVel))
			vel = targetVel;
	}
}
