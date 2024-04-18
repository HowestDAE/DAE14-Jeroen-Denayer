#include "pch.h"
#include "PhysicsBody.h"

PhysicsBody::PhysicsBody(const Rectf& bounds, int idx)
	: m_Bounds{ bounds }
	, m_Vel{ Vector2f{} }
	, m_TargetVel{ Vector2f{} }
	, m_Acc{ Vector2f{} }
	, m_OverlapRects{ std::vector<Rectf>{} }
{
}

void PhysicsBody::UpdatePhysics(float dt)
{
	if (m_Vel.x != m_TargetVel.x)
		UpdateAxis(dt, m_TargetVel.x, m_Vel.x, m_Acc.x);

	if (m_Vel.y != m_TargetVel.y)
		UpdateAxis(dt, m_TargetVel.y, m_Vel.y, m_Acc.y);
}

void PhysicsBody::UpdateAxis(float dt, float& targetVel, float& vel, float& acc)
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

void PhysicsBody::AddOverlapRect(const Vector2f& offset, float width, float height)
{
	m_OverlapRects.push_back(Rectf{ offset.x, offset.y, width, height });
}

void PhysicsBody::SetMovement(const Vector2f& targetVel, const Vector2f& vel, const Vector2f& acc)
{
	m_Vel = vel;
	m_TargetVel = targetVel;
	m_Acc = acc;
}

Point2f PhysicsBody::GetPosition() const
{
	return Point2f{ m_Bounds.left, m_Bounds.bottom };
}
