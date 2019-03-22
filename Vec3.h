#pragma once//-----------------------------------------------------------------------------
// File: Vec2.h
//
// Desc: Defines vector based co-ords used throughout game.
//		Overrides operators to allow two entry vector based calculations
//
//-----------------------------------------------------------------------------

#ifndef VEC3_H
#define VEC3_H

class Vec3
{
public:
	double x, y;

	Vec3() : x(0), y(0) { }
	Vec3(double a, double b) { x = a; y = b; }
	Vec3(int a, int b) { x = a; y = b; }
	~Vec3() { };

	Vec3& operator-();

	bool operator==(Vec3 v);
	bool operator!=(Vec3 v);

	Vec3  operator+(Vec3 v);	// +translate
	Vec3  operator-(Vec3 v);	// -translate
	Vec3& operator+=(Vec3 v);	// inc translate
	Vec3& operator-=(Vec3 v);	// dec translate

	double operator*(Vec3 v);		// dot product
	Vec3 operator*(double s);		// scale
	Vec3 operator/(double s);		// scale
	void Rotate(double radians);

	Vec3 Normalize() { return *this * (1 / Magnitude()); }
	double Magnitude() const;			// Polar magnitude
	double Argument() const;			// Polar argument
	double Distance(Vec3 v) const;		// Distance
};

Vec3 Polar2(double r, double radians);
double PrincipleAngle2(double radians);
#endif // VEC2_H

