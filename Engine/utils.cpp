#include "base.h"
//#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <iostream>
#include "utils.h"


#pragma region OpenGLDrawFunctionality
void utils::SetColor( const Color4f& color )
{
	glColor4f( color.r, color.g, color.b, color.a );
}

void utils::DrawPoint( float x, float y, float pointSize )
{
	glPointSize( pointSize );
	glBegin( GL_POINTS );
	{
		glVertex2f( x, y );
	}
	glEnd( );
}

void utils::DrawPoint( const Point2f& p, float pointSize )
{
	DrawPoint( p.x, p.y, pointSize );
}

void utils::DrawPoints( Point2f *pVertices, int nrVertices, float pointSize )
{
	glPointSize( pointSize );
	glBegin( GL_POINTS );
	{
		for ( int idx{ 0 }; idx < nrVertices; ++idx )
		{
			glVertex2f( pVertices[idx].x, pVertices[idx].y );
		}
	}
	glEnd( );
}

void utils::DrawLine( float x1, float y1, float x2, float y2, float lineWidth )
{
	glLineWidth( lineWidth );
	glBegin( GL_LINES );
	{
		glVertex2f( x1, y1 );
		glVertex2f( x2, y2 );
	}
	glEnd( );
}

void utils::DrawLine( const Point2f& p1, const Point2f& p2, float lineWidth )
{
	DrawLine( p1.x, p1.y, p2.x, p2.y, lineWidth );
}

void utils::DrawTriangle(const Point2f& p1, const Point2f& p2, const Point2f& p3, float lineWidth)
{
	glLineWidth(lineWidth);
	glBegin(GL_LINE_LOOP);
	{
		glVertex2f(p1.x, p1.y);
		glVertex2f(p2.x, p2.y);
		glVertex2f(p3.x, p3.y);
	}
	glEnd();
}

void utils::FillTriangle(const Point2f& p1, const Point2f& p2, const Point2f& p3)
{
	glBegin(GL_TRIANGLES);
	{
		glVertex2f(p1.x, p1.y);
		glVertex2f(p2.x, p2.y);
		glVertex2f(p3.x, p3.y);
	}
	glEnd();
}

void utils::DrawRect( float left, float bottom, float width, float height, float lineWidth )
{
	if (width > 0 && height > 0 && lineWidth > 0)
	{

		glLineWidth(lineWidth);
		glBegin(GL_LINE_LOOP);
		{
			glVertex2f(left, bottom);
			glVertex2f(left + width, bottom);
			glVertex2f(left + width, bottom + height);
			glVertex2f(left, bottom + height);
		}
		glEnd();
	}
}

void utils::DrawRect( const Point2f& bottomLeft, float width, float height, float lineWidth )
{
	DrawRect( bottomLeft.x, bottomLeft.y, width, height, lineWidth );
}

void utils::DrawRect( const Rectf& rect, float lineWidth )
{
	DrawRect( rect.left, rect.bottom, rect.width, rect.height, lineWidth );
}

void utils::FillRect( float left, float bottom, float width, float height )
{
	if (width > 0 && height > 0)
	{

		glBegin(GL_POLYGON);
		{
			glVertex2f(left, bottom);
			glVertex2f(left + width, bottom);
			glVertex2f(left + width, bottom + height);
			glVertex2f(left, bottom + height);
		}
		glEnd();
	}
}

void utils::FillRect( const Point2f& bottomLeft, float width, float height )
{
	FillRect( bottomLeft.x, bottomLeft.y, width, height );
}

void utils::FillRect( const Rectf& rect )
{
	FillRect( rect.left, rect.bottom, rect.width, rect.height );
}

void utils::DrawEllipse( float centerX, float centerY, float radX, float radY, float lineWidth )
{
	if (radX > 0 && radY > 0 && lineWidth > 0)
	{

		float dAngle{ radX > radY ? float(g_Pi / radX) : float(g_Pi / radY) };

		glLineWidth(lineWidth);
		glBegin(GL_LINE_LOOP);
		{
			for (float angle = 0.0; angle < float(2 * g_Pi); angle += dAngle)
			{
				glVertex2f(centerX + radX * cos(angle), centerY + radY * sin(angle));
			}
		}
		glEnd();
	}
}

void utils::DrawEllipse( const Point2f& center, float radX, float radY, float lineWidth )
{
	DrawEllipse( center.x, center.y, radX, radY, lineWidth );
}

void utils::DrawEllipse( const Ellipsef& ellipse, float lineWidth  )
{
	DrawEllipse( ellipse.center.x, ellipse.center.y, ellipse.radiusX, ellipse.radiusY, lineWidth );
}

void utils::FillEllipse( float centerX, float centerY, float radX, float radY )
{
	if (radX > 0 && radY > 0)
	{

		float dAngle{ radX > radY ? float(g_Pi / radX) : float(g_Pi / radY) };

		glBegin(GL_POLYGON);
		{
			for (float angle = 0.0; angle < float(2 * g_Pi); angle += dAngle)
			{
				glVertex2f(centerX + radX * cos(angle), centerY + radY * sin(angle));
			}
		}
		glEnd();
	}
}

void utils::FillEllipse( const Ellipsef& ellipse )
{
	FillEllipse( ellipse.center.x, ellipse.center.y, ellipse.radiusX, ellipse.radiusY );
}

void utils::FillEllipse( const Point2f& center, float radX, float radY )
{
	FillEllipse( center.x, center.y, radX, radY );
}

void utils::DrawArc( float centerX, float centerY, float radX, float radY, float fromAngle, float tillAngle, float lineWidth )
{
	if ( fromAngle > tillAngle )
	{
		return;
	}

	float dAngle{ radX > radY ? float( g_Pi / radX ) : float( g_Pi / radY ) };

	glLineWidth( lineWidth );
	glBegin( GL_LINE_STRIP );
	{
		for ( float angle = fromAngle; angle < tillAngle; angle += dAngle )
		{
			glVertex2f( centerX + radX * cos( angle ), centerY + radY * sin( angle ) );
		}
		glVertex2f( centerX + radX * cos( tillAngle ), centerY + radY * sin( tillAngle ) );
	}
	glEnd( );

}

void utils::DrawArc( const Point2f& center, float radX, float radY, float fromAngle, float tillAngle, float lineWidth )
{
	DrawArc( center.x, center.y, radX, radY, fromAngle, tillAngle, lineWidth );
}

void utils::FillArc( float centerX, float centerY, float radX, float radY, float fromAngle, float tillAngle )
{
	if ( fromAngle > tillAngle )
	{
		return;
	}
	float dAngle{ radX > radY ? float( g_Pi / radX ) : float( g_Pi / radY ) };

	glBegin( GL_POLYGON );
	{
		glVertex2f( centerX, centerY );
		for ( float angle = fromAngle; angle < tillAngle; angle += dAngle )
		{
			glVertex2f( centerX + radX * cos( angle ), centerY + radY * sin( angle ) );
		}
		glVertex2f( centerX + radX * cos( tillAngle ), centerY + radY * sin( tillAngle ) );
	}
	glEnd( );
}

void utils::FillArc( const Point2f& center, float radX, float radY, float fromAngle, float tillAngle )
{
	FillArc( center.x, center.y, radX, radY, fromAngle, tillAngle );
}

void utils::DrawPolygon( const std::vector<Point2f>& vertices, bool closed, float lineWidth )
{
	DrawPolygon( vertices.data( ), vertices.size( ), closed, lineWidth );
}

void utils::DrawPolygon( const Point2f* pVertices, size_t nrVertices, bool closed, float lineWidth )
{
	glLineWidth( lineWidth );
	closed ? glBegin( GL_LINE_LOOP ) : glBegin( GL_LINE_STRIP );
	{
		for ( size_t idx{ 0 }; idx < nrVertices; ++idx )
		{
			glVertex2f( pVertices[idx].x, pVertices[idx].y );
		}
	}
	glEnd( );
}

void utils::FillPolygon( const std::vector<Point2f>& vertices )
{
	FillPolygon( vertices.data( ), vertices.size( ) );
}

void utils::FillPolygon( const Point2f *pVertices, size_t nrVertices )
{
	glBegin( GL_POLYGON );
	{
		for ( size_t idx{ 0 }; idx < nrVertices; ++idx )
		{
			glVertex2f( pVertices[idx].x, pVertices[idx].y );
		}
	}
	glEnd( );
}
#pragma endregion OpenGLDrawFunctionality

#pragma region CollisionFunctionality
float utils::GetDistance(float x1, float y1, float x2, float y2)
{
	return (sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));
}

float utils::GetDistance(const Point2f& p1, const Point2f& p2)
{
	return GetDistance(p1.x, p1.y, p2.x, p2.y);
}

bool utils::IsPointInRect( const Point2f& p, const Rectf& r )
{
	return ( p.x >= r.left && 
		p.x <= r.left + r.width &&
		p.y >= r.bottom &&
		p.y <= r.bottom + r.height );
}

bool utils::IsPointInCircle( const Point2f& p, const Circlef& c )
{
	float squaredDist{ (p.x - c.center.x) * (p.x - c.center.x) + (p.y - c.center.y) * (p.y - c.center.y) };
	float squaredRadius{ c.radius * c.radius };
	return ( squaredRadius >= squaredDist );
}

bool utils::IsOverlapping( const Point2f& a, const Point2f& b, const Rectf& r )
{
	// if one of the line segment end points is in the rect
	if ( utils::IsPointInRect( a, r ) || utils::IsPointInRect( b, r ) )
	{
		return true;
	}

	HitInfo hitInfo{};
	Point2f vertices[]{ Point2f {r.left, r.bottom},
		Point2f{ r.left + r.width, r.bottom },
		Point2f{ r.left + r.width, r.bottom + r.height },
		Point2f{ r.left, r.bottom + r.height } };

	return Raycast( vertices, 4, a, b, hitInfo );
}

bool utils::IsOverlapping( const Rectf& r1, const Rectf& r2 )
{
	// If one rectangle is on left side of the other
	//if ( ( r1.left + r1.width ) < r2.left || ( r2.left + r2.width ) < r1.left )
	//{
	//	return false;
	//}

	// If one rectangle is under the other
	//if ( r1.bottom > ( r2.bottom + r2.height ) || r2.bottom > ( r1.bottom + r1.height ) )
	//{
	//	return false;
	//}
	if (r1.left + r1.width < r2.left || r1.left > r2.left + r2.width)
	{
		return false;
	}

	if (r1.bottom + r1.height < r2.bottom || r1.bottom > r2.bottom + r2.height)
	{
		return false;
	}
	return true;
}

bool utils::IsOverlapping( const Rectf& r, const Circlef& c )
{
	// Is center of circle in the rectangle?
	if (IsPointInRect(c.center, r))
	{
		return true;
	}
	// Check line segments
	if (utils::DistPointLineSegment(c.center, Point2f{ r.left, r.bottom }, Point2f{ r.left, r.bottom + r.height }) <= c.radius)
	{
		return true;
	}
	if ( utils::DistPointLineSegment( c.center, Point2f{ r.left, r.bottom }, Point2f{ r.left + r.width, r.bottom } ) <= c.radius )
	{
		return true;
	}
	if (utils::DistPointLineSegment(c.center, Point2f{ r.left + r.width, r.bottom + r.height }, Point2f{ r.left, r.bottom + r.height }) <= c.radius)
	{
		return true;
	}
	if (utils::DistPointLineSegment(c.center, Point2f{ r.left + r.width, r.bottom + r.height }, Point2f{ r.left + r.width, r.bottom }) <= c.radius)
	{
		return true;
	}
	return false;
}

bool utils::IsOverlapping( const Circlef& c1, const Circlef& c2 )
{
	// squared distance between centers
	float xDistance{ c1.center.x - c2.center.x };
	float yDistance{ c1.center.y - c2.center.y };
	float squaredDistance{ xDistance * xDistance + yDistance * yDistance };

	float squaredTouchingDistance{ (c1.radius + c2.radius) * (c1.radius + c2.radius) };
	return (squaredDistance < squaredTouchingDistance);
}

bool utils::IsOverlapping( const Point2f& a, const Point2f& b, const Circlef& c )
{
	return utils::DistPointLineSegment( c.center, a, b ) <= c.radius;
}

bool utils::IsOverlapping( const std::vector<Point2f>& vertices, const Circlef& c )
{
	return IsOverlapping( vertices.data( ), vertices.size( ), c );
}

bool utils::IsOverlapping( const Point2f* vertices, size_t nrVertices, const Circlef& c )
{
	// Verify whether one of vertices is in circle
	for ( size_t i{ 0 }; i < nrVertices; ++i )
	{
		if ( IsPointInCircle( vertices[i], c ) )
		{
			return true;
		}
	}

	// Verify whether one of the polygon edges overlaps with circle
	for ( size_t i{ 0 }; i < nrVertices; ++i )
	{
		if ( DistPointLineSegment( c.center, vertices[i], vertices[( i + 1 ) % nrVertices] ) <= c.radius )
		{
			return true;
		}
	}

	// No overlapping with edges, verify whether circle is completely inside the polygon
	if ( IsPointInPolygon( c.center, vertices, nrVertices ) )
	{
		return true;
	}
	return false;
}

bool utils::IsPointInPolygon( const Point2f& p, const std::vector<Point2f>& vertices )
{
	return IsPointInPolygon( p, vertices.data( ), vertices.size( ) );
}

bool utils::IsPointInPolygon( const Point2f& p, const Point2f* vertices, size_t nrVertices )
{
	if ( nrVertices < 2 )
	{
		return false;
	}
	// 1. First do a simple test with axis aligned bounding box around the polygon
	float xMin{ vertices[0].x };
	float xMax{ vertices[0].x };
	float yMin{ vertices[0].y };
	float yMax{ vertices[0].y };
	for ( size_t idx{ 1 }; idx < nrVertices; ++idx )
	{
		if (xMin > vertices[idx].x)
		{
			xMin = vertices[idx].x;
		}
		if (xMax < vertices[idx].x)
		{
			xMax = vertices[idx].x;
		}
		if (yMin > vertices[idx].y)
		{
			yMin = vertices[idx].y;
		}
		if (yMax < vertices[idx].y)
		{
			yMax = vertices[idx].y;
		}
	}
	if (p.x < xMin || p.x > xMax || p.y < yMin || p.y > yMax)
	{
		return false;
	}

	// 2. Draw a virtual ray from anywhere outside the polygon to the point 
	//    and count how often it hits any side of the polygon. 
	//    If the number of hits is even, it's outside of the polygon, if it's odd, it's inside.
	int numberOfIntersectionPoints{0};
	Point2f p2{ xMax + 10.0f, p.y }; // Horizontal line from point to point outside polygon (p2)

	// Count the number of intersection points
	float lambda1{}, lambda2{};
	for ( size_t i{ 0 }; i < nrVertices; ++i )
	{
		if ( IntersectLineSegments( vertices[i], vertices[( i + 1 ) % nrVertices], p, p2, lambda1, lambda2 ) )
		{
			if ( lambda1 > 0 && lambda1 <= 1 && lambda2 > 0 && lambda2 <= 1 )
			{
				++numberOfIntersectionPoints;
			}
		}
	}
	if (numberOfIntersectionPoints % 2 == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

/*
Calculating lambda1 and lambda 2 is done by considering
p1 as the start of line/ray, and p2 as the end of the line/ray
*/
bool utils::IntersectLineSegments( const Point2f& p1, const Point2f& p2, const Point2f& q1, const Point2f& q2, float& outLambda1, float& outLambda2, float epsilon )
{
	bool intersecting{ false };

	Vector2f p1p2{ p1, p2 };
	Vector2f q1q2{ q1, q2 };

	// Cross product to determine if parallel
	float denom = p1p2.CrossProduct( q1q2 );

	// Don't divide by zero
	if ( std::abs( denom ) > epsilon )
	{
		intersecting = true;

		Vector2f p1q1{ p1, q1 };

		float num1 = p1q1.CrossProduct( q1q2 );
		float num2 = p1q1.CrossProduct( p1p2 );
		outLambda1 = num1 / denom;
		outLambda2 = num2 / denom;
	}
	else // lines are parallel
	{
		// Connect start points
		Vector2f p1q1{ p1, q1 };

		// Cross product to determine if segments and the line connecting their start points are parallel, 
		// if so, than they are on a line
		// if not, then there is no intersection
		if (std::abs( p1q1.CrossProduct(q1q2) ) > epsilon)
		{
			return false;
		}

		outLambda1 = 0;
		outLambda2 = 0;

		//Check all 4 overlapping possibilities
		bool p1OnQ{ utils::IsPointOnLineSegment(p1, q1, q2) };
		bool p2OnQ{ utils::IsPointOnLineSegment(p2, q1, q2) };
		bool q1OnP{ utils::IsPointOnLineSegment(q1, p1, p2) };
		bool q2OnP{ utils::IsPointOnLineSegment(q2, p1, p2) };

		//1st line lies completely on 2nd line
		if (p1OnQ && p2OnQ)
		{
			outLambda1 = 1;
			outLambda2 = 0;
		}
		else
		{
			//Find lambda 1 and 2 based on the min distance p and q overlap, relative to the length of p
			float minDist{};
			//2nd line lies completely on 1st line/ray
			if (q1OnP && q2OnP)
			{
				//Find closest end point of q to p1/rayStart
				float q1p1Dist{ GetDistance(q1, p1) };
				float q2p2Dist{ GetDistance(q2, p2) };
				minDist = std::min(q1p1Dist, q2p2Dist);
			}
			//2 lines partially overlap
			else if ((p1OnQ || p2OnQ) && (q1OnP || q2OnP))
			{
				//Find the end point of q that lies on p
				//This is also the closest point to p1/rayStart
				const Point2f& qOnP{ (q1OnP) ? q1 : q2 };
				minDist = GetDistance(p1, qOnP);
			}
			float p1p2Length{ p1p2.Length() };
			outLambda1 = minDist / p1p2Length;
			outLambda2 = 1 - outLambda1;
		}
		//If outLambda1 and outLambda2 are not 0 then there is an intersection
		if (outLambda1 && outLambda2)
			intersecting = true;
	}
	return intersecting;
}

bool utils::Raycast( const std::vector<Point2f>& vertices, const Point2f& rayP1, const Point2f& rayP2, HitInfo& hitInfo )
{
	return Raycast( vertices.data( ), vertices.size( ), rayP1, rayP2, hitInfo );
}

bool utils::Raycast( const Point2f* vertices, const size_t nrVertices, const Point2f& rayP1, const Point2f& rayP2, HitInfo& hitInfo )
{
	if ( nrVertices == 0 )
	{
		return false;
	}

	std::vector<HitInfo> hits;

	Rectf r1, r2;
	// r1: minimal AABB rect enclosing the ray
	r1.left = std::min( rayP1.x, rayP2.x );
	r1.bottom = std::min( rayP1.y, rayP2.y );
	r1.width = std::max( rayP1.x, rayP2.x ) - r1.left;
	r1.height = std::max( rayP1.y, rayP2.y ) - r1.bottom;

	// Line-line intersections.
	for ( size_t idx{ 0 }; idx <= nrVertices; ++idx )
	{
		// Consider line segment between 2 consecutive vertices
		// (modulo to allow closed polygon, last - first vertice)
		Point2f q1 = vertices[( idx + 0 ) % nrVertices];
		Point2f q2 = vertices[( idx + 1 ) % nrVertices];

		// r2: minimal AABB rect enclosing the 2 vertices
		r2.left = std::min( q1.x, q2.x );
		r2.bottom = std::min( q1.y, q2.y );
		r2.width = std::max( q1.x, q2.x ) - r2.left;
		r2.height = std::max( q1.y, q2.y ) - r2.bottom;

		if ( IsOverlapping( r1, r2 ) )
		{
			float lambda1{};
			float lambda2{};
			if ( IntersectLineSegments( rayP1, rayP2, q1, q2, lambda1, lambda2 ) )
			{
				if ( lambda1 >= 0 && lambda1 <= 1 && lambda2 >= 0 && lambda2 <= 1 )
				{
					HitInfo linesHitInfo{};
					linesHitInfo.lambda = lambda1;
					linesHitInfo.intersectPoint = Point2f{ rayP1.x + ( ( rayP2.x - rayP1.x ) * lambda1 ), rayP1.y + ( ( rayP2.y - rayP1.y ) * lambda1 ) };
					linesHitInfo.normal = Vector2f{ q2 - q1 }.Orthogonal( ).Normalized( );
					hits.push_back(linesHitInfo);
				}
			}
		}
	}

	if ( hits.size( ) == 0 )
	{
		return false;
	}

	// Get closest intersection point and copy it into the hitInfo parameter
	hitInfo = *std::min_element
	( 
		hits.begin( ), hits.end( ),
		[]( const HitInfo& first, const HitInfo& last ) 
		{
			return first.lambda < last.lambda; 
		} 
	);
	return true;
}

bool  utils::IsPointOnLineSegment( const Point2f& p, const Point2f& a, const Point2f& b )
{
	Vector2f ap{ a, p }, bp{ b, p };
	// If not on same line, return false
	if ( abs( ap.CrossProduct( bp ) ) > 0.001f )
	{
		return false;
	}

	// Both vectors must point in opposite directions if p is between a and b
	if ( ap.DotProduct( bp ) > 0 )
	{
		return false;
	}

	return true;
}

float  utils::DistPointLineSegment( const Point2f& p, const Point2f& a, const Point2f& b )
{
	Vector2f ab{ a, b };
	Vector2f ap{ a, p };
	Vector2f abNorm{ ab.Normalized() };
	float distToA{ abNorm.DotProduct(ap) };

	// If distToA is negative, then the closest point is A
	// return the distance a, p
	if ( distToA < 0 )
	{
		return ap.Length( );
	}
	// If distToA is > than dist(a,b) then the closest point is B
	// return the distance b, p
	float distAB{ ab.Length() };
	if ( distToA > distAB )
	{
		return Vector2f{ b, p }.Length( );
	}

	// Closest point is between A and B, calc intersection point
	Vector2f intersection{ abNorm.DotProduct(ap) * abNorm + Vector2f{ a } };
	return Vector2f{ p - intersection }.Length( );
}

bool utils::IntersectRectLine(const Rectf& r, const Point2f& p1, const Point2f& p2, float& intersectMin, float& intersectMax)
{
	// Parameters
	// input: 
	//   r: axis aligned bounding box, start and end points of line segment.
	//   p1, p2: line
	// output: 
	//   intersectMin and intersectMax: in the interval [0,1] if intersection point is on the line segment. 
	// return
	//   true if there is an intersection

	// Example of how to use
	//float min{};
	//float max{};
	//if (utils::IntersectRectLine(rect, p1, p2, min, max))
	//{
	//	Point2f intersectP1{ p1 + (Vector2f(p2) - Vector2f(p1)) * min };
	//	Point2f intersectP2{ p1 + (Vector2f(p2) - Vector2f(p1)) * max };
	//}

	// 4 floats to convert rect space to line space
	// x1: value between 0 and 1 where 0 is on p1 and 1 is on p2, <0 and >1 means intersection is not on line segment
	float x1{ (r.left - p1.x) / (p2.x - p1.x) };
	float x2{ (r.left + r.width - p1.x) / (p2.x - p1.x) };
	float y1{ (r.bottom - p1.y) / (p2.y - p1.y) };
	float y2{ (r.bottom + r.height - p1.y) / (p2.y - p1.y) };

	using std::max; using std::min;
	float tMin{ max(min(x1,x2),min(y1,y2)) };
	float tMax{ min(max(x1,x2), max(y1,y2)) };
	if (tMin > tMax)
	{
		return false;
	}
	intersectMin = tMin;
	intersectMax = tMax;
	return true;
}

Vector2f utils::GetCenter(const Rectf& rect, bool relative)
{
	if (relative)
		return GetCenter(Vector2f{ rect.width, rect.height });
	else
		return Vector2f{ rect.left + rect.width / 2, rect.bottom + rect.height / 2 };
}

Vector2f utils::GetCenter(const Vector2f& topRight)
{
	return Vector2f{ topRight.x / 2, topRight.y / 2 };
}

RectCorners utils::GetRectCorners(const Rectf& rect)
{
	return RectCorners{
		Point2f{rect.left, rect.bottom},
		Point2f{rect.left, rect.bottom + rect.height},
		Point2f{rect.left + rect.width, rect.bottom + rect.height},
		Point2f{rect.left + rect.width, rect.bottom}
	};
}

std::vector<Point2f> utils::GetRectVertices(const Rectf& rect)
{
	RectCorners corners{ GetRectCorners(rect) };
	std::vector<Point2f> verts{};
	verts.push_back(corners.leftBottom);
	verts.push_back(corners.leftTop);
	verts.push_back(corners.rightTop);
	verts.push_back(corners.rightBottom);
	return verts;
}

TileIdx utils::GetTileIdxByPos(const Vector2f& pos, int tileSize, bool offsetTop)
{
	float fRowIdx{ pos.y / tileSize };
	float fColIdx{ pos.x / tileSize };
	int rowIdx{ int(fRowIdx) };
	int colIdx{ int(fColIdx) };
	//Offset row/col so positions that are perfectly on the border of the next row/col
	//are still considered to be in the prev row/col
	if (offsetTop)
	{
		if (fRowIdx - int(fRowIdx) == 0.f)
			--rowIdx;
		if (fColIdx - int(fColIdx) == 0.f)
			--colIdx;
	}
	return TileIdx{ rowIdx, colIdx };
}

RectCornersTileIdx utils::GetRectCornersTileIdx(const Rectf& rect, int tileSize)
{
	TileIdx leftBottomIdx{ GetTileIdxByPos(Vector2f{ rect.left, rect.bottom }, tileSize) };
	TileIdx rightTopIdx{ GetTileIdxByPos(Vector2f{ rect.left + rect.width, rect.bottom + rect.height }, tileSize, true) };
	TileIdx leftTopIdx{ rightTopIdx.r, leftBottomIdx.c };
	TileIdx rightBottomIdx{ leftBottomIdx.r, rightTopIdx.c };
	return RectCornersTileIdx{ leftBottomIdx, leftTopIdx, rightTopIdx, rightBottomIdx };
}

Vector2f utils::GetTileIdxPos(TileIdx tileIdx, int tileSize)
{
	return Vector2f{ float(tileIdx.c * tileSize), float(tileIdx.r * tileSize) };
}

Rectf utils::GetTileRect(TileIdx tileIdx, int tileSize)
{
	Point2f pos{ GetTileIdxPos(tileIdx, tileSize) };
	return Rectf{ pos.x, pos.y, float(tileSize), float(tileSize) };
}

Rectf utils::GetTileAreaRect(TileIdx corner1, TileIdx corner2, int tileSize)
{
	int minRow{ std::min(corner1.r, corner2.r) };
	int maxRow{ std::max(corner1.r, corner2.r) };
	int minCol{ std::min(corner1.c, corner2.c) };
	int maxCol{ std::max(corner1.c, corner2.c) };

	Point2f leftBottom{ GetTileIdxPos(TileIdx{minRow, minCol}, tileSize)};
	Point2f rightTop{ GetTileIdxPos(TileIdx{maxRow, maxCol}, tileSize)};
	return Rectf{ leftBottom.x, leftBottom.y, rightTop.x - leftBottom.x, rightTop.y - leftBottom.y };
}

TileIdx utils::GetTileIdxFromIdx(int idx, int rows, int cols)
{
	TileIdx tileIdx{};
	if (idx < 0 || idx >= rows * cols)
		std::cout << "utils::GetTileIdxFromIdx(): idx out of bounds" << std::endl;
	else
		tileIdx = TileIdx{ idx / cols, idx % cols };
	return tileIdx;
}

int utils::GetIdxFromTileIdx(TileIdx tileIdx, int rows, int cols)
{
	int idx{ -1 }; //invalid idx
	if (tileIdx.r >= 0 && tileIdx.r < rows && tileIdx.c >= 0 && tileIdx.c < cols)
		idx = tileIdx.r * cols + tileIdx.c;
	return idx;
}

/*
Calculate the acc needed to fall x dist in t time
Also calculate the velocity needed to travel x dist with acc acting against object
*/
AccAndVel utils::AccAndVelToTravelDistInTime(float dist, float time)
{
	float acc{ 2 * dist / (time * time) }; //s1 = s0 + v0t + 0.5at^2 => a = 2s1 / t ^ 2
	float vel{ sqrtf(abs(2 * acc * dist)) }; //v1 ^ 2 = v0 ^ 2 + 2a(s1 - s0) => v0 = sqrt(2Gs1)
	return AccAndVel{acc, vel};
}

VelInfo utils::GetVelInfo(const Vector2f& vel)
{
	bool movingLeft{ vel.x < 0.f };
	bool movingRight{ vel.x > 0.f };
	bool movingUp{ vel.y > 0.f };
	bool movingDown{ vel.y < 0.f };
	bool movingInX{ vel.x != 0.f };
	bool movingInY{ vel.y != 0.f };
	return VelInfo{ movingLeft, movingRight, movingUp, movingDown, movingInX, movingInY };
}

/*
Select 3/4 corner tiles based on movement/velocity direction
*/
VelBasedCornerTiles utils::GetVelBasedCornerTiles(const RectCornersTileIdx& corners, const VelInfo& moving)
{
	int bottomRow{ corners.leftBottom.r };
	int topRow{ corners.leftTop.r };
	int leftCol{ corners.leftBottom.c };
	int rightCol{ corners.rightBottom.c };
	TileIdx corner{
		(moving.up) ? topRow : bottomRow,
		(moving.right) ? rightCol : leftCol,
	};
	TileIdx xCorner{ //The corner left or right from the corner
		corner.r,
		(moving.right) ? leftCol : rightCol
	};
	TileIdx yCorner{ //The corner up or down from the corner
		(moving.up) ? bottomRow : topRow,
		corner.c
	};
	return VelBasedCornerTiles{ corner, xCorner, yCorner };
}

#pragma endregion CollisionFunctionality
