#include "pch.h"
#include "LevelScreen.h"
#include "Level.h"
#include "Texture.h"
#include "PhysicsBody.h"
#include <fstream>
#include <sstream>
#include "AssetManager.h"
#include "GameData.h"
#include "FileIO.h"
#include "FallingBlock.h"
#include "DashCrystal.h"
#include "Madeline.h"

LevelScreen::LevelScreen(const std::string& name, Level* pLevel)
	: m_IsValid{ true }
	, m_Name{ name }
	, m_TileSize{ GameData::TILE_SIZE_PIX() }
	, m_PixPerM{ GameData::PIX_PER_M() }
	, m_pPhysicsBodies{ std::vector<PhysicsBody*>{} }
	, m_pPhysicsBodiesOverlapinggates{ std::unordered_map<PhysicsBody*, LevelScreenGate&>{}  }
	, m_pLevel{ pLevel }
	, m_pTextures{ std::vector<Texture*>{} }
	, m_IdToTextureIdx{ std::vector<int>{} }
	, m_Gates{ std::vector<LevelScreenGate>{} }
	, m_pCrystal{ AssetManager::GetTexture("Crystal") }
	, m_CrystalPositions{ std::vector<Vector2f>{} }
{
	LoadData();
}

LevelScreen::~LevelScreen()
{
	for (Texture* pTexture : m_pTextures)
		AssetManager::RemoveTexture(pTexture);

	for (PhysicsBody* pPhysicsBody : m_pPhysicsBodies)
		delete pPhysicsBody;

	AssetManager::RemoveTexture(m_pCrystal);
}

LevelScreen& LevelScreen::operator=(LevelScreen&& other) noexcept
{
	if (this != &other)
	{
		//shallow copy
		m_IsValid = std::move(other.m_IsValid);
		m_Name = std::move(other.m_Name);
		m_Rows = std::move(other.m_Rows);
		m_Cols = std::move(other.m_Cols);
		m_TileSize = std::move(other.m_TileSize);
		m_Width = std::move(other.m_Width);
		m_Height = std::move(other.m_Height);
		m_PixPerM = std::move(other.m_PixPerM);
		m_pLevel = std::move(other.m_pLevel);
		m_IdToTextureIdx = std::move(other.m_IdToTextureIdx);
		//deep copy
		for (Texture* pTexture : m_pTextures)
			AssetManager::RemoveTexture(pTexture);
		m_pTextures = std::move(other.m_pTextures);
		m_Data = std::move(other.m_Data);
		for (PhysicsBody* pPhysicsBody : m_pPhysicsBodies)
		{
			delete pPhysicsBody;
			pPhysicsBody = nullptr;
		}
		m_pPhysicsBodies = std::move(other.m_pPhysicsBodies);
		m_Gates = std::move(other.m_Gates);
		m_CrystalPositions = std::move(other.m_CrystalPositions);
	}
	return *this;
}

void LevelScreen::Draw() const
{
	//Draw Crystals
	Vector2f offset{ m_pCrystal->GetWidth() / 2, m_pCrystal->GetHeight() / 2 };
	for (const Vector2f& pos : m_CrystalPositions)
		m_pCrystal->Draw(Point2f{ pos.x - offset.x, pos.y - offset.y });

	LevelScreen::DrawTiles(m_Data, m_Rows, m_Cols, Vector2f{ 0.f, 0.f }, this);

	//Draw debug grid lines
	//utils::SetColor(Color4f{ 0.25f, 0.25f, 0.25f, 1.f });
	//for (int row{ }; row < m_Rows; ++row)
	//{
	//	float y{ float(row * m_TileSize) };
	//	utils::DrawLine(Point2f{ 0.0f, y }, Point2f{ m_Width, y });
	//}
	//for (int col{}; col < m_Cols; ++col)
	//{
	//	float x{ float(col * m_TileSize) };
	//	utils::DrawLine(Point2f{ x, 0.0f }, Point2f{ x, m_Height });
	//}

	//Draw PhysicsBodies
	for (PhysicsBody* pPhysicsBody : m_pPhysicsBodies)
	{
		pPhysicsBody->Draw(this);
		//Draw overlaprects
		utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
		utils::DrawRect(pPhysicsBody->m_Bounds, 2.f);
		for (PhysicsBody::OverlapRectInfo& overlapRect : pPhysicsBody->m_OverlapRects)
			utils::DrawRect(overlapRect.rect, 2.f);
	}
}

void LevelScreen::DrawTiles(const std::vector<uint8_t>& data, int rows, int cols, const Vector2f& pos, const LevelScreen* ptr)
{
	//Draw tiles
	float tileSize{ float(ptr->m_TileSize) };
	for (int i{}; i < data.size(); ++i)
	{
		TileIdx tileIdx{ utils::GetTileIdxFromIdx(i, rows, cols) };
		int ID{ data[i] };
		float x{ tileIdx.c * tileSize + pos.x };
		float y{ tileIdx.r * tileSize + pos.y };
		Rectf dstRect{ x, y, tileSize, tileSize };
		if (ID >= 0 && ID < ptr->m_IdToTextureIdx.size())
		{
			int textureIdx{ ptr->m_IdToTextureIdx[ID] };
			if (textureIdx >= 0 && textureIdx < ptr->m_pTextures.size())
			{
				Texture* pTexture{ ptr->m_pTextures[textureIdx] };
				if (pTexture)
				{
					Rectf srcRect{ 0.f, 0.f, tileSize, tileSize };
					pTexture->Draw(dstRect, srcRect);
				}
				else //Draw pink rectangle, texture not properly loaded
				{
					utils::SetColor(Color4f{ 1.f, 0.f, 1.f, 1.f });
					utils::FillRect(dstRect);
				}
			}
			//else //Draw black rectangle, ID specifically mapped to invalid texture
			//{
			//	utils::SetColor(Color4f{ 0.f, 0.f, 0.f, 1.f });
			//	utils::FillRect(x, y, float(m_TileSize), float(m_TileSize));
			//}
		}
		else //Draw red rectangle, ID not mapped to textureID in m_IdToTextureIdx
		{
			utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
			utils::FillRect(dstRect);
		}
	}
}

bool LevelScreen::Update(float dt)
{
	bool levelActionRequired{ false };
	m_pPhysicsBodiesOverlapinggates.clear();
	for (std::vector<PhysicsBody*>::iterator it{m_pPhysicsBodies.begin()}; it != m_pPhysicsBodies.end();)
	{
		PhysicsBody* pPhysicsBody{ *it };
		pPhysicsBody->Update(dt); //Derived update
		pPhysicsBody->UpdatePhysics(dt); //Base update
		CollisionInfo bodyCI{ MovePhysicsRect(pPhysicsBody, pPhysicsBody->m_Vel, dt) };
		
		if (pPhysicsBody->m_AlwaysReceiveCollInfo || bodyCI.collided)
			pPhysicsBody->CollisionInfoResponse(0, bodyCI);

		switch (pPhysicsBody->m_Type)
		{
		case PhysicsBody::Type::Madeline:
			//Check if player overlaps a FallingBLock or dash crystal overlap rect
			for (PhysicsBody* pPhysicsBodyOther : m_pPhysicsBodies)
			{
				switch (pPhysicsBodyOther->m_Type)
				{
				case PhysicsBody::Type::FallingBlock:
					//bodyCI = MovePhysicsRect2(pPhysicsBody->m_Bounds, pPhysicsBodyOther->m_Bounds, pPhysicsBody->m_Vel, dt);
					if (utils::IsOverlapping(pPhysicsBody->m_Bounds, pPhysicsBodyOther->m_OverlapRects[0].rect))
						pPhysicsBodyOther->Activate(true);
					break;
				case PhysicsBody::Type::DashCrystal:
					if (pPhysicsBodyOther->m_Active && utils::IsOverlapping(pPhysicsBody->m_Bounds, pPhysicsBodyOther->m_Bounds))
					{
						DashCrystal* pDashCrystal{ static_cast<DashCrystal*>(pPhysicsBodyOther) };
						pDashCrystal->CollisionInfoResponse(0, CollisionInfo{});
						Madeline* pMadeline{ static_cast<Madeline*>(pPhysicsBody) };
						pMadeline->ResetDash();
					}
					break;
				}
			}

			//Check if player overlaps any crystals
			for (Vector2f& pos : m_CrystalPositions)
			{
				Circlef crystal{ Point2f{pos.x, pos.y}, m_pCrystal->GetHeight() / 2 * 0.75f };
				if (utils::IsOverlapping(pPhysicsBody->m_Bounds, crystal))
				{
					pPhysicsBody->m_IsDead = true;
					levelActionRequired = true;
				}
			}

			break;
		}

		//check for collision between overlapRects and with level
		for (int i{}; i < pPhysicsBody->m_OverlapRects.size(); i++)
		{
			PhysicsBody::OverlapRectInfo& overlapRect{ pPhysicsBody->m_OverlapRects[i] };
			if (overlapRect.AllowedPhysicsBodyCollisionType == PhysicsBody::Type::Level)
			{
				CollisionInfo overlapCI{ DetectRectCollision(overlapRect.rect) };
				if (overlapRect.alwaysReceiveCollInfo || overlapCI.collided)
					pPhysicsBody->CollisionInfoResponse(i + 1, overlapCI); // + 1 because 0 is ALWAYS the collision body
			}
		}

		//Check if pPhysicsBody overlaps with any of the gates in this level
		int gateIdx{ PhysicsBodyOverlapsGate(pPhysicsBody) };
		if (gateIdx >= 0 && gateIdx < m_Gates.size())
		{
			m_pPhysicsBodiesOverlapinggates.insert({ pPhysicsBody, m_Gates[gateIdx] });
			levelActionRequired = true;
		}
			
		if (levelActionRequired)
		{
			it = m_pPhysicsBodies.erase(it);
			if (!m_pLevel)
				delete pPhysicsBody;
		}
		else
			++it;
	}
	return levelActionRequired;
}

bool LevelScreen::IsValid() const
{
	return m_IsValid;
}

std::unordered_map<PhysicsBody*, LevelScreenGate&>& LevelScreen::GetPhysicsBodiesOverlapingGates()
{
	return m_pPhysicsBodiesOverlapinggates;
}

void LevelScreen::AddPhysicsBody(PhysicsBody* pPhysicsBody)
{
	m_pPhysicsBodies.push_back(pPhysicsBody);
}

void LevelScreen::AddPhysicsBodyThroughGate(PhysicsBody* pPhysicsBody, const LevelScreenGate& gate)
{
	AddPhysicsBody(pPhysicsBody);
	Rectf gateRect{ gate.GetRect(m_TileSize, m_Rows, m_Cols) };
	Vector2f spawnPos{ gateRect.left, gateRect.bottom };
	bool IsGateVertical{ int(gate.GetSide()) % 2 == 0 };
	if (IsGateVertical)
		spawnPos.x += (gate.GetSide() == LevelScreenGate::Side::Right) ? -(pPhysicsBody->m_Bounds.width + m_TileSize) : m_TileSize;
	else
		spawnPos.y += (gate.GetSide() == LevelScreenGate::Side::Top) ? -(pPhysicsBody->m_Bounds.height + m_TileSize) : m_TileSize;

	pPhysicsBody->SetPosition(spawnPos);
}

CollisionInfo LevelScreen::DetectRectCollision(const Rectf& bounds, bool checkXDir, bool checkYDir, const Vector2f& vel, float time, bool checkVelDir) const
{
	CollisionInfo ci{};
	ci.vIn = utils::GetVelInfo(vel);
	Vector2f velDist{ vel.x * m_PixPerM * time, vel.y * m_PixPerM * time };

	ci.collDir = GetCollisionDir(bounds, checkXDir, checkYDir, velDist, time, checkVelDir);
	if (!ci.collDir.x && !ci.collDir.y && !ci.collDir.corner)
		return ci;

	ci.collided = true;
	Rectf movedBounds{ bounds.left + velDist.x, bounds.bottom + velDist.y, bounds.width, bounds.height };
	SetCollDirInfo(movedBounds, velDist, ci);
	return ci;
}

/*
Checks for collision between a Rectf and a LevelScreen
If a collision was found, the Rectf is moved to an appropriate position
The velocity is also modified!!
*/
CollisionInfo LevelScreen::MovePhysicsRect(PhysicsBody* pPhysicsBody, Vector2f& vel, float time) const
{
	const Rectf bounds{ pPhysicsBody->GetBounds() };
	Point2f prevPos{ bounds.left, bounds.bottom };
	CollisionInfo ci{ DetectRectCollision(bounds, true, true, vel, time, true) };

	//Set position and vel based on collision information
	Vector2f newPos{ bounds.left, bounds.bottom };
	if (ci.collDir.x)
	{
		vel.x = 0.f;
		if (ci.collDir.left && !ci.collDir.right)
			newPos.x = ci.left.collPos;
		else if (ci.collDir.right && !ci.collDir.left)
			newPos.x = ci.right.collPos - bounds.width;
		// else left && right -> squished form both sides
	}
	else
		newPos.x += vel.x * m_PixPerM * time;

	if (ci.collDir.y)
	{
		vel.y = 0.f;
		if (ci.collDir.up && !ci.collDir.down)
			newPos.y = ci.up.collPos - bounds.height;
		else if (ci.collDir.down && !ci.collDir.up)
			newPos.y = ci.down.collPos;
		// else up && down -> squished from both sides
	}
	else
		newPos.y += vel.y * m_PixPerM * time;

	ci.movedDist = Point2f{ newPos.x - prevPos.x, newPos.y - prevPos.y };
	//Changes the actual position of the physicsBody
	pPhysicsBody->SetPosition(newPos);
	return ci;
}

Point2f LevelScreen::GetDimensions() const
{
	return Point2f{ float(GetWidth()), float(GetHeight()) };
}

int LevelScreen::GetWidth() const
{
	return m_Cols * m_TileSize;
}

int LevelScreen::GetHeight() const
{
	return m_Rows * m_TileSize;
}

//##########################
//Level Loading Functions
//##########################

void LevelScreen::LoadData()
{
	//Load texture into m_Data
	m_IsValid = FileIO::LoadTexture(m_Name, m_Data, m_Rows, m_Cols);
	if (!m_IsValid)
		return;

	m_Width = float(m_Cols * m_TileSize);
	m_Height = float(m_Rows * m_TileSize);

	//Load metadata from file
	std::ifstream fileStream{ FileIO::OpenTxtFile(m_Name, FileIO::Dir::LevelScreenData) };

	if (fileStream)
	{
		LevelScreen::LoadGates(fileStream, m_Gates);
		std::vector<std::string> textureNames{};
		FileIO::LoadStringArr(fileStream, textureNames);
		AssetManager::GetTextures(textureNames, m_pTextures);
		FileIO::LoadIntArr(fileStream, m_IdToTextureIdx);
		FileIO::LoadVector2fArr(fileStream, m_CrystalPositions);
		LoadPhysicsBodies(fileStream);
	}
	else
	{
		m_pTextures = std::vector<Texture*>{ AssetManager::GetTexture("") };
		m_IdToTextureIdx = std::vector<int>{ 0 };
	}
}

void LevelScreen::SaveData()
{
	std::stringstream stream;
	stream << "Gates: ";
	WriteGates(stream);
	stream << "\n" << "TextureNames: ";
	std::vector<std::string> textureNames{};
	for (Texture* pTexture : m_pTextures)
		textureNames.push_back(pTexture->GetName());
	FileIO::WriteStringArr(stream, textureNames);
	stream << "\n" << "IdToTextureIdx: ";
	FileIO::WriteIntArr(stream, m_IdToTextureIdx);
	stream << "\n" << "CrystalPositions: ";
	FileIO::WriteVector2fArr(stream, m_CrystalPositions);
	stream << "\n" << "PhysicsBodies: ";
	WritePhysicsBodies(stream);

	std::ofstream oStream{ FileIO::WriteTxtFile(m_Name, FileIO::Dir::LevelScreenData) };
	oStream << stream.str();
}

void LevelScreen::LoadGates(std::ifstream& fStream, std::vector<LevelScreenGate>& gates)
{
	std::string line{};
	std::getline(fStream, line);
	std::stringstream sstream{ line };
	std::string s{};
	sstream >> s; //extract name
	while (sstream.rdbuf()->in_avail()) //nr characters it can still read
	{
		std::string sGate{};
		sstream >> sGate;
		gates.push_back(LevelScreenGate(sGate));
	}
}

void LevelScreen::WriteGates(std::stringstream& sStream)
{
	for (int i{}; i < m_Gates.size(); ++i)
	{
		if (i != 0)
			sStream << " ";
		sStream << m_Gates[i].String();
	}
}

void LevelScreen::LoadPhysicsBodies(std::ifstream& fStream)
{
	std::string line{};
	std::getline(fStream, line);
	std::stringstream sstream{ line };
	std::string s{};
	sstream >> s; //extract name
	while (sstream.rdbuf()->in_avail()) //nr characters it can still read
	{
		std::string sPhysicsBody{};
		sstream >> sPhysicsBody;
		std::stringstream streamPhysicsBody{ sPhysicsBody };
		std::string value{};
		std::getline(streamPhysicsBody, value, ',');
		PhysicsBody::Type type{ PhysicsBody::Type(std::stoi(value)) };
		switch (type)
		{
		case PhysicsBody::Type::Madeline:
			break;
		case PhysicsBody::Type::FallingBlock:
		{
			std::getline(streamPhysicsBody, value, ',');
			int startRow{ std::stoi(value) };
			std::getline(streamPhysicsBody, value, ',');
			int startCol{ std::stoi(value) };
			std::getline(streamPhysicsBody, value, ',');
			int rows{ std::stoi(value) };
			std::getline(streamPhysicsBody, value, ',');
			int cols{ std::stoi(value) };
			TileIdx leftBottomIdx{ startRow, startCol };
			AddFallingBlock(leftBottomIdx, rows, cols);
			break;
		}
		case PhysicsBody::Type::DashCrystal:
		{
			std::getline(streamPhysicsBody, value, ',');
			float left{ std::stof(value) };
			std::getline(streamPhysicsBody, value, ',');
			float bottom{ std::stof(value) };
			AddDashCrystal(Vector2f{ left, bottom });
			break;
		}
		}
	}


	//for (int i{}; i < pPhysicsBodies.size(); ++i)
	//{
	//	PhysicsBody* pPhysicsBody{ pPhysicsBodies[i] };
	//}
}

void LevelScreen::WritePhysicsBodies(std::stringstream& sStream)
{
	for (int i{}; i < m_pPhysicsBodies.size(); ++i)
	{
		if (i != 0)
			sStream << " ";

		PhysicsBody* pPhysicsBody{ m_pPhysicsBodies[i] };
		sStream << pPhysicsBody->String();
	}
}

void LevelScreen::AddCrystal(Vector2f pos)
{
	m_CrystalPositions.push_back(pos);
}

void LevelScreen::AddFallingBlock(TileIdx leftBottomIdx, int rows, int cols)
{
	//Create data
	std::vector<uint8_t> data(size_t(rows * cols));
	for (int i{}; i < data.capacity(); ++i)
	{
		TileIdx tileIdx{ utils::GetTileIdxFromIdx(i, rows, cols) };
		tileIdx.r += leftBottomIdx.r;
		tileIdx.c += leftBottomIdx.c;
		data[i] = GetTileID(tileIdx);
		int dataIdx{ utils::GetIdxFromTileIdx(tileIdx, m_Rows, m_Cols) };
		//Set tile to 0, because the fallingBlock is taking over these tiles
		m_Data[dataIdx] = 0;
	}
	FallingBlock* pFallingBlock{ new FallingBlock{leftBottomIdx, rows, cols, data} };
	AddPhysicsBody(pFallingBlock);
}

void LevelScreen::AddDashCrystal(Vector2f pos)
{
	DashCrystal* pDashCrystal{ new DashCrystal(Vector2f{pos.x, pos.y}) };
	AddPhysicsBody(pDashCrystal);
}


//##########################
//Collision Functions
//##########################

CollisionDir LevelScreen::GetCollisionDir(const Rectf& bounds, bool checkXDir, bool checkYDir, const Vector2f& velDist, float time, bool checkVelDir) const
{
	CollisionDir collDir{};
	VelInfo moving{ utils::GetVelInfo(velDist) };
	bool checkCollInX{ checkXDir && (!checkVelDir || moving.inX) };
	bool checkCollInY{ checkYDir && (!checkVelDir || moving.inY) };

	//Checking left/right
	if (checkCollInX)
	{
		Rectf boundsCopy{ bounds };
		boundsCopy.left += velDist.x;
		RectCornersTileIdx corners{ utils::GetRectCornersTileIdx(boundsCopy, m_TileSize) };
		int minRow{ corners.leftBottom.r };
		int maxRow{ corners.leftTop.r };
		if (!checkVelDir || moving.left)
			collDir.left = CheckCollCollision(corners.leftBottom.c, minRow, maxRow);
		if (!checkVelDir || moving.right)
			collDir.right = CheckCollCollision(corners.rightBottom.c, minRow, maxRow);
		if (collDir.left || collDir.right)
			collDir.x = true;
	}

	//Checking up/down
	if (checkCollInY)
	{
		Rectf boundsCopy{ bounds };
		boundsCopy.bottom += velDist.y;
		RectCornersTileIdx corners{ utils::GetRectCornersTileIdx(boundsCopy, m_TileSize) };
		int minCol{ corners.leftBottom.c };
		int maxCol{ corners.rightBottom.c };
		if (!checkVelDir || moving.down)
			collDir.down = CheckRowCollision(corners.leftBottom.r, minCol, maxCol);
		if (!checkVelDir || moving.up)
			collDir.up = CheckRowCollision(corners.leftTop.r, minCol, maxCol);
		if (collDir.up || collDir.down)
			collDir.y = true;
	}

	//Checking corners
	if (checkCollInX && checkCollInY)
	{
		Rectf boundsCopy{ bounds };
		boundsCopy.left += velDist.x;
		boundsCopy.bottom += velDist.y;
		RectCornersTileIdx corners{ utils::GetRectCornersTileIdx(boundsCopy, m_TileSize) };
		TileIdx cornerInVelDir{
			(moving.up) ? corners.leftTop.r : corners.leftBottom.r,
			(moving.right) ? corners.rightBottom.c : corners.leftBottom.c
		};
		if ((!checkVelDir || corners.leftBottom == cornerInVelDir) && IsCollisionTile(corners.leftBottom))
			collDir.leftBottom = true;
		if ((!checkVelDir || corners.leftTop == cornerInVelDir) && IsCollisionTile(corners.leftTop))
			collDir.leftTop = true;
		if ((!checkVelDir || corners.rightTop == cornerInVelDir) && IsCollisionTile(corners.rightTop))
			collDir.rightTop = true;
		if ((!checkVelDir || corners.rightBottom == cornerInVelDir) && IsCollisionTile(corners.rightBottom))
			collDir.rightBottom = true;
		if (collDir.leftBottom || collDir.leftTop || collDir.rightTop || collDir.rightBottom)
			collDir.corner = true;
	}

	return collDir;
}

void LevelScreen::SetCollDirInfo(const Rectf& bounds, const Vector2f& velDist, CollisionInfo& ci) const
{
	RectCornersTileIdx corners{ utils::GetRectCornersTileIdx(bounds, m_TileSize) };
	//Calculate collision position and entryDist if collision happened in a direction
	if (ci.collDir.left || ci.collDir.leftBottom || ci.collDir.leftTop)
	{
		ci.left.collPos = float((corners.leftBottom.c + 1) * m_TileSize);
		ci.left.entryDist = ci.left.collPos - bounds.left;
		ci.left.lambda = std::min(ci.left.entryDist / std::abs(velDist.x), 1.f);
	}
	if (ci.collDir.right || ci.collDir.rightBottom || ci.collDir.rightTop)
	{
		ci.right.collPos = float(corners.rightBottom.c * m_TileSize);
		ci.right.entryDist = bounds.left + bounds.width - ci.right.collPos;
		ci.right.lambda = std::min(ci.right.entryDist / std::abs(velDist.x), 1.f);
	}
	if (ci.collDir.up || ci.collDir.leftTop || ci.collDir.rightTop)
	{
		ci.up.collPos = float(corners.leftTop.r * m_TileSize);
		ci.up.entryDist = bounds.bottom + bounds.height - ci.up.collPos;
		ci.up.lambda = std::min(ci.up.entryDist / std::abs(velDist.y), 1.f);
	}
	if (ci.collDir.down || ci.collDir.leftBottom || ci.collDir.rightBottom)
	{
		ci.down.collPos = float((corners.leftBottom.r + 1) * m_TileSize);
		ci.down.entryDist = ci.down.collPos - bounds.bottom;
		ci.down.lambda = std::min(ci.down.entryDist / std::abs(velDist.y), 1.f);
	}

	///* If the input rect is moving annd it collided only with a corner,
	//detect which axis collided first */
	if (ci.vIn.inX && ci.vIn.inY && ci.collDir.corner && !ci.collDir.x && !ci.collDir.y) //Convex corner collision
	{
		float lambdaX{ (ci.vIn.right) ? ci.right.lambda : ci.left.lambda };
		float lambdaY{ (ci.vIn.up) ? ci.up.lambda : ci.down.lambda };
		if (lambdaY <= lambdaX && lambdaY > 0.f)
		{
			ci.collDir.y = true;
			ci.collDir.up = ci.vIn.up;
			ci.collDir.down = ci.vIn.down;
		}
		else if (lambdaX < lambdaY && lambdaX > 0.f)//lambdaX > lambdaY
		{
			ci.collDir.x = true;
			ci.collDir.left = ci.vIn.left;
			ci.collDir.right = ci.vIn.right;
		}
	}
}

bool LevelScreen::CheckRowCollision(int row, int minCol, int maxCol) const
{
	for (int col{ minCol }; col < maxCol + 1; ++col)
		if (IsCollisionTile(TileIdx{ row, col }))
			return true;
	return false;
}

bool LevelScreen::CheckCollCollision(int col, int minRow, int maxRow) const
{
	for (int row{ minRow }; row < maxRow + 1; ++row)
		if (IsCollisionTile(TileIdx{ row, col }))
			return true;
	return false;
}

bool LevelScreen::IsCollisionTile(TileIdx tileIdx) const
{
	int tileID{ GetTileID(tileIdx) };
	return tileID > 1;
}

int LevelScreen::PhysicsBodyOverlapsGate(PhysicsBody* pPhysicsBody)
{
	int gateIdx{ -1 }; //invalid gate index
	for (int idx{}; idx < m_Gates.size(); ++idx)
	{
		Rectf gateRect{ m_Gates[idx].GetRect(m_TileSize, m_Rows, m_Cols) };
		if (utils::IsOverlapping(pPhysicsBody->GetBounds(), gateRect))
		{
			gateIdx = idx;
			break; //don't evaluate other gates
		}
	}

	return gateIdx;
}

//##########################
//Utility Functions
//##########################
uint8_t LevelScreen::GetTileID(TileIdx tileIdx) const
{
	return GetTileID(tileIdx.r, tileIdx.c);
}

uint8_t LevelScreen::GetTileID(int row, int col) const
{
	uint8_t tileID{ uint8_t(-1) }; //invalid tileID
	int idx{ utils::GetIdxFromTileIdx(TileIdx{ row, col }, m_Rows, m_Cols) };
	if (idx >= 0 && idx < m_Rows * m_Cols)
		tileID = m_Data[idx];

	return tileID;
}

std::vector<uint8_t> LevelScreen::GetDataBlock(TileIdx leftBottomIdx, int rows, int cols) const
{
	std::vector<uint8_t> data(size_t(rows * cols));
	for (int i{}; i < data.capacity(); ++i)
	{
		TileIdx tileIdx{ utils::GetTileIdxFromIdx(i, rows, cols) };
		tileIdx.r += leftBottomIdx.r;
		tileIdx.c += leftBottomIdx.c;
		data[i] = GetTileID(tileIdx);
	}
	return data;
}
