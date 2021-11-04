#include "Car.h"
#include "Params.h"
#include "Racetrack.h"
#include "Random.h"
#include "SpaceConversion.h"
#include "Utils.h"
//#include "Utils.h"
#include <SFML/Window/Keyboard.hpp>


Car::Car(int inputs, Racetrack* pTrack) :
	m_bearing(0.0),
	m_pTrack(pTrack)
{
	// set up sensors
	m_sensors.resize(inputs);
	m_sensorReadings.resize(inputs);
	m_rays.resize(inputs * 2);

	const double LEFT_ANGLE = -3.14159 / 2.0;
	const double RIGHT_ANGLE = 3.14159 / 2.0;
	const double ANGLE_STEP = (RIGHT_ANGLE - LEFT_ANGLE) / (double)(inputs * 2);

	for (int i = 0; i < inputs; i++)
	{
		m_sensorAngles.push_back(LEFT_ANGLE + ANGLE_STEP * (double)(1 + 2 * i));
		m_rays[2 * i].color = sf::Color(100, 100, 100);
		m_rays[2 * i + 1].color = sf::Color(100, 100, 100);
	}

	// set up body
	m_body.setSize(sf::Vector2f(WorldToScreenLength(m_length), WorldToScreenLength(m_width)));
	m_body.setOrigin(WorldToScreenLength(m_length/2.0), WorldToScreenLength(m_width/2.0));
	m_body.setFillColor(sf::Color::White);
}


void Car::Update()
{
	if (m_hasCrashed) return;

	GetInputs();

	double accel = (m_ctrlAccel - m_ctrlBrake) * MAX_ACCEL;
	double right = (m_ctrlRight - m_ctrlLeft) * MAX_TURN;

	m_bearing += right;
	Vector2d dir(cos(m_bearing), -sin(m_bearing));
	Vector2d perp(-sin(m_bearing), -cos(m_bearing));

	double forwardSpeed = Dot(dir, m_vel);
	double sidewaysSpeed = Dot(perp, m_vel);
	
	double forwardAccel = accel - forwardSpeed * FORWARD_DRAG;
	double sidewaysAccel = 0.0 - sidewaysSpeed * SIDE_DRAG - Sign(sidewaysSpeed) * SKID;

	forwardSpeed += forwardAccel / Params::TICK_RATE;
	if (forwardSpeed < 0.1) forwardSpeed = 0.1;
	sidewaysSpeed += sidewaysAccel / Params::TICK_RATE;

	m_vel = forwardSpeed * dir + sidewaysSpeed * perp;

	m_pos.x += m_vel.x / Params::TICK_RATE;
	m_pos.y += m_vel.y / Params::TICK_RATE;

	// Check for collisions
	m_hitbox[0] =  m_length / 2.0 * dir + m_width / 2.0 * perp + m_pos;
	m_hitbox[1] =  m_length / 2.0 * dir - m_width / 2.0 * perp + m_pos;
	m_hitbox[2] = -m_length / 2.0 * dir - m_width / 2.0 * perp + m_pos;
	m_hitbox[3] = -m_length / 2.0 * dir + m_width / 2.0 * perp + m_pos;

	if (m_pTrack->CollidedWithWall(m_hitbox))
	{
		m_hasCrashed = true;
		m_body.setFillColor(sf::Color::Blue);
	}

	// Update sensor positions
	for (size_t i = 0; i < m_sensorAngles.size(); i++)
	{
		m_sensors[i].origin = m_pos;
		m_sensors[i].dir = Vector2d(
			cos(m_bearing + m_sensorAngles[i]), -sin(m_bearing + m_sensorAngles[i])
		);
	}

	// Update laps count
	int nearStartLine = m_pTrack->NearStartingLine(m_pos);
	if (nearStartLine == 1 && m_wasNearStartLine == -1)
	{
		m_laps++;
	}
	if (nearStartLine == -1 && m_wasNearStartLine == 1)
	{
		m_laps--;
	}
	m_wasNearStartLine = nearStartLine;
}


void Car::Render(sf::RenderWindow& window)
{
	m_body.setPosition(WorldToScreenPosition(m_pos));
	m_body.setRotation(m_bearing * 180.0 / 3.14159);
	window.draw(m_body);

	if (!m_hasCrashed)
	{
		for (size_t i = 0; i < m_sensors.size(); i++)
		{
			m_rays[2 * i].position = WorldToScreenPosition(m_sensors[i].origin);
			m_rays[2 * i + 1].position = WorldToScreenPosition(
				m_sensors[i].origin + exp(m_sensorReadings[i]) * m_sensors[i].dir);
		}
		window.draw(m_rays.data(), m_rays.size(), sf::Lines);
	}
}


void Car::StartNewSimulation()
{
	m_pos = m_pTrack->GetSpawnPosition();
	m_bearing = m_pTrack->GetSpawnBearing();
	m_vel.x = 0.0; m_vel.y = 0.0;
	m_hasCrashed = false;
	m_wasNearStartLine = 0;
	m_laps = 0;
	m_body.setFillColor(sf::Color::Red);

	// Update sensor positions
	for (size_t i = 0; i < m_sensorAngles.size(); i++)
	{
		m_sensors[i].origin = m_pos;
		m_sensors[i].dir = Vector2d(
			cos(m_bearing + m_sensorAngles[i]), -sin(m_bearing + m_sensorAngles[i])
		);
	}
}


void Car::CalculateFitness()
{
	const double CRASH_PENALTY = 0.0;

	m_lastRecordedFitness = m_pTrack->Fitness(m_pos)
		+ static_cast<double>(m_laps) * m_pTrack->PointsPerLap()
		- ((m_hasCrashed) ? CRASH_PENALTY : 0);
}


double Car::TestFitness()
{
	const double CRASH_PENALTY = 0.0;

	return m_pTrack->Fitness(m_pos)
		+ static_cast<double>(m_laps) * m_pTrack->PointsPerLap()
		- ((m_hasCrashed) ? CRASH_PENALTY : 0);
}


void Car::GetInputs()
{
	for (size_t i = 0; i < m_sensorReadings.size(); i++)
	{
		m_sensorReadings[i] = log(m_pTrack->DistanceToWall(m_sensors[i]));
	}

	auto brainOutputs = m_pBrain->Process(m_sensorReadings);

	m_ctrlAccel = brainOutputs[0];
	m_ctrlBrake = brainOutputs[1];
	m_ctrlLeft  = brainOutputs[2];
	m_ctrlRight = brainOutputs[3];

	//m_inputAccel = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) ? 1.0 : 0.0;
	//m_inputBrake = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) ? 1.0 : 0.0;
	//m_inputLeft  = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) ? 1.0 : 0.0;
	//m_inputRight = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) ? 1.0 : 0.0;
}