#include "Evolver.h"

#include "Genome.h"
#include "NeuralNetwork.h"
#include "Params.h"
#include "Random.h"
#include "Species.h"



Evolver::Evolver(int size, int inputs, int outputs) :
	m_popSize(size), m_generation(0), m_pInnovation(nullptr), m_nInputs(inputs), m_nOutputs(outputs),
	m_nextGenomeID(0), m_nextSpeciesID(0), m_fittestGenomeIndex(0),
	m_bestEverFitness(0.0), m_totalAdjFit(0.0), m_avgAdjFit(0.0)
{
	// Create the innovation list based on a minimal genome
	Genome genome(1, inputs, outputs);
	m_pInnovation = new InnovationIndex(genome.NeuronGenes(), genome.LinkGenes());
}


Evolver::~Evolver()
{
	if (m_pInnovation != nullptr)
	{
		delete m_pInnovation;
		m_pInnovation = nullptr;
	}
}


void Evolver::CreateInitialPopulation()
{
	for (size_t i = 0; i < m_popSize; i++)
	{
		m_genomes.emplace_back(m_nextGenomeID++, m_nInputs, m_nOutputs);
	}
}

//std::vector<NeuralNetwork*> Evolver::Epoch(const std::vector<double>& fitnesses)
//{
//	for (size_t i = 0; i < m_popSize; i++)
//	{
//		m_genomes[i].SetFitness(fitnesses[i]);
//	}
//	SortAndRecord();
//
//	double totalFitness = 0.0;
//	const int N_KEPT = 40;
//
//	for (size_t i = 0; i < N_KEPT; i++)
//	{
//		totalFitness += m_genomes[i].Fitness();
//	}
//
//	for (size_t i = N_KEPT; i < m_popSize; i++)
//	{
//		double motherRand = Random::Double(0, totalFitness);
//		double fatherRand = Random::Double(0, totalFitness);
//
//		int j = 0, k = 0;
//
//		while (motherRand > 0)
//		{
//			motherRand -= m_genomes[j++].Fitness();
//		}
//		while (fatherRand > 0)
//		{
//			fatherRand -= m_genomes[k++].Fitness();
//		}
//
//		//int j = Random::Int(0, N_KEPT - 1);
//		//int k = Random::Int(0, N_KEPT - 1);
//		Genome baby = Crossover(m_genomes[j - 1], m_genomes[k - 1]);
//
//		baby.SetID(++m_nextGenomeID);
//
//		//const double CHANCE_TO_ADD_NEURON = 0.08;
//		//const double CHANCE_TO_ADD_LINK = 0.14;
//		//const double CHANCE_TO_MUTATE_WEIGHT = 0.2;
//		//const double CHANCE_OF_TOTAL_WEIGHT_MUTATION = 0.1;
//		//const double MAX_WEIGHT_PERTURBATION = 0.5;
//		//const int N_TRIES_TO_MAKE_NEW_NEURON = 10;
//		//const int N_TRIES_TO_MAKE_NEW_LINK = 10;
//
//		if (Random::Double(0, 1) < Params::CHANCE_TO_ADD_NEURON)
//		{
//			baby.AddNeuron(*m_pInnovation, Params::N_TRIES_TO_MAKE_NEW_NEURON);
//		}
//		if (Random::Double(0, 1) < Params::CHANCE_TO_ADD_LINK)
//		{
//			baby.AddLink(*m_pInnovation, Params::N_TRIES_TO_MAKE_NEW_LINK);
//		}
//		baby.MutateWeights(
//			Params::CHANCE_TO_MUTATE_WEIGHT,
//			Params::CHANCE_OF_TOTAL_WEIGHT_MUTATION,
//			Params::MAX_WEIGHT_PERTURBATION
//		);
//
//		baby.SortGenes();
//
//		m_genomes[i] = baby;
//	}
//	m_generation++;
//	return CreatePhenotypes();
//}


std::vector<NeuralNetwork*> Evolver::Epoch(const std::vector<double>& fitnessScores)
{
	if (fitnessScores.size() != m_popSize)
	{
		printf("Evolver::Epoch : wrong number of fitness scores provided\n");
	}

	StartEpoch();

	for (size_t gen = 0; gen < m_genomes.size(); gen++)
	{
		m_genomes[gen].SetFitness(fitnessScores[gen]);
	}

	SortAndRecord();
	SpeciateAndCalculateSpawnLevels();

	// Breeding

	std::vector<Genome> newPop;
	int nSpawnedAlready = 0;
	Genome baby;

	for (size_t spc = 0; spc < m_species.size(); spc++)
	{
		if (nSpawnedAlready < m_popSize)
		{
			// Number of offspring this species is required to spawn
			int nToSpawn = static_cast<int>(round(m_species[spc].NToSpawn()));
			bool chosenBestYet = false;

			while (nToSpawn-- != 0)
			{
				// We first take the best performing genome from each species
				// and transfer to the new population without mutation
				if (!chosenBestYet)
				{
					baby = m_species[spc].Leader();
					chosenBestYet = true;
				}
				else
				{
					// If there is only one member we can only do mutation
					if (m_species[spc].Population() == 1)
					{
						baby = m_species[spc].Spawn();
					}
					// Otherwise we breed a baby using Evolver::Crossover
					else
					{
						Genome mother = m_species[spc].Spawn();
						if (Random::Double(0, 1) < Params::CROSSOVER_RATE)
						{
							Genome father = m_species[spc].Spawn();
							int nTries = 5;
							while ((mother.ID() == father.ID()) && (nTries--))
							{
								father = m_species[spc].Spawn();
							}
							if (mother.ID() != father.ID())
							{
								baby = Crossover(mother, father);
							}
							else
							{
								baby = mother;
							}
						}
						else
						{
							baby = mother;
						}
					}

					m_nextGenomeID++;
					baby.SetID(m_nextGenomeID);

					// mutation
					if (Random::Double(0, 1) < Params::CHANCE_TO_ADD_NEURON)
					{
						baby.AddNeuron(*m_pInnovation, Params::N_TRIES_TO_MAKE_NEW_NEURON);
					}
					if (Random::Double(0, 1) < Params::CHANCE_TO_ADD_LINK)
					{
						baby.AddLink(*m_pInnovation, Params::N_TRIES_TO_MAKE_NEW_LINK);
					}
					baby.MutateWeights(
						Params::CHANCE_TO_MUTATE_WEIGHT,
						Params::CHANCE_OF_TOTAL_WEIGHT_MUTATION,
						Params::MAX_WEIGHT_PERTURBATION
					);
				}

				baby.SortGenes();
				newPop.push_back(baby);
				nSpawnedAlready++;
			
				if (nSpawnedAlready == m_popSize)
				{
					nToSpawn == 0;
				}
			}
		}
	} // next species

	// fill any gaps by staging tournaments
	if (nSpawnedAlready < m_popSize)
	{
		int required = m_popSize - nSpawnedAlready;
		while (required--)
		{
			newPop.push_back(TournamentSelect(m_popSize / 5));
		}
	}

	m_genomes = newPop;
	m_generation++;
	return CreatePhenotypes();
}


void Evolver::StartEpoch()
{
	m_totalAdjFit = 0.0;
	m_avgAdjFit = 0.0;

	auto it = m_species.begin();

	while (it != m_species.end())
	{
		it->Purge();

		if ((it->NGensNoImprovement() > Params::N_GENS_ALLOWED_NO_IMPROVEMENT) &&
			(it->BestFitness() < m_bestEverFitness))
		{
			it = m_species.erase(it);
			it--;
		}

		it++;
	}
}


Genome Evolver::TournamentSelect(const int nRounds)
{
	double bestFitnessSoFar = 0;
	int winner = 0;

	for (int i = 0; i < nRounds; i++)
	{
		int contestant = Random::Int(0, m_genomes.size() - 1);
		if (m_genomes[contestant].Fitness() > bestFitnessSoFar)
		{
			winner = contestant;
			bestFitnessSoFar = m_genomes[contestant].Fitness();
		}
	}
	return m_genomes[winner];
}


std::vector<NeuralNetwork*> Evolver::CreatePhenotypes()
{
	std::vector<NeuralNetwork*> pNetworks;

	for (size_t i = 0; i < m_popSize; i++)
	{
		//CalculateNetworkDepth(m_genomes[i]);
		NeuralNetwork* pNetwork = m_genomes[i].CreatePhenotype();
		pNetworks.push_back(pNetwork);
	}

	return pNetworks;
}


void Evolver::SortAndRecord()
{
	std::sort(m_genomes.begin(), m_genomes.end());
	if (m_genomes[0].Fitness() > m_bestEverFitness)
	{
		m_bestEverFitness = m_genomes[0].Fitness();
	}

	//StoreBestGenomes();
}


void Evolver::SpeciateAndCalculateSpawnLevels()
{
	for (Genome& genome : m_genomes)
	{
		bool foundCompatible = false;
		for (Species& species : m_species)
		{
			double compatibility = genome.GetCompatibilityScore(species.Leader());
			if (compatibility < Params::COMPATIBILITY_THRESHOLD)
			{
				species.AddMember(genome);
				genome.SetSpecies(species.ID());
				foundCompatible = true;
				break;
			}
		}
		if (!foundCompatible)
		{
			// no compatible species found; create a new one
			m_species.push_back(Species(genome, m_nextSpeciesID++));
		}
	}

	for (Species& species : m_species)
	{
		species.AdjustFitnesses();
	}

	for (Genome& genome : m_genomes)
	{
		m_totalAdjFit += genome.AdjFitness();
	}
	m_avgAdjFit = m_totalAdjFit / (double)m_genomes.size();

	for (Genome& genome : m_genomes)
	{
		double nToSpawn = genome.AdjFitness() / m_avgAdjFit;
		genome.SetNToSpawn(nToSpawn);
	}

	for (Species& species : m_species)
	{
		species.CalculateNumToSpawn();
	}
}


//void Evolver::CalculateNetworkDepth(Genome& genome)
//{
//	int maxSoFar = 0;
//
//	for (size_t i = 0; i < genome.NumNeurons(); i++)
//	{
//		for (size_t j = 0; j < m_splits.size(), j++)
//		{
//			if (genome.SplitY)
//		}
//	}
//}


Genome Evolver::Crossover(Genome& mother, Genome& father)
{
	enum ParentType { MOTHER, FATHER };

	// Pick the better of the two genomes. If the genomes differ in structure
	// we use the better one's characteristics
	ParentType best;

	if (mother.Fitness() == father.Fitness())
	{
		// if equal fitness, use the simpler genome since we want to keep
		// networks small if possible
		if (mother.NLinks() == father.NLinks())
		{
			// if equal in fitness and complexity, choose one at random
			best = ParentType(Random::Int(0, 1));
		}
		else if (mother.NLinks() < father.NLinks()) best = MOTHER;
		else best = FATHER;
	}
	else if (mother.Fitness() > father.Fitness()) best = MOTHER;
	else best = FATHER;

	// These vectors store the baby's genes
	std::vector<NeuronGene> babyNeurons;
	std::vector<LinkGene>	babyLinks;

	// temporary
	std::vector<int> neuronIDs;

	auto itMother = mother.StartOfLinkGenes();
	auto itFather = father.StartOfLinkGenes();

	LinkGene selectedGene;

	// Step through each parent's genes until we reach the end of both. The
	// order is determined by innovation number (least to most)
	while (itMother != mother.EndOfLinkGenes()
		|| itFather != father.EndOfLinkGenes())
	{
		// We now check various conditions
		// If any of these are true, we've found something incomparable between
		// the two genomes. We can't cross these, so we instead take those from
		// the better parent and ignore those from the worse

		// if done with mother but not with father
		if (itMother == mother.EndOfLinkGenes()
			&& itFather != father.EndOfLinkGenes())
		{
			if (best == FATHER)
				selectedGene = *itFather;
			itFather++;
		}
		// if done with father but not with mother
		else if (itFather == father.EndOfLinkGenes()
			&& itMother != mother.EndOfLinkGenes())
		{
			if (best == MOTHER)
				selectedGene = *itMother;
			itMother++;
		}
		// if mother's innovation ID is less than father's
		else if (itMother->innovationID < itFather->innovationID)
		{
			if (best == MOTHER)
				selectedGene = *itMother;
			itMother++;
		}
		// if father's innovation ID is less than mother's
		else if (itFather->innovationID < itMother->innovationID)
		{
			if (best == FATHER)
				selectedGene = *itFather;
			itFather++;
		}
		// if we got here, the current genes are comparable so pick one at random
		else
		{
			if (Random::Int(0, 1) == 0)
				selectedGene = *itMother;
			else
				selectedGene = *itFather;

			itMother++;
			itFather++;
		}

		// add the selected gene if not already added
		if (babyLinks.size() == 0 ||
			babyLinks[babyLinks.size() - 1].innovationID != selectedGene.innovationID)
		{
			babyLinks.push_back(selectedGene);
		}

		// check if we already have the neurons referred to in selectedGene
		// if not, we need to add them
		AddNeuronID(selectedGene.fromNeuron, neuronIDs);
		AddNeuronID(selectedGene.toNeuron, neuronIDs);
	}

	// Now create the required nodes (in order of id)
	std::sort(neuronIDs.begin(), neuronIDs.end());
	for (size_t i = 0; i < neuronIDs.size(); i++)
	{
		babyNeurons.push_back(m_pInnovation->CreateNeuronFromID(neuronIDs[i]));
	}

	// Create the baby's genome from the neurons and links
	Genome baby(m_nextGenomeID++, mother.NInputs(), mother.NOutputs(),
		babyNeurons, babyLinks);

	return baby;
}


void Evolver::AddNeuronID(const int neuronID, std::vector<int>& vector)
{
	for (size_t i = 0; i < vector.size(); i++)
	{
		if (vector[i] == neuronID) return;	// already added
	}
	vector.push_back(neuronID);
	return;
}