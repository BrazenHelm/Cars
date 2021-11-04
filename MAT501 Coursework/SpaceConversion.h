#pragma once
#include "Vector2d.h"
#include <SFML/System.hpp>

struct SpaceConversion
{
	static int SCREEN_WIDTH, SCREEN_HEIGHT;
	static float PIXELS_PER_WORLD_UNIT;

	static Vector2d CameraWorldPosition;
	static void SetCameraPosition(const Vector2d& newPos);
};

Vector2d ScreenToWorldPosition(sf::Vector2f screenPos);
sf::Vector2f WorldToScreenPosition(Vector2d worldPos);

double ScreenToWorldLength(float screenLength);
float WorldToScreenLength(double worldLength);
