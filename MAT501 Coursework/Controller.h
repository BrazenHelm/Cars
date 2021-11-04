#pragma once
#include "Evolver.h"
#include "Simulator.h"

class Controller
{
private:
	sf::RenderWindow m_window;
	Evolver* m_pEvolver;
	Simulator* m_pSimulator;

public:
	Controller();

	void Start(int popSize, int nInputs, int nOutputs);

	void Reset();

	void DoGeneration(bool fastMode);

	void DoTestedGeneration();

	void ShowOffBest(int nToShow, double simTime);

	void WriteFitnessesToFile(const std::string& filename);

	void SetTrack(const std::string& name) { m_pSimulator->SetTrack(name); }

private:
	sf::Font montserrat;
	sf::Text m_generationText;
	sf::Text m_fitnessText;

	std::vector<std::vector<double>> m_fitnessScores;
	std::vector<std::vector<double>> m_testFitnessScores;

	int m_popSize;
	int m_nInputs;
	int m_nOutputs;

	int m_generation;
};

