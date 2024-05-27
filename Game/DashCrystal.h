#pragma once
#include "PhysicsBody.h"

//Forward declarations
class Texture;

class DashCrystal
	:public PhysicsBody
{
public:
	DashCrystal(const Vector2f& pos);
	virtual ~DashCrystal() override;
	DashCrystal(const DashCrystal& other) = delete;
	DashCrystal& operator=(const DashCrystal& other) = delete;
	DashCrystal(DashCrystal&& other) = delete;
	DashCrystal& operator=(DashCrystal&& other) = delete;

	virtual void Draw(const LevelScreen* pLevelScreen = nullptr) const override;
	virtual void Update(float dt) override;
	virtual void CollisionInfoResponse(int idx, const CollisionInfo& ci) override;

	virtual std::string String() const override;
private:
	Texture* m_pDashCrystal;
	Texture* m_pDashCrystalOutline;
	float m_Timer;
	float m_RespawnTime;
};

