#pragma once
#include "Collisions.h"
#include <vector>
#include <SFML/Graphics.hpp>


enum TrackType { SQUARE, TEST, TRAIN };

class Racetrack
{
public:
	Racetrack();

	void Draw(sf::RenderWindow& window);

	double DistanceToWall(Ray ray);

	bool CollidedWithWall(Vector2d hitbox[4]);

	double Fitness(Vector2d position);

	// For detecting when a car crosses the starting line.
	// returns -1 if just before the line, +1 if just after it, 0 otherwise
	int NearStartingLine(Vector2d position);

private:
	std::vector<LineSegment> m_boundary;
	std::vector<LineSegment> m_scoringLoop;
	std::vector<sf::Vertex> m_vertices;

	Vector2d m_spawnPos;
	double m_spawnBearing;
	TrackType m_track;

public:
	Vector2d GetSpawnPosition() { return m_spawnPos; }
	double GetSpawnBearing() { return m_spawnBearing; }
	size_t PointsPerLap() { return m_scoringLoop.size(); }

	void SetTrack(const std::string& name);

private:
	void AddBoundaryLoop(std::vector<Vector2d> points);
	void AddScoringLoop(std::vector<Vector2d> points);
	void ResetTrack();

	void CreateSquareTrack();
	void CreateTestingTrack();
	void CreateTrainingTrack();
};

