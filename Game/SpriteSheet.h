#pragma once

//forward declarations
class Texture;

class SpriteSheet
{
public:
	SpriteSheet(const std::string& texturePath, int rows, int cols, int numFrames, float frameTime);
	virtual ~SpriteSheet();

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

