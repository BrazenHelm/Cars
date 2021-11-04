#include "Collisions.h"
#include <limits>



bool CheckCollision(Ray ray, LineSegment line, double& t)
{
	double
		x1 = ray.origin.x,
		y1 = ray.origin.y,
		x2 = ray.origin.x + ray.dir.x,
		y2 = ray.origin.y + ray.dir.y,
		x3 = line.start.x,
		y3 = line.start.y,
		x4 = line.end.x,
		y4 = line.end.y;

	double d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	if (abs(d) < std::numeric_limits<double>::epsilon()) return false;

	double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / d;
	if (u < 0.0 || u > 1.0) return false;

	double v = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / d;
	if (v < 0.0) return false;
	
	t = v;
	return true;
}


bool CheckCollision(LineSegment first, LineSegment second, Vector2d& point)
{
	double
		x1 = first.start.x,
		y1 = first.start.y,
		x2 = first.end.x,
		y2 = first.end.y,
		x3 = second.start.x,
		y3 = second.start.y,
		x4 = second.end.x,
		y4 = second.end.y;

	double d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	if (abs(d) < std::numeric_limits<double>::epsilon()) return false;

	double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / d;
	if (u < 0.0 || u > 1.0) return false;

	double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / d;
	if (t < 0.0 || t > 1.0) return false;

	point = (1.0 - t) * first.start + t * first.end;
	return true;
}

