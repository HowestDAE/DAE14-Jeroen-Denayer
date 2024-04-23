#include "pch.h"
#include "Level.h"
#include "Texture.h"
#include "Madeline.h"
#include "Camera.h"

Level::Level(InputManager* pInputManager, Camera* pCamera)
	: m_pCamera{ pCamera }
	, m_pCurLevelScreen{ nullptr }
{
	//Store data needed for loading all the levels
	//To-DO: hardcoded atm, levelScreen should load it's own data from a file using only the levelScreenName
	m_LevelScreensData = std::unordered_map<std::string, LevelScreenData>{
		{"MainRoom", 
			{
				"Levels/MainRoom.bmp",
				std::vector<LevelScreen::Gate>{
					{"SmallRoom", 0, LevelScreen::Gate::Side::Left, 6, 3},
					{"SmallRoom", 1, LevelScreen::Gate::Side::Left, 50, 3},
				}
			}
		},
		{"SmallRoom",
			{
				"Levels/SmallRoom.bmp",
				std::vector<LevelScreen::Gate>{
					{"MainRoom", 0, LevelScreen::Gate::Side::Right, 2, 3},
					{"MainRoom", 1, LevelScreen::Gate::Side::Right, 25, 3},
				}
			}
		},
	};
	LoadLevel("MainRoom");

	//Create the player Madeline
	Point2f pos{ 8 * 8, 2 * 8 };
	float madelinePixWidth{ 8 };
	float madelinePixHeight{ 16 };
	Madeline* pMadeline{ new Madeline(pos, madelinePixWidth, madelinePixHeight, pInputManager) };
	m_pCurLevelScreen->AddPhysicsBody(pMadeline);
	m_pPlayer = pMadeline;

	//Load textures needed for this level
	//To-DO: hardcoded atm, read from file
	std::string texturePath{ "Textures/" };
	std::vector<std::string> textureNames{ "grass.png", "reflection.png" };
	for (const std::string& textureName : textureNames)
	{
		Texture* pTexture{ new Texture(texturePath + textureName) };
		if (!pTexture)
			std::cout << "Couldn't load " << textureName << std::endl;
		m_pTextures.push_back(pTexture);
	}

	//Map each index in the texture to a corresponding idx in m_pTextures
	//-1 will result in a black tile
	m_IDToTextureIdxArr = std::vector<int>{ -1, -1, 0, 0, 1 };
}

Level::~Level()
{
	delete m_pCurLevelScreen;

	for (Texture* pTexture : m_pTextures)
		delete pTexture;
}

void Level::Draw() const
{
	Rectf playerBounds{ m_pPlayer->GetBounds() };
	Point2f pos{ playerBounds.left + playerBounds.width / 2, playerBounds.bottom + playerBounds.height / 2 };
	Camera::TrackingInfo trackignInfo{ m_pCurLevelScreen->GetWidth(), m_pCurLevelScreen->GetHeight(), pos };
	m_pCamera->Aim(trackignInfo);
	m_pCurLevelScreen->Draw();
}

void Level::Update(float dt)
{
	m_pCurLevelScreen->Update(dt);
}

void Level::LoadLevel(const std::string& name)
{
	if (m_pCurLevelScreen) //delete existing levelScreen
		delete m_pCurLevelScreen;

	//Combine all data that the levelScreen will need into a single struct
	LevelScreen::InitData initData{ m_LevelScreensData[name].filePath, this, m_LevelScreensData[name].gates };
	//Load new level
	m_pCurLevelScreen = new LevelScreen(name, initData);
}

/*
If the physicsBody is not the player, the physicsBody will be removed
If the physcisBody is the player and there is valid new valid level to move to,
then the old level is deleted, the new level is instantiated, and the player is moved
to this new level
Return: true if the player moved to a new level, otherwise false
*/
bool Level::TransferPhysicsBody(PhysicsBody* pPhysicsBody, const LevelScreen::Gate& srcGate)
{
	bool movedToNewLevel{};
	if (pPhysicsBody == m_pPlayer) 
	{
		std::unordered_map<std::string, LevelScreenData>::iterator it{ m_LevelScreensData.find(srcGate.connectedLevelScreenName) };
		if (it != m_LevelScreensData.end()) //found level
		{
			const std::string& nextLevelScreenName{ it->first };
			const LevelScreenData& nextLevelScreenInitData{ it->second };

			//check if a matching gate exists
			std::vector<LevelScreen::Gate>& nextLevelScreenGates{ m_LevelScreensData[nextLevelScreenName].gates };
			if (srcGate.dstGateIdx >= 0 && srcGate.dstGateIdx < nextLevelScreenGates.size()) //connectedGateIdx maps to an existing gate
			{
				LevelScreen::Gate& dstGate{ nextLevelScreenGates[srcGate.dstGateIdx] };
				//check if the dstGate is on the opposite side and with equal length to the srcGate
				bool srcGateIsVertical{ int(srcGate.side) % 2 == 0 };
				bool dstGateIsVertical{ int(dstGate.side) % 2 == 0 };
				if (srcGateIsVertical == dstGateIsVertical && srcGate.side != dstGate.side && srcGate.length == dstGate.length)
				{
					LoadLevel(nextLevelScreenName);
					//Transfer the player to this new level via the dstGate
					m_pCurLevelScreen->AddPhysicsBodyThroughGate(pPhysicsBody, dstGate);
					movedToNewLevel = true;
				}
			}
		}
	}
	
	if (!movedToNewLevel)
		delete pPhysicsBody;

	return movedToNewLevel;
}
