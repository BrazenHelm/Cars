#pragma once
#include "NeuronType.h"

class Neuron;


struct NeuronGene
{
	int id;
	NeuronType type;
	double response;
	bool isRecurrent;
	double depth;

	NeuronGene(int id, NeuronType type, double depth,
		bool recurrent = false, double response = 1.0) :
		id(id), type(type), depth(depth),
		isRecurrent(recurrent), response(response)
	{};

	// use depth as criterion for sorting neurons
	friend bool operator<(const NeuronGene& lhs, const NeuronGene& rhs)
	{
		return (lhs.depth < rhs.depth);
	}
};


struct LinkGene
{
	int fromNeuron, toNeuron;
	double weight;
	bool isEnabled;
	int innovationID;
	bool isRecurrent;


	LinkGene() :
		fromNeuron(-1), toNeuron(-1), isEnabled(false),
		innovationID(-1), weight(0.0), isRecurrent(false)
	{};

	LinkGene(int fromID, int toID, bool enabled,
		int tag, double weight, bool recurrent = false) :
		fromNeuron(fromID), toNeuron(toID), isEnabled(enabled),
		innovationID(tag), weight(weight), isRecurrent(recurrent)
	{};

	// use innovation ID as criterion for sorting links
	friend bool operator<(const LinkGene& lhs, const LinkGene& rhs)
	{
		return (lhs.innovationID < rhs.innovationID);
	}
};

