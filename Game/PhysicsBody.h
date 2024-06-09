#pragma once
#include <vector>
#include <unordered_map>
#include "LevelScreen.h"

class PhysicsBody
{
public:
	enum class Type
	{
		Madeline, FallingBlock, Level, DashCrystal, Spike, LevelScreenGate, Badeline
	};

	struct TypeInfo
	{
		bool block;
	};

	explicit PhysicsBody(Type type, const Vector2f& pos);
	virtual ~PhysicsBody() = default;
	PhysicsBody(const PhysicsBody& other) = delete;
	PhysicsBody& operator=(const PhysicsBody& other) = delete;
	PhysicsBody(PhysicsBody&& other) = delete;
	PhysicsBody& operator=(PhysicsBody&& other) = delete;

	virtual void Draw(const LevelScreen* pLevelScreen = nullptr) const = 0;
	virtual void Update(float dt) = 0;
	virtual void CollisionInfoResponse(int overlapRectIdx, const CollisionInfo& ci, Type type, const PhysicsBody* pCollisionBody = nullptr) = 0;
	virtual bool CanTransferThroughGate(const LevelScreenGate& gate) const;
	virtual std::string String() const;

	void Activate(bool activate);
	void UpdatePhysics(float dt);
	void AddOverlapRect(const Vector2f& leftBottom, float width, float height, const std::unordered_map<Type, TypeInfo>& allowedCollisionTypes, bool alwaysReceiveCollInfo);
	void SetPosition(const Vector2f& pos);
	void SetMovement(const Vector2f& targetVel, const Vector2f& vel, const Vector2f& acc);
	Rectf GetBounds(int overlapRectIdx = 0) const;
	Type GetType() const;
	bool IsActive() const;

	struct OverlapRectInfo
	{
		Rectf rect;
		std::unordered_map<Type, TypeInfo> allowedCollisionTypes;
		bool alwaysReceiveCollInfo;
	};
protected:
	Type m_Type;
	Vector2f m_Pos;
	Vector2f m_Vel;
	Vector2f m_TargetVel;
	Vector2f m_Acc;
	std::vector<OverlapRectInfo> m_OverlapRects;
	bool m_Active;
private:
	//Functions
	void UpdateAxis(float dt, float& targetVel, float& vel, float& acc);

	friend void LevelScreen::Draw() const;
	friend bool LevelScreen::Update(float dt);
	friend void LevelScreen::AddPhysicsBodyThroughGate(PhysicsBody* pPhysicsBody, const LevelScreenGate& gate);
};

