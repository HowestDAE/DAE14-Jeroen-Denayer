#pragma once
#include <vector>

class LevelScreenGate final
{
public:
	//Left/Right = even/horizontal, Top/Bottom = uneven/vertical
	enum class Side
	{
		Left, Top, Right, Bottom
	};

	LevelScreenGate(const std::string& connectedLevelScreenName, int dstGateIdx, Side side, int startIdx, int length);
	LevelScreenGate(const std::string& string);
	LevelScreenGate(const LevelScreenGate& other) = delete;
	LevelScreenGate& operator=(const LevelScreenGate& other) = delete;
	LevelScreenGate(LevelScreenGate&& other) noexcept;
	LevelScreenGate& operator=(LevelScreenGate&& other) noexcept;

	bool Equals(const LevelScreenGate& rhs) const;
	std::string GetconnectedLevelScreenName() const;
	int GetdstGateIdx() const;
	Side GetSide() const;
	Rectf GetRect(int tileSize, int rows, int cols) const;
	std::string String() const;
private:
	//Members
	std::string m_ConnectedLevelScreenName;
	int m_DstGateIdx;
	Side m_Side;
	static std::vector<std::string> s_SideNames;
	int m_StartIdx; //Start row or column depending on the Side
	int m_Length;
};

bool operator==(const LevelScreenGate& lhs, const LevelScreenGate& rhs);

