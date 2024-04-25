#include "pch.h"
#include "Level.h"
#include "Texture.h"
#include "Madeline.h"
#include "Camera.h"

Level::Level(const Rectf& viewport)
	: m_pCamera{ nullptr }
	, m_pPlayer{ nullptr }
	, m_pCurLevelScreen{ nullptr }
{
	//Store data needed for loading all the levels
	//To-DO: hardcoded atm, levelScreen should load it's own data from a file using only the levelScreenName
	m_LevelScreensData = std::unordered_map<std::string, LevelScreenData>{
		{"MainRoom",
			{
				"LevelScreensData/MainRoom.bmp",
				std::vector<LevelScreen::Gate>{
					{"SmallRoom", 0, LevelScreen::Gate::Side::Left, 6, 3},
					{"SmallRoom", 1, LevelScreen::Gate::Side::Left, 50, 3},
				}
			}
		},
		{"SmallRoom",
			{
				"LevelScreensData/SmallRoom.bmp",
				std::vector<LevelScreen::Gate>{
					{"MainRoom", 0, LevelScreen::Gate::Side::Right, 2, 3},
					{"MainRoom", 1, LevelScreen::Gate::Side::Right, 25, 3},
				}
			}
		},
	};
	LoadLevel("MainRoom");

	//Create the player Madeline
	float madelinePixWidth{ 8 };
	float madelinePixHeight{ 16 };
	AccAndVel madelineJumpAccAndVel{ utils::AccAndVelToTravelDistInTime(3.5f, 0.35f) };
	Point2f pos{ 8 * 8, 2 * 8 };
	m_pPlayer = new Madeline(pos, madelinePixWidth, madelinePixHeight);
	m_pCurLevelScreen->AddPhysicsBody(m_pPlayer);

	//Level parameters
	float G = -madelineJumpAccAndVel.acc;
	float TILE_SIZE_PIX = 8;
	float WINDOW_NUM_TILES_X = 40.f;
	float WINDOW_NUM_TILES_Y = 22.5f;
	float SCREEN_WIDTH = viewport.width;
	float SCREEN_HEIGHT = viewport.height;
	float RENDER_RES_X = TILE_SIZE_PIX * WINDOW_NUM_TILES_X;
	float RENDER_RES_Y = TILE_SIZE_PIX * WINDOW_NUM_TILES_Y;
	float RES_SCALE_X = SCREEN_WIDTH / RENDER_RES_X;
	float RES_SCALE_Y = SCREEN_HEIGHT / RENDER_RES_Y;
	float PIX_PER_M = TILE_SIZE_PIX;

	//Create the camera
	m_pCamera = new Camera(Point2f{ RENDER_RES_X, RENDER_RES_Y }, Point2f{ RES_SCALE_X, RES_SCALE_Y });
}

Level::~Level()
{
	delete m_pCamera;
	delete m_pCurLevelScreen;

	m_pPlayer = nullptr;
}

void Level::Draw() const
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	//Upscale to screen resolution
	glPushMatrix();
	const Point2f& resScale{ m_pCamera->GetResolutionScale() };
	glScalef(resScale.x, resScale.y, 1);

	Rectf playerBounds{ m_pPlayer->GetBounds() };
	Point2f pos{ playerBounds.left + playerBounds.width / 2, playerBounds.bottom + playerBounds.height / 2 };
	Camera::TrackingInfo trackignInfo{ m_pCurLevelScreen->GetWidth(), m_pCurLevelScreen->GetHeight(), pos };
	m_pCamera->Aim(trackignInfo);
	m_pCurLevelScreen->Draw();

	m_pCamera->Reset();
	glPopMatrix(); //Release upscaling matrix
}

void Level::Update(float dt)
{
	m_pCurLevelScreen->Update(dt);
}

void Level::LoadLevel(const std::string& name)
{
	if (m_pCurLevelScreen) //delete existing levelScreen
		delete m_pCurLevelScreen;

	m_pCurLevelScreen = new LevelScreen(name, this);
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
