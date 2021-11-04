#include "Controller.h"
#include "Params.h"
#include "Random.h"
#include <fstream>
#include <iomanip>


Controller::Controller() : 
	m_window(sf::VideoMode(800, 600), "MAT501")
{
	m_window.setFramerateLimit(std::min(60u, (unsigned int)Params::TICK_RATE));
	sf::Mouse mouse;

	montserrat.loadFromFile("Assets/Montserrat-Regular.ttf");
	m_generationText.setFont(montserrat);
	m_generationText.setPosition(5, 5);
	m_generationText.setCharacterSize(28);
	m_generationText.setFillColor(sf::Color::Red);
	m_generationText.setString("Generation 0");
	m_fitnessText.setFont(montserrat);
	m_fitnessText.setPosition(5, 40);
	m_fitnessText.setCharacterSize(20);
	m_fitnessText.setFillColor(sf::Color::Cyan);
	m_fitnessText.setString("Best Fitness : -");
}


void Controller::Start(int popSize, int nInputs, int nOutputs)
{
	m_popSize = popSize;
	m_nInputs = nInputs;
	m_nOutputs = nOutputs;

	m_pEvolver = new Evolver(popSize, nInputs, nOutputs);
	m_pSimulator = new Simulator(popSize, nInputs, nOutputs, &m_window);

	m_generation = 1;
}


void Controller::Reset()
{
	delete m_pEvolver;
	delete m_pSimulator;

	m_fitnessScores.clear();

	Start(m_popSize, m_nInputs, m_nOutputs);
}


void Controller::DoGeneration(bool fastMode)
{
	printf("Doing Generation %i\n", m_generation);

	double simTime = Params::SIM_TIME_BASE + (m_generation - 1) * Params::SIM_TIME_INCR;

	const size_t NUM_TICKS = static_cast<size_t>(Params::TICK_RATE * simTime);

	if (m_generation == 1)
	{
		// generate minimal genomes
		m_pEvolver->CreateInitialPopulation();
		std::vector<NeuralNetwork*> pBrains = m_pEvolver->CreatePhenotypes();
		m_pSimulator->UpdateBrains(pBrains);
	}
	else
	{
		// evolution
		auto fitnesses = m_pSimulator->GetFitnessScores();

		double maxFitness = fitnesses[0];
		for (double score : fitnesses) if (score > maxFitness) maxFitness = score;

		auto pBrains = m_pEvolver->Epoch(fitnesses);
		m_pSimulator->UpdateBrains(pBrains);

		m_generationText.setString("Generation " + std::to_string(m_generation));
		m_fitnessText.setString("Best Fitness : " + std::to_string(maxFitness));
	}

	// simulation
	if (fastMode)
	{
		for (size_t i = 0; i < NUM_TICKS; i++)
		{
			sf::Event event;
			while (m_window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed) m_window.close();
			}
			m_pSimulator->DoFrame(false);
		}
		m_pSimulator->CalculateFitnessScores();
	}
	else
	{
		for (size_t i = 0; i < NUM_TICKS; i++)
		{
			sf::Event event;
			while (m_window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed) m_window.close();
			}
			m_window.clear(sf::Color(0, 0, 0, 255));
			m_pSimulator->DoFrame(true);
			m_window.draw(m_generationText);
			m_window.draw(m_fitnessText);
			m_window.display();
		}
		m_pSimulator->CalculateFitnessScores();
	}
	m_generation++;

	m_fitnessScores.push_back(m_pSimulator->GetFitnessScores());
}


void Controller::DoTestedGeneration()
{
	printf("Doing Generation %i\n", m_generation);

	double simTime = Params::SIM_TIME_BASE + (m_generation - 1) * Params::SIM_TIME_INCR;

	const size_t NUM_TICKS = static_cast<size_t>(Params::TICK_RATE * simTime);

	if (m_generation == 1)
	{
		// generate minimal genomes
		m_pEvolver->CreateInitialPopulation();
		std::vector<NeuralNetwork*> pBrains = m_pEvolver->CreatePhenotypes();
		m_pSimulator->UpdateBrains(pBrains);
	}
	else
	{
		// evolution
		auto fitnesses = m_pSimulator->GetFitnessScores();

		double maxFitness = fitnesses[0];
		for (double score : fitnesses) if (score > maxFitness) maxFitness = score;

		auto pBrains = m_pEvolver->Epoch(fitnesses);
		m_pSimulator->UpdateBrains(pBrains);

		m_generationText.setString("Generation " + std::to_string(m_generation));
		m_fitnessText.setString("Best Fitness : " + std::to_string(maxFitness));
	}

	// simulation
	m_pSimulator->SetTrack("train");
	m_pSimulator->NewSimulation();
	for (size_t i = 0; i < NUM_TICKS; i++)
	{
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) m_window.close();
		}
		m_pSimulator->DoFrame(false);
	}
	m_pSimulator->CalculateFitnessScores();
	m_fitnessScores.push_back(m_pSimulator->GetFitnessScores());

	// testing
	m_pSimulator->SetTrack("test");
	m_pSimulator->NewSimulation();
	for (size_t i = 0; i < NUM_TICKS; i++)
	{
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) m_window.close();
		}
		m_pSimulator->DoFrame(false);
	}
	m_testFitnessScores.push_back(m_pSimulator->TestFitnessScores());

	m_generation++;
}


void Controller::ShowOffBest(int nToShow, double simTime)
{
	const size_t NUM_TICKS = static_cast<size_t>(Params::TICK_RATE * simTime);

	m_pSimulator->PrepareBestBrains(nToShow);

	for (size_t i = 0; i < NUM_TICKS; i++)
	{
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) m_window.close();
		}
		m_window.clear(sf::Color(0, 0, 0, 255));
		m_pSimulator->DoFrame(true, nToShow);
		m_window.draw(m_generationText);
		m_window.draw(m_fitnessText);
		m_window.display();
	}

	m_pSimulator->ReplaceStandardBrains(nToShow);
}


void Controller::WriteFitnessesToFile(const std::string& filename)
{
	std::ofstream outfile;
	outfile.open(filename.c_str());

	outfile << std::fixed << std::setprecision(5);

	for (size_t i = 0; i < Params::POP_SIZE; i++)
	{
		for (size_t j = 0; j < m_fitnessScores.size() - 1; j++)
		{
			outfile << m_fitnessScores[j][i] << ',';
		}
		outfile << m_fitnessScores[m_fitnessScores.size() - 1][i] << "\n";
	}

	if (m_testFitnessScores.size() == 0)
	{
		outfile.close();
		return;
	}

	outfile << '\n';

	for (size_t i = 0; i < Params::POP_SIZE; i++)
	{
		for (size_t j = 0; j < m_testFitnessScores.size() - 1; j++)
		{
			outfile << m_testFitnessScores[j][i] << ',';
		}
		outfile << m_testFitnessScores[m_testFitnessScores.size() - 1][i] << '\n';
	}

	outfile.close();
	return;
}

