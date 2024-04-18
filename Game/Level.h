#pragma once
#include <vector>
#include <unordered_map>
#include "LevelScreen.h"

//Forward declarations
class PhysicsBody;
class Texture;
class InputManager;

class Level final
{
public:
	Level(InputManager* pInputManager);
	~Level();

	void Draw() const;
	void Update(float dt);
	const LevelScreen* GetCurLevelScreen() const;
	PhysicsBody* GetPhysicsBodyToTrack() const;
private:
	//Functions
	void RemovingPhysicsBody(PhysicsBody* pPhysicsBody, const LevelScreen::Gate& gate);

	//Members
	int m_CurLevelScreenDataIdx;
	std::vector<LevelScreen::InitData> m_LevelScreensData;
	LevelScreen* m_pCurLevelScreen;
	LevelScreen* m_pPrevLevelScreen;
	std::vector<Texture*> m_pTextures;
	std::vector<int> m_IDToTextureIdxArr;
	PhysicsBody* m_pPhysicsBodyToTrack;

	friend void LevelScreen::Draw() const;
};

