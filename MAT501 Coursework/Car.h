#pragma once
#include "Collisions.h"
#include "NeuralNetwork.h"
#include "Vector2d.h"
#include <SFML/Graphics.hpp>


class Racetrack;

class Car
{
public:
	Car(int inputs, Racetrack* pTrack);

	void Update();
	void Render(sf::RenderWindow& window);

	void InsertNewBrain(NeuralNetwork* pBrain) { m_pBrain = pBrain; }
	void StartNewSimulation();

	double GetFitness() { return m_lastRecordedFitness; }
	double TestFitness();
	void CalculateFitness();
	NeuralNetwork GetBrain() { return *m_pBrain; }

private:
	double
		m_ctrlAccel,
		m_ctrlBrake,
		m_ctrlLeft,
		m_ctrlRight;

	Vector2d m_pos;
	Vector2d m_vel;
	double m_bearing;	// in radians; 0.0 => facing right

	std::vector<Ray> m_sensors;
	std::vector<double> m_sensorAngles;
	std::vector<double> m_sensorReadings;

	Vector2d m_hitbox[4];
	double m_length = 0.2;
	double m_width = 0.1;

	const double MAX_ACCEL = 5.0;
	const double MAX_TURN = 0.05;
	const double FORWARD_DRAG = 0.5;
	const double SIDE_DRAG = 3.0;
	const double SKID = 2.0;

	sf::RectangleShape m_body;
	std::vector<sf::Vertex> m_rays;

	NeuralNetwork* m_pBrain;

	Racetrack* m_pTrack;

	int m_laps = 0;
	int m_wasNearStartLine = 0;

	bool m_hasCrashed;

	double m_lastRecordedFitness = 0.0;

private:
	void GetInputs();

};

