#pragma once
#include "Genes.h"
#include "Innovation.h"
#include <vector>

class NeuralNetwork;

class Genome
{
private:
	int m_genomeID;

	std::vector<NeuronGene>		m_neuronGenes;
	std::vector<LinkGene>		m_linkGenes;

	NeuralNetwork* m_pPhenotype;

	int m_networkDepth;

	double m_rawFitness, m_adjFitness;

	double m_nToSpawn;

	int m_nInputs, m_nOutputs;

	int m_speciesID;

public:
	Genome();

	// creates a minimal fully-connected genome
	Genome(int id, int nInputs, int nOutputs);

	// creates a genome from an already populated list of genes
	Genome(int id, int nInputs, int nOutputs,
		std::vector<NeuronGene> neurons, std::vector<LinkGene> links);

	~Genome();

	Genome(const Genome& src);
	Genome& operator=(const Genome& src);

	NeuralNetwork* CreatePhenotype();
	void DeletePhenotype();

	// Used for mutations
	void AddNeuron(InnovationIndex& innov, int nTries);
	void AddLink(InnovationIndex& innov, int nTries);
	void AddLink(InnovationIndex& innov, double chanceOfLooped,
		int nTriesLoop, int nTriesLink);
	void MutateWeights(double mutationRate, double chanceOfTotalMutation,
		double maxPerturbation);

	// Used for speciation
	double GetCompatibilityScore(const Genome& other);
	
	void SortGenes();

	// Use fitness as criterion for sorting (fittest to least fit)
	friend bool operator<(const Genome& lhs, const Genome& rhs)
	{
		return (lhs.m_rawFitness > rhs.m_rawFitness);
	}

private:
	bool DuplicateNeuronID(int neuronID);
	bool DuplicateLink(int fromNeuron, int toNeuron);
	int GetElementPos(int neuron);

public:
	// Accessors

	int ID() const { return m_genomeID; }
	void SetID(const int val) { m_genomeID = val; }

	int Depth() const { return m_networkDepth; }
	void SetDepth(const int val) { m_networkDepth = val; }

	size_t NNeurons() const { return m_neuronGenes.size(); }
	size_t NLinks() const { return m_linkGenes.size(); }
	int NInputs() const { return m_nInputs; }
	int NOutputs() const { return m_nOutputs; }

	double NToSpawn() const { return m_nToSpawn; }
	void SetNToSpawn(const double val) { m_nToSpawn = val; }

	double Fitness() const { return m_rawFitness; }
	void SetFitness(const double val) { m_rawFitness = val; }
	double AdjFitness() const { return m_adjFitness; }
	void SetAdjFitness(const double val) { m_adjFitness = val; }

	int Species() const { return m_speciesID; }
	void SetSpecies(const int val) { m_speciesID = val; }

	NeuralNetwork* Phenotype() { return m_pPhenotype; }

	std::vector<NeuronGene> NeuronGenes() const { return m_neuronGenes; }
	std::vector<LinkGene> LinkGenes() const { return m_linkGenes; }

	std::vector<LinkGene>::iterator StartOfLinkGenes() { return m_linkGenes.begin(); }
	std::vector<LinkGene>::iterator EndOfLinkGenes() { return m_linkGenes.end(); }
};

