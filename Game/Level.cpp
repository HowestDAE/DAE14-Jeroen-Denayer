#include "pch.h"
#include "Level.h"
#include "Texture.h"
#include "Madeline.h"

Level::Level(InputManager* pInputManager)
	: m_pCurLevelScreen{ nullptr }
	, m_pPrevLevelScreen{ nullptr }
{
	m_LevelScreensData = std::vector<LevelScreen::InitData>{
		{"MainRoom", "Textures/Levels/MainRoom.bmp",
			std::vector<LevelScreen::Gate>{
				{"SmallRoom", 0, LevelScreen::Gate::Orientation::Vertical, LevelScreen::Gate::Side::Left, 6, 3}
			}
		},
		{"SmallRoom", "Textures/Levels/SmallRoom.bmp",
			std::vector<LevelScreen::Gate>{
				{"MainRoom", 0, LevelScreen::Gate::Orientation::Vertical, LevelScreen::Gate::Side::Right, 25, 3}
			}
		},
	};
	m_CurLevelScreenDataIdx = 0;
	m_pCurLevelScreen = new LevelScreen(m_LevelScreensData[m_CurLevelScreenDataIdx], this);

	Point2f pos{ 8 * 8, 2 * 8 };
	float madelinePixWidth{ 8 };
	float madelinePixHeight{ 16 };
	Madeline* pMadeline{ new Madeline(pos, madelinePixWidth, madelinePixHeight, pInputManager) };
	m_pCurLevelScreen->AddPhysicsBody(pMadeline);
	m_pPhysicsBodyToTrack = pMadeline;

	//Load textures needed for this level
	//Hardcoded atm, should be changed
	std::vector<std::string> textureNames{ "Textures/grass.png", "Textures/reflection.png" };
	for (const std::string& textureName : textureNames)
	{
		Texture* pTexture{ new Texture(textureName) };
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
	delete m_pPrevLevelScreen;

	for (Texture* pTexture : m_pTextures)
		delete pTexture;
}

void Level::Draw() const
{
	m_pCurLevelScreen->Draw();
}

void Level::Update(float dt)
{
	m_pCurLevelScreen->Update(dt);
}

const LevelScreen* Level::GetCurLevelScreen() const
{
	return m_pCurLevelScreen;;
}

PhysicsBody* Level::GetPhysicsBodyToTrack() const
{
	return m_pPhysicsBodyToTrack;
}

void Level::RemovingPhysicsBody(PhysicsBody* pPhysicsBody, const LevelScreen::Gate& gate)
{

}

