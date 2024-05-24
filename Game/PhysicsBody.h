#pragma once
#include <vector>
#include "LevelScreen.h"

class PhysicsBody
{
public:
	explicit PhysicsBody(const Rectf& bounds, bool canDie = false);
	virtual ~PhysicsBody() = default;
	PhysicsBody(const PhysicsBody& other) = delete;
	PhysicsBody& operator=(const PhysicsBody& other) = delete;
	PhysicsBody(PhysicsBody&& other) = delete;
	PhysicsBody& operator=(PhysicsBody&& other) = delete;

	virtual void Draw() const = 0;
	virtual void Update(float dt) = 0;
	virtual void CollisionInfoResponse(int idx, const CollisionInfo& ci) = 0;

	void UpdatePhysics(float dt);
	void AddOverlapRect(const Vector2f& offset, float width, float height);
	void SetPosition(const Point2f& pos);
	void SetMovement(const Vector2f& targetVel, const Vector2f& vel, const Vector2f& acc);
	Rectf GetBounds() const;
	bool IsDead() const;
	void SetIsDead(bool isDead);
protected:
	Rectf m_Bounds;
	Vector2f m_Vel;
	Vector2f m_TargetVel;
	Vector2f m_Acc;
	std::vector<Rectf> m_OverlapRects;
	bool m_CanDie;
	bool m_IsDead;
private:
	//Functions
	void UpdateAxis(float dt, float& targetVel, float& vel, float& acc);

	friend bool LevelScreen::Update(float dt);
	friend void LevelScreen::AddPhysicsBodyThroughGate(PhysicsBody* pPhysicsBody, const LevelScreenGate& gate);
};

