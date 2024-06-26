#pragma once

//Forward declarations
class Texture;

class SpriteSheet
{
public:
	SpriteSheet(const std::string& name, int rows, int cols, int numFrames, float frameTime);
	virtual ~SpriteSheet();
	SpriteSheet(const SpriteSheet& other) = delete;
	SpriteSheet& operator=(const SpriteSheet& other) = delete;
	SpriteSheet(SpriteSheet&& other) = delete;
	SpriteSheet& operator=(SpriteSheet&& other) = delete;

	void Draw(const Rectf& dstRect) const;
	virtual void Update(float dt);
	void Flip(bool flipX);
protected:
	Texture* m_pTexture;
	int m_Rows;
	int m_Cols;
	int m_NumFrames;
	int m_FrameSize;
	int m_FrameNr;
	float m_FrameTime;
	float m_AccumulatedFrameTime;
};

