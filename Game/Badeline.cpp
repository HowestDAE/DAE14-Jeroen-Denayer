#include "pch.h"
#include "Badeline.h"

Badeline::Badeline(float width, float height)
	: PhysicsBody(PhysicsBody::Type::Badeline, Vector2f{})
	, m_MultiSpriteSheet{ MultiSpriteSheet{ "BadelineSpritesheet", 10, 10,
		std::unordered_map<std::string, MultiSpriteSheet::SpriteSheetInfo>
			{
				{"Charge", {0, 46, 0.1f}},
				{"Hit", {46, 66, 0.1f}},
				{"Idle", {66, 84, 0.1f}}
			}
		}
	}
	, m_PathIdx{ 0 }
	, m_Path{ std::vector<PathPointInfo>{
			PathPointInfo{Vector2f{456.f, 48.f}, "Room1"},//Room1
			PathPointInfo{Vector2f{552.f, 96.f}, "Room1"},
			PathPointInfo{Vector2f{552.f, 0.f}, "Room1"},
			PathPointInfo{Vector2f{40.f, 184.f}, "Room2"},//Room2
			PathPointInfo{Vector2f{152.f, 48.f}, "Room2"},
			PathPointInfo{Vector2f{256.f, 88.f}, "Room2"},
			PathPointInfo{Vector2f{392.f, 48.f}, "Room2"},
			PathPointInfo{Vector2f{528.f, 72.f}, "Room2"},
			PathPointInfo{Vector2f{688.f, 80.f}, "Room2"},
			PathPointInfo{Vector2f{696.f, 0.f}, "Room2"},
			PathPointInfo{Vector2f{40.f, 184.f}, "Room3"},//Room3
			PathPointInfo{Vector2f{176.f, 112.f}, "Room3"},
			PathPointInfo{Vector2f{272.f, 112.f}, "Room3"},
			PathPointInfo{Vector2f{432.f, 64.f}, "Room3"},
			PathPointInfo{Vector2f{448.f, 128.f}, "Room3"},
			PathPointInfo{Vector2f{568.f, 104.f}, "Room3"},
			PathPointInfo{Vector2f{584.f, 0.f}, "Room3"},
			PathPointInfo{Vector2f{40.f, 176}, "Room4"},//Room4
			PathPointInfo{Vector2f{168.f, 112.f}, "Room4"},
			PathPointInfo{Vector2f{296.f, 48.f}, "Room4"},
			PathPointInfo{Vector2f{408.f, 64.f}, "Room4"},
			PathPointInfo{Vector2f{560.f, 56.f}, "Room4"},
			PathPointInfo{Vector2f{552.f, 0}, "Room4"},
			PathPointInfo{Vector2f{40.f, 176}, "Room5"},//Room5
			PathPointInfo{Vector2f{264.f, 104.f}, "Room5"},
			PathPointInfo{Vector2f{560.f, 136.f}, "Room5"},
			PathPointInfo{Vector2f{584.f, 0}, "Room5"},
			PathPointInfo{Vector2f{40.f, 176}, "Room6"},//Room6
			PathPointInfo{Vector2f{192.f, 80.f}, "Room6"},
			PathPointInfo{Vector2f{320.f, 40.f}, "Room6"},
			PathPointInfo{Vector2f{392.f, 136.f}, "Room6"},
			PathPointInfo{Vector2f{424.f, 0}, "Room6"},
			PathPointInfo{Vector2f{40.f, 176.f}, "Room7"},//Room7
			PathPointInfo{Vector2f{232.f, 88.f}, "Room7"},
			PathPointInfo{Vector2f{584.f, 112.f}, "Room7"},
			PathPointInfo{Vector2f{600.f, 0.f}, "Room7"},
			PathPointInfo{Vector2f{56.f, 448}, "Room8"},//Room8
			PathPointInfo{Vector2f{144.f, 304.f}, "Room8"},
			PathPointInfo{Vector2f{200.f, 176.f}, "Room8"},
			PathPointInfo{Vector2f{280.f, 80.f}, "Room8"},
			PathPointInfo{Vector2f{296.f, 0}, "Room8"}
		}
	}
{
	Activate(false);
	AimTargetVelToNextPathPoint();

	Vector2f p0{ m_Path[m_PathIdx].pos };
	SetPosition(p0);
	Vector2f size{16.f, 16.f};
	AddOverlapRect(Vector2f{ p0.x - size.x / 2.f, p0.y - size.y / 2.f }, size.x, size.y, std::unordered_map<Type, TypeInfo>{{Type::Madeline, TypeInfo{ false }}, { Type::LevelScreenGate, TypeInfo{ false } }}, false);

	m_MultiSpriteSheet.SetSpriteSheetName("Idle");
}

void Badeline::Draw(const LevelScreen* pLevelScreen) const
{
	float frameSize{ 96.f };
	const Rectf& rect{ m_OverlapRects[0].rect };
	Rectf dstRect{
		rect.left + rect.width / 2.f - frameSize / 2.f,
		rect.bottom - 32.f,
		frameSize, frameSize
	};
	m_MultiSpriteSheet.Draw(dstRect);
}

void Badeline::Update(float dt)
{
	if (m_Active)
	{
		Vector2f newPos{ m_Pos.x + m_Vel.x * dt, m_Pos.y + m_Vel.y * dt };
		SetPosition(newPos);

		Vector2f pNext{ m_Path[m_PathIdx + 1].pos };
		float distX{ pNext.x - m_Pos.x };
		float distY{ pNext.y - m_Pos.y };
		float dist{ std::sqrtf(distX * distX + distY * distY) };
		if (dist < 4.f) //reached next path point
			MoveToNextPathPoint();
	}

	m_MultiSpriteSheet.Update(dt);

	std::cout << m_Pos.x << " " << m_Pos.y << std::endl;
}

void Badeline::CollisionInfoResponse(int overlapRectIdx, const CollisionInfo& ci, Type type, const PhysicsBody* pCollisionBody)
{
	if (m_Active || m_PathIdx + 1 >= m_Path.size())
		return;

	switch (type)
	{
	case PhysicsBody::Type::Madeline:
		Activate(true);
		break;
	case PhysicsBody::Type::LevelScreenGate:
		MoveToNextPathPoint();
		Activate(true);
		break;
	}
}

bool Badeline::CanTransferThroughGate(const LevelScreenGate& gate) const
{
	//Check if the gate goes to a levelScreen where the next path point is
	return !m_Active && gate.GetconnectedLevelScreenName() == m_Path[m_PathIdx + 1].levelScreenName;
}

std::string Badeline::GetCurrentLevelScreenName() const
{
	return m_Path[m_PathIdx].levelScreenName;
}

bool Badeline::IsMovingToNextRoom() const
{
		return m_Active &&
			m_PathIdx + 2 < m_Path.size() &&
			m_Path[m_PathIdx + 1].levelScreenName != m_Path[m_PathIdx + 2].levelScreenName;
}

void Badeline::MoveToNextRoom()
{
	while (m_PathIdx < (m_Path.size() - 1))
	{
		if (m_Path[m_PathIdx].levelScreenName == m_Path[m_PathIdx + 1].levelScreenName)
		{
			MoveToNextPathPoint();
		}
		else
		{
			MoveToNextPathPoint();
			break;
		}
	}
}

void Badeline::Reset()
{
	MoveToPathPoint(0);
}

void Badeline::MoveToPathPoint(int pathIdx)
{
	if (pathIdx < 0 && pathIdx >= m_Path.size())
		return;

	Activate(false);
	m_PathIdx = pathIdx;
	SetPosition(m_Path[m_PathIdx].pos);
	AimTargetVelToNextPathPoint();
}

void Badeline::MoveToNextPathPoint()
{
	MoveToPathPoint(m_PathIdx + 1);
}

void Badeline::AimTargetVelToNextPathPoint()
{
	if (m_PathIdx + 1 < m_Path.size())
	{
		const float targetVel{ 250.f };
		const float acc{ 500.f };
		Vector2f p0{ m_Path[m_PathIdx].pos };
		Vector2f p1{ m_Path[m_PathIdx + 1].pos };
		float distX{ p1.x - p0.x };
		float distY(p1.y - p0.y);
		float angle{ std::atan2f(distY, distX) };
		SetMovement(Vector2f{ targetVel * cosf(angle), targetVel * sinf(angle) }, Vector2f{ 0.f, 0.f }, Vector2f{ acc * cosf(angle), acc * sinf(angle) });
	}
	else
	{
		SetMovement(Vector2f{}, Vector2f{}, Vector2f{});
	}
}

void Badeline::Attack()
{
}
