#include "SpaceConversion.h"


int SpaceConversion::SCREEN_WIDTH = 800;
int SpaceConversion::SCREEN_HEIGHT = 600;

float SpaceConversion::PIXELS_PER_WORLD_UNIT = 50.f;

Vector2d SpaceConversion::CameraWorldPosition = Vector2d();


void SpaceConversion::SetCameraPosition(const Vector2d& newPos)
{
	CameraWorldPosition = newPos;
}


Vector2d ScreenToWorldPosition(sf::Vector2f screenPos)
{
	// Number of pixels from centre of screen
	double x = screenPos.x - SpaceConversion::SCREEN_WIDTH / 2.0;
	double y = SpaceConversion::SCREEN_HEIGHT / 2.0 - screenPos.y;	// invert y direction

	// Number of world units from camera position
	x /= SpaceConversion::PIXELS_PER_WORLD_UNIT;
	y /= SpaceConversion::PIXELS_PER_WORLD_UNIT;

	return SpaceConversion::CameraWorldPosition + Vector2d(x, y);
}


sf::Vector2f WorldToScreenPosition(Vector2d worldPos)
{
	// Number of world units from camera position
	float x = worldPos.x - SpaceConversion::CameraWorldPosition.x;
	float y = SpaceConversion::CameraWorldPosition.y - worldPos.y;	// invert y direction

	// Number of pixels from centre of screen
	x *= SpaceConversion::PIXELS_PER_WORLD_UNIT;
	y *= SpaceConversion::PIXELS_PER_WORLD_UNIT;

	// Add centre of screen co-ordinates
	x += SpaceConversion::SCREEN_WIDTH / 2.f;
	y += SpaceConversion::SCREEN_HEIGHT / 2.f;

	return sf::Vector2f(x, y);
}



double ScreenToWorldLength(float screenLength)
{
	return screenLength / SpaceConversion::PIXELS_PER_WORLD_UNIT;
}


float WorldToScreenLength(double worldLength)
{
	return worldLength * SpaceConversion::PIXELS_PER_WORLD_UNIT;
}

