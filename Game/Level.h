#pragma once
#include <vector>
#include <unordered_map>
#include "LevelScreen.h"
#include "LevelScreenGate.h"

//Forward declarations
class PhysicsBody;
class Texture;
class InputManager;
class Camera;
class Badeline;

class Level final
{
public:
	Level();
	~Level();
	Level(const Level& other) = delete;
	Level& operator=(const Level& other) = delete;
	Level(Level&& other) = delete;
	Level& operator=(Level&& other) = delete;

	void Draw() const;
	void Update(float dt);
	void Reset();
	bool IsValid() const;
private:
	//Functions
	void LoadLevel(const std::string& name);
	void TransferPhysicsBody(PhysicsBody* pPhysicsBody, const LevelScreenGate& srcGate);

	//Members
	bool m_IsValid;
	std::string m_StartRoom;
	Camera* m_pCamera;
	PhysicsBody* m_pPlayer;	
	std::string m_BadelineRoom;
	Badeline* m_pBadeline;
	LevelScreen* m_pCurLevelScreen;
	LevelScreen* m_pNextLevelScreen;
};

