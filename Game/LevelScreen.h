#pragma once
#include <vector>
#include <unordered_map>
#include "Game.h"
#include "LevelScreenGate.h"

//Forward declarations
class PhysicsBody;
class Level;
class Texture;

class LevelScreen final
{
public:
	explicit LevelScreen(const std::string& name, Level* pLevel = nullptr);
	~LevelScreen();
	LevelScreen(const LevelScreen& other) = delete;
	LevelScreen& operator=(const LevelScreen& other) = delete;
	LevelScreen(LevelScreen&& other) = delete;
	LevelScreen& operator=(LevelScreen&& other) noexcept;

	void Draw() const;
	static void DrawTiles(const std::vector<uint8_t>& data, int rows, int cols, const Vector2f& pos, const LevelScreen* ptr);
	bool Update(float dt);
	bool IsValid() const;
	std::unordered_map<PhysicsBody*, LevelScreenGate&>& GetPhysicsBodiesOverlapingGates();

	void AddPhysicsBody(PhysicsBody* pPhysicsBody);
	void AddPhysicsBodyThroughGate(PhysicsBody* pPhysicsBody, const LevelScreenGate& gate);

	CollisionInfo DetectRectCollision(const Rectf& bounds, bool checkXDir = true, bool checkYDir = true, const Vector2f& vel = Vector2f{}, float time = 0.f, bool checkVelDir = false) const;
	CollisionInfo MovePhysicsRect(Rectf& bounds, Vector2f& vel, float time) const;

	Point2f GetDimensions() const;
	int GetWidth() const;
	int GetHeight() const;

	void LoadData();
	void SaveData();
	static void LoadGates(std::ifstream& fStream, std::vector<LevelScreenGate>& gates);
	void WriteGates(std::stringstream& sStream);
	void LoadPhysicsBodies(std::ifstream& fStream);
	void WritePhysicsBodies(std::stringstream& sStream);
	void AddCrystal(Vector2f pos);
	void AddFallingBlock(TileIdx leftBottomIdx, int rows, int cols);
private:
	//Functions
	CollisionDir GetCollisionDir(const Rectf& bounds, bool checkXDir = true, bool checkYDir = true, const Vector2f& velDist = Vector2f{}, float time = 0.f, bool checkVelDir = false) const;
	void SetCollDirInfo(const Rectf& bounds, const Vector2f& velDist, CollisionInfo& ci) const;
	bool CheckRowCollision(int row, int minCol, int maxCol) const;
	bool CheckCollCollision(int col, int minRow, int maxRow) const;
	bool IsCollisionTile(TileIdx tileIdx) const;
	int PhysicsBodyOverlapsGate(PhysicsBody* pPhysicsBody);

	uint8_t GetTileID(TileIdx tileIdx) const;
	uint8_t GetTileID(int row, int col) const;
	std::vector<uint8_t> GetDataBlock(TileIdx leftBottomIdx, int rows, int cols) const;

	//Members
	bool m_IsValid;
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
	std::unordered_map<PhysicsBody*, LevelScreenGate&> m_pPhysicsBodiesOverlapinggates;
	std::vector<LevelScreenGate> m_Gates;
	Texture* m_pCrystal;
	std::vector<Vector2f> m_CrystalPositions;
};

