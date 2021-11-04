#pragma once

#include "Car.h"
#include "Racetrack.h"
#include <SFML/Graphics.hpp>

class Simulator
{
public:
	Simulator(int size, int inputs, int outputs, sf::RenderWindow* pWindow);
	~Simulator();

	void DoFrame(bool doRendering, int nToShow = 0);

	std::vector<double> GetFitnessScores();
	void UpdateBrains(std::vector<NeuralNetwork*> pBrains);

	//void UpdateBestBrains(int nToShow);
	void PrepareBestBrains(int nToShow);
	void ReplaceStandardBrains(int nToShow);

	void SetTrack(const std::string& name) { m_racetrack.SetTrack(name); }

	void CalculateFitnessScores();
	std::vector<double> TestFitnessScores();

	void NewSimulation();

private:
	std::vector<NeuralNetwork*> m_pBrains;
	std::vector<Car> m_cars;
	Racetrack m_racetrack;

	sf::RenderWindow* m_pWindow;

	int m_popSize;

	std::vector<int> m_bestBrains;	// indices in m_pBrains of the fittest brains
	std::vector<double> m_bestFitnesses;
};

