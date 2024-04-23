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
	Level(const Level& other) = delete;
	Level& operator=(const Level& other) = delete;
	Level(Level&& other) = delete;
	Level& operator=(Level&& other) = delete;

	void Draw() const;
	void Update(float dt);
private:
	//Functions
	void LoadLevel(const std::string& name);
	bool TransferPhysicsBody(PhysicsBody* pPhysicsBody, const LevelScreen::Gate& srcGate);

	//Members
	std::unordered_map<std::string, LevelScreenData> m_LevelScreensData; //TO-DO: LevelScreen should load/hold it's own data
	Camera* m_pCamera;
	PhysicsBody* m_pPlayer;
	LevelScreen* m_pCurLevelScreen;
	//Shared data for all LevelScreens;
	std::vector<Texture*> m_pTextures;
	std::vector<int> m_IDToTextureIdxArr;

	friend void LevelScreen::Draw() const;
	friend void LevelScreen::Update(float dt);
};

