#include "Simulator.h"
#include "NeuralNetwork.h"
#include "Params.h"
#include "Random.h"



Simulator::Simulator(int size, int inputs, int outputs, sf::RenderWindow* pWindow) :
	m_pWindow(pWindow),
	m_popSize(size)
{
	m_cars.reserve(size);
	for (int i = 0; i < size; i++)
	{
		m_cars.emplace_back(inputs, &m_racetrack);
	}
}


Simulator::~Simulator()
{
}


void Simulator::DoFrame(bool doRendering, int nToShow)
{	
	// Update
	for (Car& car : m_cars)
	{
		car.Update();
	}

	// Render
	if (doRendering)
	{
		if (nToShow == 0)	// render all cars
		{
			for (Car& car : m_cars)
			{
				car.Render(*m_pWindow);
			}
		}
		else
		{
			for (int i = 0; i < nToShow; i++)
			{
				m_cars[i].Render(*m_pWindow);
			}
		}
		m_racetrack.Draw(*m_pWindow);
	}
}


std::vector<double> Simulator::GetFitnessScores()
{
	std::vector<double> scores;
	scores.resize(m_popSize);

	for (int i = 0; i < m_popSize; i++)
	{
		scores[i] = m_cars[i].GetFitness();
	}

	return scores;
}



void Simulator::UpdateBrains(std::vector<NeuralNetwork*> pBrains)
{
	if (pBrains.size() != m_cars.size())
	{
		printf("Simulator::UpdateBrains : wrong number of brains provided\n");
		return;
	}

	m_pBrains = pBrains;

	for (size_t i = 0; i < pBrains.size(); i++)
	{
		m_cars[i].InsertNewBrain(m_pBrains[i]);
		m_cars[i].StartNewSimulation();
	}
}


void Simulator::PrepareBestBrains(int nToShow)
{
	std::vector<double> scores = GetFitnessScores();

	m_bestBrains.resize(nToShow);
	m_bestFitnesses.resize(nToShow);

	for (int i = 0; i < m_popSize; i++)
	{
		for (int j = 0; j < nToShow; j++)
		{
			if (scores[i] > m_bestFitnesses[j])
			{
				m_bestFitnesses.insert(m_bestFitnesses.begin() + j, scores[i]);
				m_bestBrains.insert(m_bestBrains.begin() + j, i);
				break;
			}
		}
	}

	m_bestBrains.resize(nToShow);
	m_bestFitnesses.resize(nToShow);

	for (int i = 0; i < nToShow; i++)
	{
		m_cars[i].InsertNewBrain(m_pBrains[m_bestBrains[i]]);
		m_cars[i].StartNewSimulation();
	}
}


void Simulator::ReplaceStandardBrains(int nToShow)
{
	for (int i = 0; i < nToShow; i++)
	{
		m_cars[i].InsertNewBrain(m_pBrains[i]);
		m_cars[i].StartNewSimulation();
	}
}


void Simulator::CalculateFitnessScores()
{
	for (Car& car : m_cars)
	{
		car.CalculateFitness();
	}
}


void Simulator::NewSimulation()
{
	for (Car& car : m_cars)
	{
		car.StartNewSimulation();
	}
}


std::vector<double> Simulator::TestFitnessScores()
{
	std::vector<double> scores;
	scores.resize(m_popSize);

	for (int i = 0; i < m_popSize; i++)
	{
		scores[i] = m_cars[i].TestFitness();
	}

	return scores;
}