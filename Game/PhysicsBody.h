#pragma once
#include <vector>
#include "LevelScreen.h"

class PhysicsBody
{
public:
	enum class Type
	{
		Madeline, FallingBlock, Level, DashCrystal
	};

	explicit PhysicsBody(Type type, const Rectf& bounds, bool canDie = false, bool alwaysReceiveCollInfo = false);
	virtual ~PhysicsBody() = default;
	PhysicsBody(const PhysicsBody& other) = delete;
	PhysicsBody& operator=(const PhysicsBody& other) = delete;
	PhysicsBody(PhysicsBody&& other) = delete;
	PhysicsBody& operator=(PhysicsBody&& other) = delete;

	virtual void Draw(const LevelScreen* pLevelScreen = nullptr) const = 0;
	virtual void Update(float dt) = 0;
	virtual void CollisionInfoResponse(int idx, const CollisionInfo& ci) = 0;
	virtual std::string String() const;

	void UpdatePhysics(float dt);
	void AddOverlapRect(const Vector2f& offset, float width, float height, Type allowedPhysicsBodyCollision, bool alwaysReceiveCollInfo);
	void SetPosition(const Point2f& pos);
	void SetMovement(const Vector2f& targetVel, const Vector2f& vel, const Vector2f& acc);
	Rectf GetBounds() const;
	bool IsDead() const;
	void SetIsDead(bool isDead);
	Type GetType() const;
	void Activate(bool activate);

	struct OverlapRectInfo
	{
		Rectf rect;
		Type AllowedPhysicsBodyCollisionType;
		bool alwaysReceiveCollInfo;
	};
protected:
	Type m_Type;
	std::vector<Type> m_CollisionTypes;
	Rectf m_Bounds;
	Vector2f m_Vel;
	Vector2f m_TargetVel;
	Vector2f m_Acc;
	std::vector<OverlapRectInfo> m_OverlapRects;
	bool m_CanDie;
	bool m_IsDead;
	bool m_Active;
	bool m_AlwaysReceiveCollInfo;
private:
	//Functions
	void UpdateAxis(float dt, float& targetVel, float& vel, float& acc);

	friend void LevelScreen::Draw() const;
	friend bool LevelScreen::Update(float dt);
	friend void LevelScreen::AddPhysicsBodyThroughGate(PhysicsBody* pPhysicsBody, const LevelScreenGate& gate);
};

