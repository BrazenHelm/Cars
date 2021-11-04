#pragma once
#include "Link.h"
#include "Neuron.h"
#include <SFML/Graphics.hpp>
#include <vector>



class NeuralNetwork
{
	friend class NetworkDrawer;

public:
	NeuralNetwork();
	NeuralNetwork(std::vector<Neuron*> pNeurons, size_t nInputs, size_t nOutputs);

	~NeuralNetwork();
	
	std::vector<double> Process(const std::vector<double>& inputs);

	void Draw(sf::RenderWindow& window);

	void AddNeuron(int id, NeuronType type, double depth, double response = 1.0);
	void AddLink(double weight, int idFrom, int idTo, bool recurrent);
	void AddLinks(const std::vector<double>& weights,
		const std::vector<int>& idsFrom, int idTo);

private:
	// * Vector of Neurons. Must be stored in order:
	// * Inputs, Bias, Hidden (by layer), Output
	std::vector<Neuron*> m_pNeurons;
	std::vector<int> m_neuronIDs;

	size_t m_nInputs, m_nOutputs;

	int GetIndex(int id);

};

