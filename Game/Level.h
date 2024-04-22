#pragma once
#include <vector>
#include <unordered_map>
#include "LevelScreen.h"

//Forward declarations
class PhysicsBody;
class Texture;
class InputManager;
class Camera;

class Level final
{
public:
	struct LevelScreenData
	{
		std::string filePath;
		std::vector<LevelScreen::Gate> gates;
	};

	Level(InputManager* pInputManager, Camera* pCamera);
	~Level();

	void Draw() const;
	void Update(float dt);
private:
	//Functions
	void LoadLevel(const std::string& name);
	bool TransferPhysicsBody(PhysicsBody* pPhysicsBody, const LevelScreen::Gate& srcGate);

	//Members
	Camera* m_pCamera;
	std::unordered_map<std::string, LevelScreenData> m_LevelScreensData;
	LevelScreen* m_pCurLevelScreen;
	std::vector<Texture*> m_pTextures;
	std::vector<int> m_IDToTextureIdxArr;
	PhysicsBody* m_pPlayer;

	friend void LevelScreen::Draw() const;
	friend void LevelScreen::Update(float dt);
};

