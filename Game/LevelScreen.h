#pragma once
#include <vector>
#include "Game.h"

//Forward declarations
class PhysicsBody;
class Level;
class Texture;

class LevelScreen final
{
public:
	struct Gate
	{
		//Left/Right = even/horizontal, Top/Bottom = uneven/vertical
		enum class Side
		{
			Left, Top, Right, Bottom
		};

		std::string connectedLevelScreenName;
		int dstGateIdx;
		Side side;
		int startIdx; //Start row or column depending on the Side
		int length;
	};

	struct InitData
	{
		const std::string& filePath;
		Level* pLevel;
		std::vector<LevelScreen::Gate>* pGates;
	};

	LevelScreen(const std::string& name, const InitData& initData);
	~LevelScreen();

	void Draw() const;
	void Update(float dt);
	void AddPhysicsBody(PhysicsBody* pPhysicsBody);
	void AddPhysicsBodyThroughGate(PhysicsBody* pPhysicsBody, const Gate& gate);
	int GetTileID(TileIdx tileIdx) const;
	int GetTileID(int row, int col) const;
	CollisionInfo DetectRectCollision(const Rectf& bounds, bool checkXDir = true, bool checkYDir = true, const Vector2f& vel = Vector2f{}, float time = 0.f, bool checkVelDir = false) const;
	CollisionInfo MovePhysicsRect(Rectf& bounds, Vector2f& vel, float time) const;
	int GetWidth() const;
	int GetHeight() const;
private:
	//Functions
	bool Load(const std::string& filePath);
	void FlipLevel();
	CollisionDir GetCollisionDir(const Rectf& bounds, bool checkXDir = true, bool checkYDir = true, const Vector2f& velDist = Vector2f{}, float time = 0.f, bool checkVelDir = false) const;
	void SetCollDirInfo(const Rectf& bounds, const Vector2f& velDist, CollisionInfo& ci) const;
	bool CheckRowCollision(int row, int minCol, int maxCol) const;
	bool CheckCollCollision(int col, int minRow, int maxRow) const;
	uint8_t GetPixelID(const SDL_Surface* pSurface, int x, int y) const;
	bool IsCollisionTile(TileIdx tileIdx) const;
	int PhysicsBodyOverlapsGate(PhysicsBody* pPhysicsBody);
	Rectf GetGateRect(const Gate& gate);

	//Members
	std::string m_Name;
	std::string m_FilePath;
	int m_Rows;
	int m_Cols;
	int m_TileSize;
	float m_Width;
	float m_Height;
	int m_PixPerM;
	std::vector< std::vector<uint8_t> > m_Data;
	std::vector<PhysicsBody*> m_pPhysicsBodies;

	//Aggregated Members
	Level* m_pLevel;
	std::vector<Gate>* m_pGates;
};

