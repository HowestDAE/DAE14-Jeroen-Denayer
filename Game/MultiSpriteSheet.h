#pragma once
#include "SpriteSheet.h"
#include <unordered_map>

class MultiSpriteSheet final
	:public SpriteSheet
{
public:
	struct SpriteSheetInfo
	{
		int startFrameNr;
		int endFrameNr;
		float frameTime;
	};

	explicit MultiSpriteSheet(const std::string& texturePath, int rows, int cols, std::unordered_map<std::string, SpriteSheetInfo> spriteSheetMapping);
	
	virtual void Update(float dt) override;
	void SetSpriteSheetName(const std::string& name);
private:
	std::unordered_map<std::string, SpriteSheetInfo> m_Mapping;
	std::string m_CurrentSpriteSheetName;
};

