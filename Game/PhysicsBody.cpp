#include "pch.h"
#include "PhysicsBody.h"

PhysicsBody::PhysicsBody(Type type, const Rectf& bounds, bool canDie)
	: m_Type{ type }
	, m_Bounds{ bounds }
	, m_Vel{ Vector2f{} }
	, m_TargetVel{ Vector2f{} }
	, m_Acc{ Vector2f{} }
	, m_OverlapRects{ std::vector<OverlapRectInfo>{} }
	, m_CanDie{ canDie }
	, m_IsDead{ false }
	, m_Active{ true }
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
	if (!m_Active)
		return;

	//Point acc in direction of targetVel
	if (targetVel > 0.f || (targetVel == 0.f && vel < 0.f))
		acc = std::abs(acc);
	else // if (targetVel < 0.f || (targetVel == 0.f && vel > 0.f))
		acc = -std::abs(acc);

	vel += acc * dt;
	if ((acc > 0.f && vel > targetVel) || (acc < 0.f && vel < targetVel))
		vel = targetVel;
}

void PhysicsBody::AddOverlapRect(const Vector2f& offset, float width, float height, Type allowedPhysicsBodyCollision, bool detailedCollisionInfo)
{
	m_OverlapRects.push_back(OverlapRectInfo{ Rectf{ offset.x, offset.y, width, height }, allowedPhysicsBodyCollision, detailedCollisionInfo });
}

void PhysicsBody::SetPosition(const Point2f& pos)
{
	Point2f origPos{ m_Bounds.left, m_Bounds.bottom };
	m_Bounds.left = pos.x;
	m_Bounds.bottom = pos.y;
	for (OverlapRectInfo& overlapRect : m_OverlapRects)
	{
		Rectf& rect{ overlapRect.rect };
		Point2f rectOffset{ rect.left - origPos.x, rect.bottom - origPos.y };
		rect.left = m_Bounds.left + rectOffset.x;
		rect.bottom = m_Bounds.bottom + rectOffset.y;
	}
}

void PhysicsBody::SetMovement(const Vector2f& targetVel, const Vector2f& vel, const Vector2f& acc)
{
	m_Vel = vel;
	m_TargetVel = targetVel;
	m_Acc = acc;
}

Rectf PhysicsBody::GetBounds() const
{
	return m_Bounds;
}

bool PhysicsBody::IsDead() const
{
	return m_IsDead;
}

void PhysicsBody::SetIsDead(bool isDead)
{
	m_IsDead = isDead;
}

void PhysicsBody::Activate(bool activate)
{
	m_Active = activate;
}

PhysicsBody::Type PhysicsBody::GetType() const
{
	return m_Type;
}
