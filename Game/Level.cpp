#include "pch.h"
#include "Level.h"
#include "Texture.h"
#include "Madeline.h"
#include "Camera.h"
#include "GameData.h"
#include "FileIO.h"

Level::Level()
	: m_pCamera{ new Camera() }
	, m_pPlayer{ nullptr }
	, m_pCurLevelScreen{ nullptr }
	, m_pNextLevelScreen{ nullptr }
{
	LoadLevel("MainRoom");

	//Create the player Madeline
	float madelinePixWidth{ 8 };
	float madelinePixHeight{ 16 };
	AccAndVel madelineJumpAccAndVel{ utils::AccAndVelToTravelDistInTime(3.5f, 0.35f) };
	Point2f pos{ 8 * 8, 2 * 8 };
	m_pPlayer = new Madeline(pos, madelinePixWidth, madelinePixHeight);
	m_pCurLevelScreen->AddPhysicsBody(m_pPlayer);
}

Level::~Level()
{
	delete m_pCamera;
	delete m_pCurLevelScreen;

	m_pPlayer = nullptr;
}

void Level::Draw() const
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);;
	glClear(GL_COLOR_BUFFER_BIT);

	m_pCamera->Aim(m_pPlayer->GetBounds());

	m_pCurLevelScreen->Draw();

	m_pCamera->Reset();
}

void Level::Update(float dt)
{
	m_pCurLevelScreen->Update(dt);
}

void Level::LoadLevel(const std::string& name)
{
	if (m_pNextLevelScreen)
		*m_pNextLevelScreen = std::move(*new LevelScreen(name, this));
	else
		m_pNextLevelScreen = new LevelScreen(name, this);

	if (!m_pCurLevelScreen)
	{
		m_pCurLevelScreen = m_pNextLevelScreen;
		m_pNextLevelScreen = nullptr;
	}
	else
	{
		*m_pCurLevelScreen = std::move(*m_pNextLevelScreen); //move assignment
		m_pNextLevelScreen = nullptr;
	}

	m_pCamera->SetTarget(Vector2f{ m_pCurLevelScreen->GetDimensions() }, Vector2f{ GameData::RES_SCALE_X(), GameData::RES_SCALE_Y() });
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
	if (m_pPlayer && pPhysicsBody == m_pPlayer)
	{
		std::ifstream fileStream{ FileIO::OpenTxtFile(srcGate.connectedLevelScreenName, FileIO::Dir::LevelScreenData) };
		if (fileStream)
		{
			std::vector<LevelScreen::Gate> gates{};
			LevelScreen::LoadGates(fileStream, gates);
			//check if a matching gate exists
			if (srcGate.dstGateIdx >= 0 && srcGate.dstGateIdx < gates.size())
			{
				LevelScreen::Gate& dstGate{ gates[srcGate.dstGateIdx] };
				//check if the dstGate is on the opposite side and with equal length to the srcGate
				bool srcGateIsVertical{ int(srcGate.side) % 2 == 0 };
				bool dstGateIsVertical{ int(dstGate.side) % 2 == 0 };
				if (srcGateIsVertical == dstGateIsVertical && srcGate.side != dstGate.side && srcGate.length == dstGate.length)
				{
					LoadLevel(srcGate.connectedLevelScreenName);
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
