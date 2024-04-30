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

	explicit LevelScreen(const std::string& name, Level* pLevel = nullptr);
	~LevelScreen();
	LevelScreen(const LevelScreen& other) = delete;
	LevelScreen& operator=(const LevelScreen& other) = delete;
	LevelScreen(LevelScreen&& other) = delete;
	LevelScreen& operator=(LevelScreen&& other) noexcept;

	void Draw() const;
	void Update(float dt);

	void AddPhysicsBody(PhysicsBody* pPhysicsBody);
	void AddPhysicsBodyThroughGate(PhysicsBody* pPhysicsBody, const Gate& gate);

	CollisionInfo DetectRectCollision(const Rectf& bounds, bool checkXDir = true, bool checkYDir = true, const Vector2f& vel = Vector2f{}, float time = 0.f, bool checkVelDir = false) const;
	CollisionInfo MovePhysicsRect(Rectf& bounds, Vector2f& vel, float time) const;

	Point2f GetDimensions() const;
	int GetWidth() const;
	int GetHeight() const;

	void LoadData();
	void SaveData();
	static void LoadGates(std::ifstream& fStream, std::vector<Gate>& gates);
private:
	//Functions
	CollisionDir GetCollisionDir(const Rectf& bounds, bool checkXDir = true, bool checkYDir = true, const Vector2f& velDist = Vector2f{}, float time = 0.f, bool checkVelDir = false) const;
	void SetCollDirInfo(const Rectf& bounds, const Vector2f& velDist, CollisionInfo& ci) const;
	bool CheckRowCollision(int row, int minCol, int maxCol) const;
	bool CheckCollCollision(int col, int minRow, int maxRow) const;
	bool IsCollisionTile(TileIdx tileIdx) const;
	int PhysicsBodyOverlapsGate(PhysicsBody* pPhysicsBody);

	Rectf GetGateRect(const Gate& gate) const;
	int GetTileID(TileIdx tileIdx) const;
	int GetTileID(int row, int col) const;

	//Members
	std::string m_Name;
	int m_Rows;
	int m_Cols;
	int m_TileSize;
	float m_Width;
	float m_Height;
	int m_PixPerM;
	Level* m_pLevel;
	std::vector<Texture*> m_pTextures;
	std::vector<int> m_IdToTextureIdx;
	std::vector<uint8_t> m_Data;
	std::vector<PhysicsBody*> m_pPhysicsBodies;
	std::vector<Gate> m_Gates;
};

