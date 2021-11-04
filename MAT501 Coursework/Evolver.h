#pragma once

#include <vector>



class Genome;
class InnovationIndex;
class NeuralNetwork;
class Species;

class Evolver
{
private:
	std::vector<Genome>		m_genomes;
	std::vector<Genome>		m_bestGenomes;
	std::vector<Species>	m_species;
	InnovationIndex*		m_pInnovation;

	int m_generation;
	int m_nextGenomeID;
	int m_nextSpeciesID;
	size_t m_popSize;
	int m_nInputs, m_nOutputs;

	double m_totalAdjFit, m_avgAdjFit;

	int m_fittestGenomeIndex;
	double m_bestEverFitness;

public:
	Evolver(int size, int inputs, int outputs);
	~Evolver();

	// Performs one epoch of the genetic algorithm
	std::vector<NeuralNetwork*> Epoch(const std::vector<double>& fitnesses);

	// Creates phenotypes for the genotypes in the population
	std::vector<NeuralNetwork*> CreatePhenotypes();

	// Creates a population of minimal genomes
	void CreateInitialPopulation();

	// Getters
	//int NumSpecies() const { return m_species.size(); }
	double BestEverFitness() const { return m_bestEverFitness; }
	int Generation() const { return m_generation; }

private:
	void AddNeuronID(int nodeID, std::vector<int>& vec);

	// Prepares for a new epoch by resetting some values and killing all
	// phenotypes and any poorly performing species
	void StartEpoch();

	// Separates individuals into species by calculating a compatibility
	// score with every other member of the population. Determines how
	// many offspring each individual should spawn
	void SpeciateAndCalculateSpawnLevels();

	// Breeds two compatible genomes to produce a baby
	Genome Crossover(Genome& mother, Genome& father);

	// Holds a tournament to select a suitable offspring
	Genome TournamentSelect(const int nRounds);

	// Sorts the population into order of descending fitness, keeping
	// a record of the best genomes and updating statistics accordingly
	void SortAndRecord();
};

