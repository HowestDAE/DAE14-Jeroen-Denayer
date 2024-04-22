#pragma once
#include <vector>
#include "LevelScreen.h"

class PhysicsBody
{
public:
	explicit PhysicsBody(const Rectf& bounds, int idx = 0);
	virtual ~PhysicsBody() = default;
	
	virtual void Draw() const = 0;
	virtual void Update(float dt) = 0;
	void UpdatePhysics(float dt);
	void AddOverlapRect(const Vector2f& offset, float width, float height);
	void SetMovement(const Vector2f& targetVel, const Vector2f& vel, const Vector2f& acc);
	virtual void CollisionInfoResponse(int idx, const CollisionInfo& ci) = 0;
	Rectf GetBounds() const;
protected:
	Rectf m_Bounds;
	Vector2f m_Vel;
	Vector2f m_TargetVel;
	Vector2f m_Acc;
	std::vector<Rectf> m_OverlapRects;
private:
	void UpdateAxis(float dt, float& targetVel, float& vel, float& acc);

	friend void LevelScreen::Update(float dt);
	friend 	void LevelScreen::AddPhysicsBodyThroughGate(PhysicsBody* pPhysicsBody, const Gate& gate);
};

