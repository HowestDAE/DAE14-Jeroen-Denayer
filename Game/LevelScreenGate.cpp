#include "pch.h"
#include "LevelScreenGate.h"
#include <sstream>

std::vector<std::string> LevelScreenGate::s_SideNames{ "Left", "Top", "Right", "Bottom" };

LevelScreenGate::LevelScreenGate(const std::string& connectedLevelScreenName, int dstGateIdx, Side side, int startIdx, int length)
	: m_ConnectedLevelScreenName{ connectedLevelScreenName }
	, m_DstGateIdx{ dstGateIdx }
	, m_Side{ side }
	, m_StartIdx{ startIdx }
	, m_Length{ length }
{
}

LevelScreenGate::LevelScreenGate(const std::string& string)
{
	std::stringstream sStream{ string };
	std::string value{};
	std::getline(sStream, value, ',');
	m_ConnectedLevelScreenName = value;
	std::getline(sStream, value, ',');
	m_DstGateIdx = std::stoi(value);
	std::getline(sStream, value, ',');
	if (value == "Left")
		m_Side = Side::Left;
	else if (value == "Top")
		m_Side = Side::Top;
	else if (value == "Right")
		m_Side = Side::Right;
	else if (value == "Bottom")
		m_Side = Side::Bottom;
	std::getline(sStream, value, ',');
	m_StartIdx = std::stoi(value);
	std::getline(sStream, value, ',');
	m_Length = std::stoi(value);
}

LevelScreenGate::LevelScreenGate(LevelScreenGate&& other) noexcept
	: m_ConnectedLevelScreenName{ std::move(other.m_ConnectedLevelScreenName) }
	, m_DstGateIdx{ std::move(other.m_DstGateIdx) }
	, m_Side{ std::move(other.m_Side) }
	, m_StartIdx{ std::move(other.m_StartIdx) }
	, m_Length{ std::move(other.m_Length) }
{
}

LevelScreenGate& LevelScreenGate::operator=(LevelScreenGate&& other) noexcept
{
	if (this != &other)
	{
		m_ConnectedLevelScreenName = std::move(other.m_ConnectedLevelScreenName);
		m_DstGateIdx = std::move(other.m_DstGateIdx);
		m_Side = std::move(other.m_Side);
		m_StartIdx = std::move(other.m_StartIdx);
		m_Length = std::move(other.m_Length);
	}
	return *this;
}

bool LevelScreenGate::Equals(const LevelScreenGate& rhs) const
{
	bool lhsIsVertical{ int(m_Side) % 2 == 0};
	bool rhsIsVertical{ int(rhs.m_Side) % 2 == 0 };
	return lhsIsVertical == rhsIsVertical && m_Side != rhs.m_Side && m_Length == rhs.m_Length;
}

std::string LevelScreenGate::GetconnectedLevelScreenName() const
{
	return m_ConnectedLevelScreenName;
}

int LevelScreenGate::GetdstGateIdx() const
{
	return m_DstGateIdx;
}

LevelScreenGate::Side LevelScreenGate::GetSide() const
{
	return m_Side;
}

Rectf LevelScreenGate::GetRect(int tileSize, int rows, int cols) const
{
	TileIdx gateStartIdx{};
	switch (m_Side)
	{
	case Side::Left: case Side::Right:
		gateStartIdx.r = m_StartIdx;
		gateStartIdx.c = (m_Side == Side::Left) ? 0 : cols;
		break;
	case Side::Top: case Side::Bottom:
		gateStartIdx.r = (m_Side == Side::Bottom) ? 0 : rows;
		gateStartIdx.c = m_StartIdx;
		break;
	}

	TileIdx gateEndIdx{ gateStartIdx };
	bool IsGateVertical{ int(m_Side) % 2 == 0 };
	if (IsGateVertical)
		gateEndIdx.r += m_Length;
	else
		gateEndIdx.c += m_Length;
	return Rectf{ utils::GetTileAreaRect(gateStartIdx, gateEndIdx, tileSize) };
}

std::string LevelScreenGate::String() const
{
	std::stringstream stream;
	stream << m_ConnectedLevelScreenName << "," << m_DstGateIdx << "," << s_SideNames[int(m_Side)] << "," << m_StartIdx << "," << m_Length;
	return stream.str();
}

bool operator==(const LevelScreenGate& lhs, const LevelScreenGate& rhs)
{
	return lhs.Equals(rhs);
}
