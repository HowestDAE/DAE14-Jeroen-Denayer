#pragma once
#include <vector>
#include "Game.h"

//Forward declarations
class Texture;
class PhysicsBody;

class Level final
{
public:
	Level();
	~Level();

	void Draw() const;
	void Update(float dt);
	void AddPhysicsBody(PhysicsBody& physicsBody);
	int GetTileID(TileIdx tileIdx) const;
	int GetTileID(int row, int col) const;
	CollisionInfo DetectRectCollision(const Rectf& bounds, bool checkXDir = true, bool checkYDir = true, const Vector2f& vel = Vector2f{}, float time = 0.f, bool checkVelDir = false) const;
	CollisionInfo MovePhysicsRect(Rectf& bounds, Vector2f& vel, float time) const;
	int GetWidth() const;
	int GetHeight() const;
private:
	//Functions
	bool LoadLevel(const std::string& name);
	void FlipLevel();
	CollisionDir GetCollisionDir(const Rectf& bounds, bool checkXDir = true, bool checkYDir = true, const Vector2f& velDist = Vector2f{}, float time = 0.f, bool checkVelDir = false) const;
	void SetCollDirInfo(const Rectf& bounds, const Vector2f& velDist, CollisionInfo& ci) const;
	bool CheckRowCollision(int row, int minCol, int maxCol) const;
	bool CheckCollCollision(int col, int minRow, int maxRow) const;
	uint8_t GetPixelID(const SDL_Surface* pSurface, int x, int y) const;
	bool IsCollisionTile(TileIdx tileIdx) const;

	//Members
	int m_Rows;
	int m_Cols;
	int m_TileSize;
	float m_Width;
	float m_Height;
	int m_PixPerM;
	std::vector<Texture*> m_pTextures;
	std::vector<int> m_IDToTextureIdxArr;
	std::vector< std::vector<uint8_t> > m_Data;
	std::vector<PhysicsBody*> m_pPhysicsBodies;
};

