#include "pch.h"
#include "LevelScreen.h"
#include "Level.h"
#include "Texture.h"
#include "PhysicsBody.h"
#include <fstream>
#include <sstream>
#include "AssetManager.h"

LevelScreen::LevelScreen(const std::string& name, Level* pLevel)
	: m_TileSize{ 8 }
	, m_PixPerM{ 8 }
	, m_pPhysicsBodies{}
	, m_pLevel{ pLevel }
	, m_pTextures{ std::vector<Texture*>{} }
	, m_IdToTextureIdx{ std::vector<int>{} }
	, m_Gates{}
{
	Load(name);

	//Load metadata from file
	const std::string& dir{ AssetManager::GetDir(AssetManager::Dir::LevelScreenData) };
	std::string metadataFilePath{ dir + name + ".txt" };
	std::ifstream metadataFile{ metadataFilePath };

	if (!metadataFile.is_open())
	{
		std::cout << "LevelScreen::LevelScreen(): Couldn't open file: " << metadataFilePath << std::endl;
		Texture* pTexture{ AssetManager::GetTexture("") };
		m_pTextures.push_back(pTexture);
		m_IdToTextureIdx = std::vector<int>{ 0 };
		return;
	}
	else 
	{
		//To-Do: improve reading from file, less hardcoded stuff
		std::string line{};
		int nrLinesToRead{ 2 };
		enum class VariableName
		{
			TextureNames, IdToTextureIdxArr
		};
		VariableName variableName{};
		for (int i = 0; i < nrLinesToRead; i++) {
			std::getline(metadataFile, line);
			std::stringstream stream{ line };
			std::string s{}; //read first part, should be the variable name
			stream >> s;
			if (s == "TextureNames:")
				variableName = VariableName::TextureNames;
			else if (s == "IdToTextureIdx:")
				variableName = VariableName::IdToTextureIdxArr;

			while (stream.rdbuf()->in_avail())
			{
				switch (variableName)
				{
				case VariableName::TextureNames:
				{
					std::string textureName{};
					stream >> textureName;
					Texture* pTexture{ AssetManager::GetTexture(textureName) };
					m_pTextures.push_back(pTexture);
					break;
				}
				case VariableName::IdToTextureIdxArr:
				{
					int idx{};
					stream >> idx;
					m_IdToTextureIdx.push_back(idx);
					break;
				}
				}
			}
		}
		metadataFile.close();
	}

	//Texture* pTexture{ AssetManager::GetTexture(textureName) };
	//m_pTextures.push_back(pTexture);
	//m_IdToTextureIdx = std::vector<int>{ -1, -1, 0, 0, 1 };
}

LevelScreen::~LevelScreen()
{
	for (Texture* pTexture : m_pTextures)
		AssetManager::RemoveTexture(pTexture);

	for (PhysicsBody* pPhysicsBody : m_pPhysicsBodies)
		delete pPhysicsBody;
}

void LevelScreen::Draw() const
{
	for (int i{}; i < m_Data.size(); ++i)
	{
		TileIdx tileIdx{ GetTileIdxFromIdx(i) };
		int ID{ m_Data[i] };
		float x{ float(tileIdx.c * m_TileSize) };
		float y{ float(tileIdx.r * m_TileSize) };
		if (ID >= 0 && ID < m_IdToTextureIdx.size())
		{
			int textureIdx{ m_IdToTextureIdx[ID]};
			if (textureIdx >= 0 && textureIdx < m_pTextures.size())
			{
				Texture* pTexture{ m_pTextures[textureIdx] };
				if (pTexture)
				{
					Rectf srcRect{ 0.f, 0.f, float(m_TileSize), float(m_TileSize) };
					Rectf dstRect{ utils::GetTileRect(tileIdx, m_TileSize) };
					pTexture->Draw(dstRect, srcRect);
				}
				else //Draw pink rectangle, texture not properly loaded
				{
					utils::SetColor(Color4f{ 1.f, 0.f, 1.f, 1.f });
					utils::FillRect(x, y, m_TileSize, m_TileSize);
				}
			}
			else //Draw black rectangle, ID specifically mapped to invalid texture
			{
				utils::SetColor(Color4f{ 0.f, 0.f, 0.f, 1.f });
				utils::FillRect(x, y, m_TileSize, m_TileSize);
			}
		}
		else //Draw red rectangle, ID not mapped to textureID in m_IdToTextureIdx
		{
			utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
			utils::FillRect(x, y, m_TileSize, m_TileSize);
		}
	}

	//Draw debug grid lines
	utils::SetColor(Color4f{ 0.25f, 0.25f, 0.25f, 1.f });
	for (int row{ }; row < m_Rows; ++row)
	{
		float y{ float(row * m_TileSize) };
		utils::DrawLine(Point2f{ 0.0f, y }, Point2f{ m_Width, y });
	}
	for (int col{}; col < m_Cols; ++col)
	{
		float x{ float(col * m_TileSize) };
		utils::DrawLine(Point2f{ x, 0.0f }, Point2f{ x, m_Height });
	}

	//Draw PhysicsBodies
	for (PhysicsBody* pPhysicsBody : m_pPhysicsBodies)
		pPhysicsBody->Draw();
}

void LevelScreen::Update(float dt)
{
	for (std::vector<PhysicsBody*>::iterator it{m_pPhysicsBodies.begin()}; it != m_pPhysicsBodies.end();)
	{
		PhysicsBody* pPhysicsBody{ *it };
		if (!pPhysicsBody) //nullptr
		{
			it = m_pPhysicsBodies.erase(it);
			continue;
		}

		pPhysicsBody->Update(dt);
		//Move the PhysicsBody and check for collision
		pPhysicsBody->UpdatePhysics(dt);
		CollisionInfo bodyCI{ MovePhysicsRect(pPhysicsBody->m_Bounds, pPhysicsBody->m_Vel, dt) };
		pPhysicsBody->CollisionInfoResponse(0, bodyCI);

		//Move all the other overlapRects connected to this PhysicsBody and check for collision
		for (int i{}; i < pPhysicsBody->m_OverlapRects.size(); i++)
		{
			Rectf& overlapRect{ pPhysicsBody->m_OverlapRects[i] };
			overlapRect += bodyCI.movedDist;
			CollisionInfo overlapCI{ DetectRectCollision(overlapRect) };
			pPhysicsBody->CollisionInfoResponse(i + 1, overlapCI); // + 1 because 0 is ALWAYS the collision body
		}

		//Check if pPhysicsBody overlaps with any of the gates in this level
		int gateIdx{ PhysicsBodyOverlapsGate(pPhysicsBody) };
		if (gateIdx >= 0 && gateIdx < m_Gates.size())
		{
			it = m_pPhysicsBodies.erase(it);
			//Notify the level that a physicsBody was removed from this level via a certain gate,
			//level should take care of moving the physicsBody to another levelScreen or delete it
			bool movedToNewLevel{};
			if (m_pLevel) //controlling level should take care of pPHysicsBody
				movedToNewLevel = m_pLevel->TransferPhysicsBody(pPhysicsBody, m_Gates[gateIdx]);
			else //no controlling level so remove pPhysicsBody
				delete pPhysicsBody;

			if (movedToNewLevel) //The class instance was removed because the player moved to another level
				return;
		}
		else
		{
			++it;
		}
	}
}

void LevelScreen::AddPhysicsBody(PhysicsBody* pPhysicsBody)
{
	m_pPhysicsBodies.push_back(pPhysicsBody);
}

void LevelScreen::AddPhysicsBodyThroughGate(PhysicsBody* pPhysicsBody, const Gate& gate)
{
	AddPhysicsBody(pPhysicsBody);
	Rectf gateRect{ GetGateRect(gate) };
	Point2f spawnPos{ gateRect.left, gateRect.bottom };
	bool IsGateVertical{ int(gate.side) % 2 == 0 };
	if (IsGateVertical)
		spawnPos.x += (gate.side == Gate::Side::Right) ? -(pPhysicsBody->m_Bounds.width + m_TileSize) : m_TileSize;
	else
		spawnPos.y += (gate.side == Gate::Side::Top) ? -(pPhysicsBody->m_Bounds.height + m_TileSize) : m_TileSize;

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
CollisionInfo LevelScreen::MovePhysicsRect(Rectf& bounds, Vector2f& vel, float time) const
{
	Point2f prevPos{ bounds.left, bounds.bottom };
	CollisionInfo ci{ DetectRectCollision(bounds, true, true, vel, time, true) };

	//Set position and vel based on collision information
	if (ci.collDir.x)
	{
		vel.x = 0.f;
		if (ci.collDir.left && !ci.collDir.right)
			bounds.left = ci.left.collPos;
		else if (ci.collDir.right && !ci.collDir.left)
			bounds.left = ci.right.collPos - bounds.width;
		// else left && right -> squished form both sides
	}
	else
		bounds.left += vel.x * m_PixPerM * time;

	if (ci.collDir.y)
	{
		vel.y = 0.f;
		if (ci.collDir.up && !ci.collDir.down)
			bounds.bottom = ci.up.collPos - bounds.height;
		else if (ci.collDir.down && !ci.collDir.up)
			bounds.bottom = ci.down.collPos;
		// else up && down -> squished from both sides
	}
	else
		bounds.bottom += vel.y * m_PixPerM * time;

	Point2f newPos{ bounds.left, bounds.bottom };
	ci.movedDist = Point2f{ newPos.x - prevPos.x, newPos.y - prevPos.y };

	return ci;
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

bool LevelScreen::Load(const std::string& name)
{
	bool succes{ false };
	const std::string& dir{ AssetManager::GetDir(AssetManager::Dir::LevelScreenData) };
	std::string path{ dir + name + ".bmp" };
	SDL_Surface* pSurface{ IMG_Load(path.c_str()) };
	if (pSurface == nullptr)
	{
		std::cout << "LevelScreen::Load() Couldn't load: " << path << std::endl;
		return succes;
	}

	succes = true;
	m_Rows = pSurface->h;
	m_Cols = pSurface->w;
	m_Width = float(m_Cols * m_TileSize);
	m_Height = float(m_Rows * m_TileSize);
	//Reserve only necessary space for level
	m_Data = std::vector<uint8_t>(m_Rows * m_Cols);

	for (int i{}; i < m_Rows * m_Cols; ++i)
	{
		TileIdx tileIdx{ GetTileIdxFromIdx(i) };
		uint8_t pixelID{ GetPixelID(pSurface, tileIdx.c, tileIdx.r) };
		m_Data[i] = pixelID;
	}
	FlipLevel();

	return succes;
}

void LevelScreen::FlipLevel()
{
	for (int srcIdx{}; srcIdx < int(m_Rows * m_Cols / 2); ++srcIdx)
	{
		int temp{ m_Data[srcIdx] };
		TileIdx srcTileIdx{ GetTileIdxFromIdx(srcIdx) };
		TileIdx dstTileIdx{ m_Rows - 1 - srcTileIdx.r, srcTileIdx.c };
		int dstIdx{ GetIdxFromTileIdx(dstTileIdx) };
		m_Data[srcIdx] = m_Data[dstIdx];
		m_Data[dstIdx] = temp;
	}
}

uint8_t LevelScreen::GetPixelID(const SDL_Surface* pSurface, int x, int y) const
{
	// Bytes per pixel
	const Uint8 Bpp = pSurface->format->BytesPerPixel;

	/*
	Retrieve the address to a specific pixel
	pSurface->pixels	= an array containing the SDL_Surface' pixels
	pSurface->pitch		= the length of a row of pixels (in bytes)
	X and Y				= the offset on where on the image to retrieve the pixel, (0, 0) is in the upper left corner of the image
	*/
	Uint8* pPixel = (Uint8*)pSurface->pixels + y * pSurface->pitch + x * Bpp;
	return *pPixel;
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
	int gateIdx{ -1 }; //invalid gate
	for (int idx{}; idx < m_Gates.size(); ++idx)
	{
		Gate& gate{ m_Gates[idx] };
		Rectf gateRect{ GetGateRect(gate) };
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

Rectf LevelScreen::GetGateRect(const Gate& gate) const
{
	TileIdx gateStartIdx{};
	switch (gate.side)
	{
	case Gate::Side::Left: case Gate::Side::Right:
		gateStartIdx.r = gate.startIdx;
		gateStartIdx.c = (gate.side == Gate::Side::Left) ? 0 : m_Cols;
		break;
	case Gate::Side::Top: case Gate::Side::Bottom:
		gateStartIdx.r = (gate.side == Gate::Side::Bottom) ? 0 : m_Rows;
		gateStartIdx.c = gate.startIdx;
		break;
	}

	TileIdx gateEndIdx{ gateStartIdx };
	bool IsGateVertical{ int(gate.side) % 2 == 0 };
	if (IsGateVertical)
		gateEndIdx.r += gate.length;
	else
		gateEndIdx.c += gate.length;
	return Rectf{ utils::GetTileAreaRect(gateStartIdx, gateEndIdx, m_TileSize) };
}
 
TileIdx LevelScreen::GetTileIdxFromIdx(int idx) const
{
	TileIdx tileIdx{};
	if (idx < 0 || idx >= m_Rows * m_Cols)
		std::cout << "LevelScreen::GetRowAndCol(int idx): idx out of bounds" << std::endl;
	else
		tileIdx = TileIdx{ idx / m_Cols, idx % m_Cols };
	return tileIdx;
}

int LevelScreen::GetIdxFromTileIdx(TileIdx tileIdx) const
{
	int idx{ -1 }; //invalid idx
	if (tileIdx.r >= 0 && tileIdx.r < m_Rows && tileIdx.c >= 0 && tileIdx.c < m_Cols)
		idx = tileIdx.r * m_Cols + tileIdx.c;
	return idx;
}

int LevelScreen::GetTileID(TileIdx tileIdx) const
{
	return GetTileID(tileIdx.r, tileIdx.c);
}

int LevelScreen::GetTileID(int row, int col) const
{
	int tileID{ -1 }; //invalid tileID
	int idx{ GetIdxFromTileIdx(TileIdx{ row, col }) };
	if (idx >= 0 && idx < m_Rows * m_Cols)
		tileID = m_Data[idx];

	return tileID;
}