#pragma once
#include <vector>
#include "MultiSpriteSheet.h"
#include "PhysicsBody.h"

class Badeline final
	:public PhysicsBody
{
public:
	explicit Badeline(float width, float height);

	virtual void Draw(const LevelScreen* pLevelScreen = nullptr) const override;
	virtual void Update(float dt) override;
	virtual void CollisionInfoResponse(int overlapRectIdx, const CollisionInfo& ci, Type type, const PhysicsBody* pCollisionBody = nullptr) override;
	virtual bool CanTransferThroughGate(const LevelScreenGate& gate) const;

	std::string GetCurrentLevelScreenName() const;
	bool IsMovingToNextRoom() const;
	void MoveToNextRoom();
	void Reset();
private:
	//Functions
	void MoveToNextPathPoint();
	void MoveToPathPoint(int pathIdx);
	void AimTargetVelToNextPathPoint();
	void Attack();
	
	//Members
	MultiSpriteSheet m_MultiSpriteSheet;
	int m_PathIdx;
	struct PathPointInfo
	{
		Vector2f pos;
		std::string levelScreenName;
	};
	std::vector<PathPointInfo> m_Path;
};