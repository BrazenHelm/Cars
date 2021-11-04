#include "Innovation.h"


InnovationIndex::InnovationIndex(
	std::vector<NeuronGene> startNeurons, std::vector<LinkGene> startLinks) :
	m_nextInnovationID(0), m_nextNeuronID(0)
{
	for (size_t i = 0; i < startNeurons.size(); i++)
	{
		m_innovations.emplace_back(m_nextInnovationID, startNeurons[i]);
		m_nextInnovationID++;
		m_nextNeuronID++;
	}
	for (size_t j = 0; j < startLinks.size(); j++)
	{
		m_innovations.emplace_back(m_nextInnovationID, startLinks[j]);
		m_nextInnovationID++;
	}
}



int InnovationIndex::CheckInnovation(int from, int to, InnovationType type)
{
	for (const Innovation& innov : m_innovations)
	{
		if (innov.neuronIn == from && innov.neuronOut == to && innov.type == type)
		{
			return innov.id;
		}
	}
	return -1;
}


int InnovationIndex::CreateNeuronInnovation(int from, int to, NeuronType type, double depth)
{
	Innovation innov(m_nextInnovationID, from, to, m_nextNeuronID, type, depth);
	m_nextInnovationID++;
	m_nextNeuronID++;
	m_innovations.push_back(innov);

	return m_nextNeuronID - 1;
}


int InnovationIndex::CreateLinkInnovation(int from, int to)
{
	Innovation innov(m_nextInnovationID, from, to);
	m_nextInnovationID++;
	m_nextNeuronID++;
	m_innovations.push_back(innov);

	return m_nextInnovationID - 1;
}


NeuronGene InnovationIndex::CreateNeuronFromID(int id)
{
	NeuronGene result(0, NeuronType::HIDDEN, 0.0);

	for (size_t i = 0; i < m_innovations.size(); i++)
	{
		if (m_innovations[i].neuronID == id)
		{
			result.type = m_innovations[i].neuronType;
			result.id = m_innovations[i].neuronID;
			result.depth = m_innovations[i].depth;
			return result;
		}
	}

	printf("InnovationIndex::CreateNeuronFromID - id not found\n");
	return result;
}

