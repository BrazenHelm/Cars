#pragma once
#include <math.h>

struct Vector2d
{
	double x, y;

	Vector2d(double x = 0.0, double y = 0.0) :
		x(x), y(y)
	{};

	static Vector2d Zero() { return Vector2d(); }

	inline double Length() { return sqrt(x * x + y * y); }
	inline Vector2d& Normalise() {
		double len = Length(); x /= len; y /= len; return *this;
	}
};


inline Vector2d operator+(const Vector2d& lhs, const Vector2d& rhs)
{
	return Vector2d(lhs.x + rhs.x, lhs.y + rhs.y);
}


inline Vector2d operator-(const Vector2d& lhs, const Vector2d& rhs)
{
	return Vector2d(lhs.x - rhs.x, lhs.y - rhs.y);
}


inline double Dot(const Vector2d& lhs, const Vector2d& rhs)
{
	return (lhs.x * rhs.x + lhs.y * rhs.y);
}


inline Vector2d operator*(const double& scalar, const Vector2d& vector)
{
	return Vector2d(scalar * vector.x, scalar * vector.y);
}