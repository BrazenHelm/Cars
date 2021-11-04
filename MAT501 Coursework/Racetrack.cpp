#include "Racetrack.h"
#include "SpaceConversion.h"

#include <iostream>


Racetrack::Racetrack()
{
	CreateTrainingTrack();
}


void Racetrack::Draw(sf::RenderWindow& window)
{
	window.draw(m_vertices.data(), m_vertices.size(), sf::Lines);
}


double Racetrack::DistanceToWall(Ray ray)
{
	double shortest = 999.0, t = 0.0;
	bool success = false;
	for (LineSegment line : m_boundary)
	{
		if (CheckCollision(ray, line, t) && t < shortest)
		{
			shortest = t;
			success = true;
		}
	}
	if (success) return shortest;
	else return -1.0;
}


bool Racetrack::CollidedWithWall(Vector2d hitbox[4])
{
	Vector2d temp;
	LineSegment front, right, left, back;

	front.start = hitbox[0];
	front.end = hitbox[1];
	for (LineSegment line : m_boundary)
		if (CheckCollision(line, front, temp)) return true;

	right.start = hitbox[1];
	right.end = hitbox[2];
	for (LineSegment line : m_boundary)
		if (CheckCollision(line, right, temp)) return true;

	back.start = hitbox[2];
	back.end = hitbox[3];
	for (LineSegment line : m_boundary)
		if (CheckCollision(line, back, temp)) return true;

	left.start = hitbox[3];
	left.end = hitbox[0];
	for (LineSegment line : m_boundary)
		if (CheckCollision(line, left, temp)) return true;

	return false;
}


double Racetrack::Fitness(Vector2d position)
{
	size_t nPoints = m_scoringLoop.size();

	size_t closestPoint = 0;
	std::vector<double> distances;
	double shortest = 999.0;

	for (size_t i = 0; i < nPoints; i++)
	{
		double distance = (m_scoringLoop[i].start - position).Length();
		distances.push_back(distance);
		if (distance < shortest)
		{
			shortest = distance;
			closestPoint = i;
		}
	}

	size_t prevPoint, nextPoint;
	if (closestPoint == 0) { prevPoint = nPoints - 1; nextPoint = 1; }
	else if (closestPoint == nPoints - 1) { prevPoint = nPoints - 2; nextPoint = 0; }
	else { prevPoint = closestPoint - 1; nextPoint = closestPoint + 1; }

	double score;
	if (distances[prevPoint] < distances[nextPoint])
	{
		score = static_cast<double>(closestPoint)
			- distances[closestPoint] / (distances[prevPoint] + distances[closestPoint]);
	}
	else
	{
		score = static_cast<double>(closestPoint)
			+ distances[closestPoint] / (distances[nextPoint] + distances[closestPoint]);
	}

	return score;
}


int Racetrack::NearStartingLine(Vector2d position)
{
	switch (m_track)
	{
		case SQUARE:
		{
			if (-4.0 < position.y && position.y < -2.0)
			{
				if (1.5 < position.x && position.x <= 2.0)
				{
					return 1;
				}
				if (2.0 < position.x && position.x < 2.5)
				{
					return -1;
				}
			}
			return 0;
		}
		case TEST:
		{
			if (-6.0 < position.y && position.y < -5.0)
			{
				if (2.0 < position.x && position.x <= 2.5)
				{
					return 1;
				}
				if (2.5 < position.x && position.x < 3.0)
				{
					return -1;
				}
			}
			return 0;
		}
		case TRAIN:
		{
			if (-5.0 < position.y && position.y < -4.0)
			{
				if (0.5 < position.x && position.x <= 1.0)
				{
					return 1;
				}
				if (1.0 < position.x && position.x < 1.5)
				{
					return -1;
				}
			}
			return 0;
		}
		default:
		{
			printf("Error in Racetrack::NearStartingLine - invalid track selected\n");
			return 0;
		}
	}
}


void Racetrack::SetTrack(const std::string& name)
{
	//std::cout << "name entered was x" << name << "x" << std::endl;
	if (name == "square")
	{
		CreateSquareTrack();
		printf("Successfully set track to square\n");
	}
	else if (name == "test")
	{
		CreateTestingTrack();
		printf("Successfully set track to test\n");
	}
	else if (name == "train")
	{
		CreateTrainingTrack();
		printf("Successfully set track to train\n");
	}
	else
	{
		printf("Error in Racetrack::SetTrack - Invalid track name entered\n");
		printf("Options are \"square\", \"test\" and \"train\"\n");
	}
}


void Racetrack::AddBoundaryLoop(std::vector<Vector2d> points)
{
	size_t nPoints = points.size();
	for (size_t i = 0; i < nPoints - 1; i++)
	{
		m_boundary.emplace_back(points[i], points[i + 1]);
	}
	m_boundary.emplace_back(points[nPoints - 1], points[0]);
}


void Racetrack::AddScoringLoop(std::vector<Vector2d> points)
{
	size_t nPoints = points.size();
	for (size_t i = 0; i < nPoints - 1; i++)
	{
		m_scoringLoop.emplace_back(points[i], points[i + 1]);
	}
	m_scoringLoop.emplace_back(points[nPoints - 1], points[0]);
}


void Racetrack::ResetTrack()
{
	m_boundary.clear();
	m_scoringLoop.clear();
	m_vertices.clear();
}


void Racetrack::CreateSquareTrack()
{
	ResetTrack();

	std::vector<Vector2d> outerLoop
	{
		{-2.0, -4.0}, {-2.0, 4.0}, {6.0, 4.0}, {6.0, -4.0}
	};
	AddBoundaryLoop(outerLoop);

	std::vector<Vector2d> innerLoop
	{
		{0.0, -2.0}, {0.0, 2.0}, {4.0, 2.0}, {4.0, -2.0}
	};
	AddBoundaryLoop(innerLoop);

	std::vector<Vector2d> scoringLoop
	{
		{-1.0, -3.0}, {-1.0, 3.0}, {5.0, 3.0}, {5.0, -3.0}
	};
	AddScoringLoop(scoringLoop);

	for (LineSegment line : m_boundary)
	{
		m_vertices.emplace_back(WorldToScreenPosition(line.start), sf::Color::White);
		m_vertices.emplace_back(WorldToScreenPosition(line.end), sf::Color::White);
	}

	for (LineSegment line : m_scoringLoop)
	{
		m_vertices.emplace_back(WorldToScreenPosition(line.start), sf::Color::Yellow);
		m_vertices.emplace_back(WorldToScreenPosition(line.end), sf::Color::Yellow);
	}

	m_vertices.emplace_back(WorldToScreenPosition(Vector2d(2.0, -4.0)), sf::Color::Cyan);
	m_vertices.emplace_back(WorldToScreenPosition(Vector2d(2.0, -2.0)), sf::Color::Cyan);

	m_spawnPos = Vector2d(-1.0, -3.0);
	m_spawnBearing = -3.14159 / 2.0;
	m_track = SQUARE;
}


void Racetrack::CreateTestingTrack()
{
	ResetTrack();

	std::vector<Vector2d> outerLoop
	{
		{0,-5}, {-4,-5}, {-6,-4}, {-7,-3}, {-7,-2}, {-6,-1}, {-4,0}, {-4,1}, {-6,2},
		{-6,3}, {-5,5}, {-3,6}, {-1,5}, {-1, 1}, {0,-2}, {2,-2}, {1,-1}, {0,1}, {0,3},
		{2,5}, {4,5}, {5,4}, {4,2}, {5,2}, {6,3}, {7,3}, {8,2}, {8,-1}, {7,-2}, {7,-3},
		{4,-6}, {1,-6}
	};
	AddBoundaryLoop(outerLoop);

	std::vector<Vector2d> innerLoop
	{
		{1,-4}, {-4,-4}, {-5,-3}, {-5,-2}, {-3,-1}, {-3,2}, {-4,3}, {-4,4}, {-3,5},
		{-2,3}, {-2,0}, {-1,-2}, {0,-3}, {3,-3}, {4,-2}, {1,1}, {1,2}, {2,4}, {3,4},
		{2,2}, {3,0}, {5,0}, {6,1}, {7,1}, {7,0}, {6,-1}, {6,-2}, {3,-5}, {2,-5}
	};
	AddBoundaryLoop(innerLoop);

	std::vector<Vector2d> scoringLoop
	{
		{1.5,-5.5}, {0.5,-4.5}, {-1,-4.5}, {-2.5,-4.5}, {-4,-4.5}, {-5.5,-3.5}, {-6,-2.5},
		{-5.5,-1.5}, {-3.5,-0.5}, {-3.5,1.5}, {-4.5,2}, {-5,3.5}, {-4,5}, {-3,5.5},
		{-2,4.5}, {-1.5,3.5}, {-1.5,1.5}, {-1.5,0}, {-1,-1}, {0,-2.5}, {1,-2.5},
		{2.5,-2.5}, {2.5,-1.5}, {1.5,-0.5}, {0.5,1}, {0.5,2.5}, {1,3.5}, {2.5,4.5},
		{4,4}, {3,2.5}, {3.5,1}, {5,1}, {6.5,2}, {7.5,1.5}, {7.5,-0.5}, {6.5,-1.5},
		{6.5,-1.5}, {6.5,-2.5}, {5.5,-3.5}, {4.5,-4.5}, {3.5,-5.5}
	};
	AddScoringLoop(scoringLoop);

	for (LineSegment line : m_boundary)
	{
		m_vertices.emplace_back(WorldToScreenPosition(line.start), sf::Color::White);
		m_vertices.emplace_back(WorldToScreenPosition(line.end), sf::Color::White);
	}

	for (LineSegment line : m_scoringLoop)
	{
		m_vertices.emplace_back(WorldToScreenPosition(line.start), sf::Color::Yellow);
		m_vertices.emplace_back(WorldToScreenPosition(line.end), sf::Color::Yellow);
	}

	m_vertices.emplace_back(WorldToScreenPosition(Vector2d(2.5, -5.0)), sf::Color::Cyan);
	m_vertices.emplace_back(WorldToScreenPosition(Vector2d(2.5, -6.0)), sf::Color::Cyan);

	m_spawnPos = Vector2d(0.0, -4.5);
	m_spawnBearing = 3.14159;
	m_track = TEST;
}


void Racetrack::CreateTrainingTrack()
{
	ResetTrack();

	std::vector<Vector2d> outerLoop
	{
		{-5,-5}, {-7,-2}, {-7,1}, {-4,3}, {-2,1}, {-3,-1}, {-2,-2}, {-1,-1}, {-1,1},
		{-3,3}, {-3,5}, {0,6}, {3,5}, {5,5}, {7,3}, {7,1}, {5,0}, {3,1}, {5,-1}, {7,-2},
		{7,-4}, {6,-6}, {4,-6}, {2,-5}
	};
	AddBoundaryLoop(outerLoop);

	std::vector<Vector2d> innerLoop
	{
		{-4,-4}, {-6,-2}, {-6,0}, {-4,2}, {-3,1}, {-4,-1}, {-3,-3}, {-1,-3}, {1,-1},
		{0,1}, {-2,3}, {-2,4}, {0,5}, {3,4}, {5,4}, {6,3}, {6,2}, {5,1}, {4,2}, {2,3},
		{1,2}, {1,1}, {4,-2}, {5,-2}, {6,-3}, {5,-5}, {2,-4}
	};
	AddBoundaryLoop(innerLoop);

	std::vector<Vector2d> scoringLoop
	{
		{0,-4.5}, {-1.5,-4.5}, {-3,-4.5}, {-4.5,-4.5}, {-6.5,-2}, {-6.5, 0.5}, {-4,2.5},
		{-2.5,1}, {-3.5,-1}, {-2.5,-2.5}, {-1.5,-2.5}, {0,-1}, {-0.5,1}, {-2.5,3},
		{-2.5,4.5}, {0,5.5}, {3,4.5}, {5,4.5}, {6.5,3}, {6.5,1.5}, {5,0.5}, {3.5,1.5},
		{2,1.5}, {3.5,-0.5}, {5,-1.5}, {6.5,-2.5}, {6.5,-3.5}, {5.5,-5.5}, {4.5,-5.5},
		{2,-4.5}
	};
	AddScoringLoop(scoringLoop);

	for (LineSegment line : m_boundary)
	{
		m_vertices.emplace_back(WorldToScreenPosition(line.start), sf::Color::White);
		m_vertices.emplace_back(WorldToScreenPosition(line.end), sf::Color::White);
	}

	for (LineSegment line : m_scoringLoop)
	{
		m_vertices.emplace_back(WorldToScreenPosition(line.start), sf::Color::Yellow);
		m_vertices.emplace_back(WorldToScreenPosition(line.end), sf::Color::Yellow);
	}

	m_vertices.emplace_back(WorldToScreenPosition(Vector2d(1.0, -4.0)), sf::Color::Cyan);
	m_vertices.emplace_back(WorldToScreenPosition(Vector2d(1.0, -5.0)), sf::Color::Cyan);

	m_spawnPos = Vector2d(0.0, -4.5);
	m_spawnBearing = 3.14159;
	m_track = TRAIN;
}

