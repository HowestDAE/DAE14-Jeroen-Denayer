#include "pch.h"
#include "PhysicsBody.h"

PhysicsBody::PhysicsBody(Type type, const Vector2f& pos)
	: m_Type{ type }
	, m_Pos{ pos }
	, m_Vel{ Vector2f{} }
	, m_TargetVel{ Vector2f{} }
	, m_Acc{ Vector2f{} }
	, m_OverlapRects{ std::vector<OverlapRectInfo>{} }
	, m_Active{ true }
{
}

bool PhysicsBody::CanTransferThroughGate(const LevelScreenGate& gate) const
{
	return true;
}

std::string PhysicsBody::String() const
{
	return std::string("");
}

void PhysicsBody::UpdatePhysics(float dt)
{
	if (!m_Active)
		return;

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

void PhysicsBody::AddOverlapRect(const Vector2f& leftBottom, float width, float height, const std::unordered_map<Type, TypeInfo>& allowedCollisionTypes, bool alwaysReceiveCollInfo)
{
	m_OverlapRects.push_back(OverlapRectInfo{ Rectf{ leftBottom.x, leftBottom.y, width, height }, allowedCollisionTypes, alwaysReceiveCollInfo });
}

void PhysicsBody::SetPosition(const Vector2f& pos)
{
	Point2f origPos{ m_Pos.x, m_Pos.y };
	m_Pos.x = pos.x;
	m_Pos.y = pos.y;
	for (OverlapRectInfo& overlapRect : m_OverlapRects)
	{
		Rectf& rect{ overlapRect.rect };
		Point2f rectOffset{ rect.left - origPos.x, rect.bottom - origPos.y };
		rect.left = m_Pos.x + rectOffset.x;
		rect.bottom = m_Pos.y + rectOffset.y;
	}
}

void PhysicsBody::SetMovement(const Vector2f& targetVel, const Vector2f& vel, const Vector2f& acc)
{
	m_Vel = vel;
	m_TargetVel = targetVel;
	m_Acc = acc;
}

Rectf PhysicsBody::GetBounds(int overlapRectIdx) const
{
	if (overlapRectIdx > -1 && overlapRectIdx < m_OverlapRects.size())
		return m_OverlapRects[overlapRectIdx].rect;
	else
		return Rectf{};
}

void PhysicsBody::Activate(bool activate)
{
	m_Active = activate;
}

PhysicsBody::Type PhysicsBody::GetType() const
{
	return m_Type;
}

bool PhysicsBody::IsActive() const
{
	return m_Active;
}
