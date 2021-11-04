#pragma once
#include "Vector2d.h"


struct Ray
{
	Vector2d origin;
	Vector2d dir;	// should be normalised

	Ray(Vector2d origin = Vector2d::Zero(), Vector2d dir = Vector2d(1.0, 0.0)) :
		origin(origin), dir(dir.Normalise())
	{};
};


struct LineSegment
{
	Vector2d start;
	Vector2d end;

	LineSegment(Vector2d start = Vector2d::Zero(), Vector2d end = Vector2d::Zero()) :
		start(start), end(end)
	{};
};


// Check for collision between the given ray and line segment
// Returns true if there was a collision and false if there wasn't
// If there was a collision, t becomes the distance along the ray
// at which the collision occurred
// If there was not a collision, t is unchanged
bool CheckCollision(Ray ray, LineSegment line, double& t);


// Check for collision between the given line segments
// Returns true if there was a collision and false if there wasn't
// If there was a collision, point becomes the collision point
// If there was not a collision, point is unchanged
bool CheckCollision(LineSegment first, LineSegment second, Vector2d& point);

