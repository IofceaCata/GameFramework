//-----------------------------------------------------------------------------
// File: Vec2.h
//
// Desc: Defines vector based co-ords used throughout game.
//		 Overrides operators to allow two entry vector based calculations
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Vec2 Specific Includes
//-----------------------------------------------------------------------------
#include "Vec3.h"
#include "main.h"

Vec3& Vec3::operator-()
{
	x = -x;
	y = -y;
	return *this;
}

bool Vec3::operator==(Vec3 v)
{
	return (x == v.x && y == v.y);
}

bool Vec3::operator!=(Vec3 v)
{
	return (x != v.x || y != v.y);
}

Vec3 Vec3::operator+(Vec3 v)
{
	return Vec3(x + v.x, y + v.y);
}

Vec3 Vec3::operator-(Vec3 v)
{
	return Vec3(x - v.x, y - v.y);
}

Vec3& Vec3::operator+=(Vec3 v)
{
	*this = *this + v;
	return *this;
}

Vec3& Vec3::operator-=(Vec3 v)
{
	*this = *this - v;
	return *this;
}

double Vec3::Magnitude() const
{
	return sqrt(x*x + y * y);
}

double Vec3::Argument() const
{
	if (x < 0.0)
	{
		return PI + atan(y / x);
	}
	else if (fabs(x) < EPS)
	{
		return (y > 0 ? PI : -PI);
	}
	else
	{
		return atan(y / x);
	}
}

double Vec3::Distance(Vec3 v) const // Euclidean distance
{
	double dx = x - v.x;
	double dy = y - v.y;
	return sqrt(dx*dx + dy * dy);
}

Vec3 Polar(double r, double radians)
{
	Vec3 result;
	if (r < 0)
	{
		r = -r;
	}
	radians = PrincipleAngle2(radians);
	result.x = r * cos(radians);
	result.y = r * sin(radians);
	return result;
}

double Vec3::operator*(Vec3 v)  // dot product
{
	return x * v.x + y * v.y;
}

void Vec3::Rotate(double radians)
{
	double xx = cos(radians)*x - sin(radians)*y;
	double yy = sin(radians)*x + cos(radians)*y;
	x = xx;
	y = yy;
}

Vec3 Vec3::operator*(double s) // scale
{
	return Vec3(s*x, s*y);
}

Vec3 Vec3::operator/(double s) // scale
{
	return Vec3(x / s, y / s);
}

double PrincipleAngle2(double radians)
{
	double result = fmod(radians, 2 * PI);
	if (radians > 0)
	{
		return result;
	}
	else
	{
		return 2 * PI + result;
	}
}
