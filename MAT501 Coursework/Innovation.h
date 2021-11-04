#pragma once
#include "Genes.h"
#include "NeuronType.h"
#include <vector>


enum InnovationType { LINK, NEURON };


struct Innovation
{
	InnovationType type;
	int id;

	// only used for links
	int neuronIn, neuronOut;

	// only used for neurons
	int neuronID;
	NeuronType neuronType;
	double depth;

	Innovation(int id, int in, int out) :
		type(InnovationType::LINK), id(id), neuronIn(in), neuronOut(out),
		neuronID(-1), neuronType(NeuronType::INVALID), depth(0.0)
	{};

	Innovation(int id, int in, int out, int neuronID, NeuronType neuronType, double depth) :
		type(InnovationType::NEURON), id(id), neuronIn(in), neuronOut(out),
		neuronID(neuronID), neuronType(neuronType), depth(depth)
	{};

	Innovation(int id, NeuronGene neuron) :
		type(InnovationType::NEURON), id(id), neuronIn(-1), neuronOut(-1),
		neuronID(neuron.id), neuronType(neuron.type), depth(neuron.depth)
	{};

	Innovation(int id, LinkGene link) :
		type(InnovationType::LINK), id(id), neuronIn(link.fromNeuron), neuronOut(link.toNeuron),
		neuronID(-1), neuronType(NeuronType::INVALID), depth(0.0)
	{};
};


class InnovationIndex
{
private:
	std::vector<Innovation> m_innovations;
	int m_nextNeuronID;
	int m_nextInnovationID;

public:
	InnovationIndex(
		std::vector<NeuronGene> startNeurons, std::vector<LinkGene> startLinks);

	// Checks to see if the given innovation has already occurred. If so, returns
	// the innovation id. If not, returns -1.
	int CheckInnovation(int from, int to, InnovationType type);

	// Creates a new Neuron innovation and returns its ID
	int CreateNeuronInnovation(int from, int to, NeuronType type, double depth);

	// Creates a new Link innovation and returns its ID
	int CreateLinkInnovation(int from, int to);

	// Creates a basic (non-recurrent) neuron gene from the given ID
	NeuronGene CreateNeuronFromID(int id);

	// Accessor
	int GetNeuronID(int innovID) const { return m_innovations[innovID].neuronID; }
};

