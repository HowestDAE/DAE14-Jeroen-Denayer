#pragma once
#include <vector>
#include "Game.h"

//Forward declarations
class PhysicsBody;
class Level;

class LevelScreen final
{
public:
	struct Gate
	{
		enum class Orientation
		{
			Horizontal, Vertical
		};

		enum class Side
		{
			Left, Right, Top, Bottom
		};

		enum class XSide
		{
			left, Right
		};

		enum class YSide
		{
			Top, Bottom
		};

		std::string connectedLevelScreenName;
		int connectedGateIdx;
		Orientation orientation;
		Side side;
		int startIdx; //Start row or column depending on the Orientation
		int length;
	};

	struct InitData
	{
		std::string name;
		std::string filePath;
		std::vector<Gate> gates;
	};

	LevelScreen(const InitData& data, Level* pLevel);
	~LevelScreen();

	void Draw() const;
	void Update(float dt);
	void AddPhysicsBody(PhysicsBody* physicsBody);
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
	bool PhysicsBodyOverlapsGate(PhysicsBody* pPhysicsBody);

	//Members
	std::string m_Name;
	std::string m_FilePath;
	Level* m_pLevel;
	int m_Rows;
	int m_Cols;
	int m_TileSize;
	float m_Width;
	float m_Height;
	int m_PixPerM;
	std::vector< std::vector<uint8_t> > m_Data;
	std::vector<Gate> m_Gates;
	std::vector<PhysicsBody*> m_pPhysicsBodies;
};

