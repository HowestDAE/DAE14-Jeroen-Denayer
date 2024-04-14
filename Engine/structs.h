#pragma once
#include <string>

struct Window
{
	explicit Window( const std::string& title = "Title", float width = 320.0f, 
		float height = 180.0f, bool isVSyncOn = true );

	std::string title;
	float width;
	float height;
	bool isVSyncOn;
};

struct Point2f
{
	Point2f( );
	explicit Point2f( float x, float y );

	float x;
	float y;
};

struct Rectf
{
	Rectf( );
	explicit Rectf( float left, float bottom, float width, float height );

	float left;
	float bottom;
	float width;
	float height;
};

struct Color4f
{
	Color4f( );
	explicit Color4f( float r, float g, float b, float a );
	
	float r;
	float g;
	float b;
	float a;
};

struct Circlef
{
	Circlef( );
	explicit Circlef( const Point2f& center, float radius );
	explicit Circlef( float centerX, float centerY, float radius );

	Point2f center;
	float radius;
};

struct Ellipsef
{
	Ellipsef( );
	explicit Ellipsef( const Point2f& center, float radiusX, float radiusY );
	explicit Ellipsef( float centerX, float centerY, float radiusX, float radiusY );

	Point2f center;
	float radiusX;
	float radiusY;
};

//New structs
struct RectCorners
{
	Point2f leftBottom;
	Point2f leftTop;
	Point2f rightTop;
	Point2f rightBottom;
};

struct TileIdx
{
	int r; //row
	int c; //column
};
bool operator==(const TileIdx& lhs, const TileIdx& rhs);

struct RectCornersTileIdx
{
	TileIdx leftBottom;
	TileIdx leftTop;
	TileIdx rightTop;
	TileIdx rightBottom;
};

struct VelBasedCornerTiles
{
	TileIdx corner;
	TileIdx xCorner;
	TileIdx yCorner;
};

struct AccAndVel
{
	float acc;
	float vel;
};

struct Vector2i
{
	int x;
	int y;
};

struct BoolXY
{
	bool x;
	bool y;
};

struct VelInfo
{
	bool left;
	bool right;
	bool up;
	bool down;
	bool inX;
	bool inY;
};

struct CollisionDir
{
	bool x;
	bool left;
	bool right;
	bool y;
	bool up;
	bool down;
	bool corner;
	bool leftBottom;
	bool leftTop;
	bool rightTop;
	bool rightBottom;
};

struct CollisionDirInfo
{
	float collPos;
	float entryDist;
	float lambda;
};

struct CollisionInfo
{
	bool collided;
	VelInfo vIn;
	CollisionDir collDir;
	CollisionDirInfo left;
	CollisionDirInfo right;
	CollisionDirInfo up;
	CollisionDirInfo down;
};