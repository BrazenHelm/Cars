#include "Genome.h"
#include "NeuralNetwork.h"
#include "Random.h"


Genome::Genome() : 
	m_pPhenotype(nullptr), m_genomeID(0), m_nInputs(0), m_nOutputs(0),
	m_rawFitness(0.0), m_adjFitness(0.0), m_nToSpawn(0.0)
{}


Genome::Genome(int id, int nInputs, int nOutputs) :
	m_pPhenotype(nullptr), m_genomeID(id), m_nInputs(nInputs), m_nOutputs(nOutputs),
	m_rawFitness(0.0), m_adjFitness(0.0), m_nToSpawn(0.0)
{
	// Create input neurons
	for (int i = 0; i < nInputs; i++)
	{
		m_neuronGenes.emplace_back(i, NeuronType::INPUT, 0.0, false);
	}

	// Create bias neuron
	m_neuronGenes.emplace_back(nInputs, NeuronType::BIAS, 0.0, false);

	// Create output neurons
	for (int j = 0; j < nOutputs; j++)
	{
		m_neuronGenes.emplace_back(nInputs + 1 + j, NeuronType::OUTPUT, 1.0, false);
	}

	// Fully connect network wirth random link weights -1 < w < 1
	for (int i = 0; i < nInputs + 1; i++)
	{
		for (int j = 0; j < nOutputs; j++)
		{
			m_linkGenes.emplace_back(
				m_neuronGenes[i].id, m_neuronGenes[nInputs + 1 + j].id,
				true, nInputs + nOutputs + 1 + NLinks(), Random::Double(-1.0, 1.0)
			);
		}
	}
}


Genome::Genome(int id, int nInputs, int nOutputs,
	std::vector<NeuronGene> neurons, std::vector<LinkGene> links) :
	m_pPhenotype(nullptr), m_genomeID(id), m_nInputs(nInputs), m_nOutputs(nOutputs),
	m_rawFitness(0.0), m_adjFitness(0.0), m_nToSpawn(0.0),
	m_neuronGenes(neurons), m_linkGenes(links)
{}


Genome::~Genome()
{
	DeletePhenotype();
}


Genome::Genome(const Genome& src)
{
	m_genomeID = src.m_genomeID;
	m_neuronGenes = src.m_neuronGenes;
	m_linkGenes = src.m_linkGenes;
	m_pPhenotype = nullptr;			// no need for deep copy
	m_rawFitness = src.m_rawFitness;
	m_adjFitness = src.m_adjFitness;
	m_nInputs = src.m_nInputs;
	m_nOutputs = src.m_nOutputs;
	m_nToSpawn = src.m_nToSpawn;
}


Genome& Genome::operator=(const Genome& src)
{
	if (this != &src)
	{
		m_genomeID = src.m_genomeID;
		m_neuronGenes = src.m_neuronGenes;
		m_linkGenes = src.m_linkGenes;
		m_pPhenotype = nullptr;			// no need for deep copy
		m_rawFitness = src.m_rawFitness;
		m_adjFitness = src.m_adjFitness;
		m_nInputs = src.m_nInputs;
		m_nOutputs = src.m_nOutputs;
		m_nToSpawn = src.m_nToSpawn;
	}
	return *this;
}


NeuralNetwork* Genome::CreatePhenotype()
{
	DeletePhenotype();

	// Create all neurons
	std::vector<Neuron*> pNeurons;
	for (size_t i = 0; i < m_neuronGenes.size(); i++)
	{
		Neuron* pNeuron = new Neuron(
			m_neuronGenes[i].id,
			m_neuronGenes[i].type,
			m_neuronGenes[i].depth,
			m_neuronGenes[i].response
		);
		pNeurons.push_back(pNeuron);
	}

	// Create links
	for (size_t j = 0; j < m_linkGenes.size(); j++)
	{
		if (m_linkGenes[j].isEnabled)
		{
			Neuron* pFrom = pNeurons[GetElementPos(m_linkGenes[j].fromNeuron)];
			Neuron* pTo = pNeurons[GetElementPos(m_linkGenes[j].toNeuron)];

			pTo->AddLinkIn(m_linkGenes[j].weight, pFrom, m_linkGenes[j].isRecurrent);
		}
	}

	// Now the neurons contain all the information needed
	m_pPhenotype = new NeuralNetwork(pNeurons, m_nInputs, m_nOutputs);
	return m_pPhenotype;
}


void Genome::DeletePhenotype()
{
	if (m_pPhenotype != nullptr)
	{
		delete m_pPhenotype;
		m_pPhenotype = nullptr;
	}
}


int Genome::GetElementPos(int neuronID)
{
	for (size_t i = 0; i < m_neuronGenes.size(); i++)
	{
		if (m_neuronGenes[i].id == neuronID) return i;
	}
	return -1;
}


void Genome::SortGenes()
{
	std::sort(m_neuronGenes.begin(), m_neuronGenes.end());
	std::sort(m_linkGenes.begin(), m_linkGenes.end());
}


bool Genome::DuplicateNeuronID(int neuronID)
{
	for (NeuronGene gene : m_neuronGenes)
	{
		if (neuronID == gene.id)
		{
			return true;
		}
	}
	return false;
}


bool Genome::DuplicateLink(int fromNeuron, int toNeuron)
{
	for (LinkGene gene : m_linkGenes)
	{
		if (gene.fromNeuron == fromNeuron && gene.toNeuron == toNeuron)
		{
			return true;
		}
	}
	return false;
}


// Choose a link to split, adding a new hidden neuron in the middle of it
void Genome::AddNeuron(InnovationIndex& innov, int nTries)
{
	bool done = false;
	int chosen = 0;

	// First, choose a link to split
	while (!done)
	{
		chosen = Random::Int(0, NLinks() - 1);
		int fromNeuron = m_linkGenes[chosen].fromNeuron;

		// ensure the link is enabled and does not have bias neuron as its input
		if (m_linkGenes[chosen].isEnabled &&
			m_neuronGenes[GetElementPos(fromNeuron)].type != NeuronType::BIAS)
		{
			done = true;
		}
	}

	// disable the link
	m_linkGenes[chosen].isEnabled = false;

	// Grab the weight from the gene. We use this for the weight of one of the
	// new links so that the split doesn't disrupt what the network has already
	// learned
	double originalWeight = m_linkGenes[chosen].weight;

	// ids of the neurons at either end of the chosen link
	int from = m_linkGenes[chosen].fromNeuron;
	int to = m_linkGenes[chosen].toNeuron;

	double newDepth = (m_neuronGenes[GetElementPos(from)].depth +
						m_neuronGenes[GetElementPos(to)].depth) / 2.0;

	int id = innov.CheckInnovation(from, to, InnovationType::NEURON);

	// We need to check if a neuron ID is already being used. If it is, we create
	// a new innovation for the neuron. This prevents links from being repeatedly
	// disabled and re-enabled
	if (id >= 0)
	{
		int neuronID = innov.GetNeuronID(id);
		if (DuplicateNeuronID(neuronID)) id = -1;
	}

	if (id < 0)
	{
		// Add innovation for the new neuron
		int newNeuronID = innov.CreateNeuronInnovation(from, to, NeuronType::HIDDEN, newDepth);

		// Create the new neuron gene and add it
		m_neuronGenes.push_back(NeuronGene(newNeuronID, NeuronType::HIDDEN, newDepth));

		// Now create two links, one for each side of the split link
		int linkID1 = innov.CreateLinkInnovation(from, newNeuronID);
		LinkGene link1(from, newNeuronID, true, linkID1, 1.0);
		m_linkGenes.push_back(link1);

		int linkID2 = innov.CreateLinkInnovation(newNeuronID, to);
		LinkGene link2(newNeuronID, to, true, linkID2, originalWeight);
		m_linkGenes.push_back(link2);
	}
	else
	{
		// In this case, the innovation has already occurred so grab the relevant
		// information from the innovation index
		int newNeuronID = innov.GetNeuronID(id);

		int linkID1 = innov.CheckInnovation(from, newNeuronID, InnovationType::LINK);
		int linkID2 = innov.CheckInnovation(newNeuronID, to, InnovationType::LINK);

		if (linkID1 < 0 || linkID2 < 0)
		{
			printf("Genome::AddNeuron - big problemo\n");
			return;
		}

		// create new link genes as above
		LinkGene link1(from, newNeuronID, true, linkID1, 1.0);
		m_linkGenes.push_back(link1);

		LinkGene link2(newNeuronID, to, true, linkID2, originalWeight);
		m_linkGenes.push_back(link2);

		// create new neuron
		NeuronGene newNeuron(newNeuronID, NeuronType::HIDDEN, newDepth);
		m_neuronGenes.push_back(newNeuron);
	}
}


// avoids adding recurrent links
void Genome::AddLink(InnovationIndex& innov, int nTries)
{
	// The two neurons to be linked. If we find valid neurons to link, these
	// values will be updated to their IDs
	int neuronID1 = -1, neuronID2 = -1;

	while (nTries)
	{
		neuronID1 = m_neuronGenes[Random::Int(0, NNeurons() - 1)].id;
		neuronID2 = m_neuronGenes[Random::Int(m_nInputs + 1, NNeurons() - 1)].id;

		// make sure the neurons are not the same, are not already linked,
		// and the first comes before the second in the network
		if (neuronID1 == neuronID2 || DuplicateLink(neuronID1, neuronID2) ||
			m_neuronGenes[GetElementPos(neuronID1)].depth >
			m_neuronGenes[GetElementPos(neuronID2)].depth)
		{
			neuronID1 = -1;
			neuronID2 = -1;
		}
		else
		{
			break;
		}
	}

	if (neuronID1 < 0 || neuronID2 < 0)
	{
		// unsuccessful in finding suitable neurons, so give up
		return;
	}

	int id = innov.CheckInnovation(neuronID1, neuronID2, InnovationType::LINK);

	if (id < 0)
	{
		// innovation not already occurred
		int innovID = innov.CreateLinkInnovation(neuronID1, neuronID2);
		LinkGene link(
			neuronID1, neuronID2, true, innovID, Random::Double(-1.0, 1.0)
		);
		m_linkGenes.push_back(link);
	}
	else
	{
		// innovation alraedy occurred
		LinkGene link(
			neuronID1, neuronID2, true, id, Random::Double(-1.0, 1.0)
		);
		m_linkGenes.push_back(link);
	}
}


// allows recurrent links
void Genome::AddLink(InnovationIndex& innov, double chanceOfLooped,
	int nTriesLoop, int nTriesLink)
{
	// The two neurons to be linked. If we find valid neurons to link, these
	// values will be updated to their IDs
	int neuronID1 = -1, neuronID2 = -1;

	// flag set if a recurrent link is selected
	bool recurrent = false;

	if (Random::Double(0, 1) < chanceOfLooped)
	{
		while (nTriesLoop)
		{
			// grab any non-input neuron
			int neuronPos = Random::Int(m_nInputs + 1, NNeurons() - 1);
			
			// check to make sure the neuron is not already recurrent
			if (!m_neuronGenes[neuronPos].isRecurrent)
			{
				neuronID1 = neuronID2 = m_neuronGenes[neuronPos].id;
				m_neuronGenes[neuronPos].isRecurrent = true;
				recurrent = true;
				break;
			}
		}
	}

	else
	{
		while (nTriesLink)
		{
			neuronID1 = m_neuronGenes[Random::Int(0, NNeurons() - 1)].id;
			neuronID2 = m_neuronGenes[Random::Int(m_nInputs + 1, NNeurons() - 1)].id;

			// make sure the neurons are not the same and are not already linked
			if (neuronID1 == neuronID2 || DuplicateLink(neuronID1, neuronID2))
			{
				neuronID1 = -1;
				neuronID2 = -1;
			}
			else
			{
				break;
			}
		}
	}

	if (neuronID1 < 0 || neuronID2 < 0)
	{
		// unsuccessful in finding suitable neurons, so give up
		return;
	}

	int id = innov.CheckInnovation(neuronID1, neuronID2, InnovationType::LINK);

	if (m_neuronGenes[GetElementPos(neuronID1)].depth >
		m_neuronGenes[GetElementPos(neuronID2)].depth)
	{
		recurrent = false;
	}

	if (id < 0)
	{
		// innovation not already occurred
		int id = innov.CreateLinkInnovation(neuronID1, neuronID2);
		LinkGene link(
			neuronID1, neuronID2, true, id, Random::Double(-1.0, 1.0), recurrent
		);
		m_linkGenes.push_back(link);
	}
	else
	{
		// innovation alraedy occurred
		LinkGene link(
			neuronID1, neuronID2, true, id, Random::Double(-1.0, 1.0), recurrent
		);
		m_linkGenes.push_back(link);
	}
}


void Genome::MutateWeights(double mutationRate, double chanceOfTotalMutation,
	double maxPerturbation)
{
	for (size_t i = 0; i < m_linkGenes.size(); i++)
	{
		// Do we mutate the weight on this link at all
		if (Random::Double(0, 1) < mutationRate)
		{
			// Do we change the weight to something completely new
			if (Random::Double(0, 1) < chanceOfTotalMutation)
			{
				m_linkGenes[i].weight = Random::Double(-1, 1);
			}
			// Or do we just perturb it
			else
			{
				m_linkGenes[i].weight += Random::Double(-1, 1) * maxPerturbation;
			}
		}
	}
}


double Genome::GetCompatibilityScore(const Genome& other)
{
	// track the number of each type of gene to find difference
	double nDisjoint = 0.0;
	double nExcess = 0.0;
	double nMatching = 0.0;
	double weightDiff = 0.0;

	// point reached in each genome
	int i = 0, j = 0;

	while (i < m_linkGenes.size() - 1 || j < other.m_linkGenes.size() - 1)
	{
		// end of this genome reached but not of other; increment excess genes
		if (i == m_linkGenes.size() - 1)
		{
			j++;
			nExcess++;
			continue;
		}
		// vice versa
		if (j == other.m_linkGenes.size() - 1)
		{
			i++;
			nExcess++;
			continue;
		}

		// innovation numbers for each gene at the current point
		int id1 = m_linkGenes[i].innovationID;
		int id2 = other.m_linkGenes[j].innovationID;

		// innovation numbers match; increment matching genes
		if (id1 == id2)
		{
			i++;
			j++;
			nMatching++;
			weightDiff += abs(m_linkGenes[i].weight - other.m_linkGenes[j].weight);
			continue;
		}
		// innovation numbers do not match; increment disjoint genes
		if (id1 < id2)
		{
			i++;
			nDisjoint++;
			continue;
		}
		if (id2 < id1)
		{
			j++;
			nDisjoint++;
			continue;
		}
	} // end while

	int longest = other.NLinks();
	if (NLinks() > longest) longest = NLinks();

	const double multiplierDisjoint = 1.0;
	const double multiplierExcess = 1.0;
	const double multiplierMatching = 0.4;

	return (multiplierDisjoint * nDisjoint / (double)longest)
		+ (multiplierExcess * nExcess / (double)longest)
		+ (multiplierMatching * weightDiff / nMatching);
}

