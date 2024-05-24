#include "pch.h"
#include "Level.h"
#include "Texture.h"
#include "Madeline.h"
#include "Camera.h"
#include "GameData.h"
#include "FileIO.h"
#include "AssetManager.h"

Level::Level()
	: m_IsValid{ true }
	, m_StartRoom{ "Room4" }
	, m_pCamera{ new Camera() }
	, m_pPlayer{ nullptr }
	, m_pCurLevelScreen{ nullptr }
	, m_pNextLevelScreen{ nullptr }
{
	LoadLevel(m_StartRoom); //hardcoded 1st level atm
	m_IsValid = m_pCurLevelScreen->IsValid();

	//Create the player Madeline
	float madelinePixWidth{ 8 };
	float madelinePixHeight{ 16 };
	AccAndVel madelineJumpAccAndVel{ utils::AccAndVelToTravelDistInTime(3.5f, 0.35f) };
	Point2f pos{ 8 * 8, 4 * 8 }; //hardcoded spawn position
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

	if (m_pCurLevelScreen)
		m_pCurLevelScreen->Draw();

	m_pCamera->Reset();
}

void Level::Update(float dt)
{
	bool actionRequired{};
	if (m_pCurLevelScreen)
		actionRequired = m_pCurLevelScreen->Update(dt);

	if (actionRequired)
	{
		//Check if the player is dead
		if (m_pPlayer->IsDead())
		{
			AssetManager::PlaySoundEffect("Death");
			Reset();
		}
		else
		{
			//Transfer all physics bodies that overlap a gate
			std::unordered_map<PhysicsBody*, LevelScreenGate&>& physicsBodiesOverlappingGates{ m_pCurLevelScreen->GetPhysicsBodiesOverlapingGates() };
			for (auto it{ physicsBodiesOverlappingGates.begin() }; it != physicsBodiesOverlappingGates.end(); ++it)
				TransferPhysicsBody((*it).first, (*it).second);
		}
	}
}

void Level::Reset()
{
	LoadLevel(m_StartRoom); //hardcoded 1st level atm

	m_pCurLevelScreen->AddPhysicsBody(m_pPlayer);
	Point2f pos{ 8 * 8, 4 * 8 }; //hardcoded spawn position
	m_pPlayer->SetIsDead(false);
	m_pPlayer->SetPosition(pos);
}

bool Level::IsValid() const
{
	return m_IsValid;
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
void Level::TransferPhysicsBody(PhysicsBody* pPhysicsBody, const LevelScreenGate& srcGate)
{
	bool deletePhysicsBody{true};
	if (m_pPlayer && pPhysicsBody == m_pPlayer)
	{
		std::ifstream fileStream{ FileIO::OpenTxtFile(srcGate.GetconnectedLevelScreenName(), FileIO::Dir::LevelScreenData)};
		if (fileStream)
		{
			std::vector<LevelScreenGate> gates{};
			LevelScreen::LoadGates(fileStream, gates);
			int dstGateIdx{ srcGate.GetdstGateIdx() };
			//check if a matching gate exists
			if (dstGateIdx >= 0 && dstGateIdx < gates.size())
			{
				LevelScreenGate& dstGate{ gates[dstGateIdx] };
				if (srcGate == dstGate)
				{
					LoadLevel(srcGate.GetconnectedLevelScreenName());
					//Transfer the player to this new level via the dstGate
					m_pCurLevelScreen->AddPhysicsBodyThroughGate(pPhysicsBody, dstGate);
					deletePhysicsBody = false;
				}
			}
		}
	}
	
	if (deletePhysicsBody)
		delete pPhysicsBody;
}
