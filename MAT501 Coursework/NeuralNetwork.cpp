#include "NeuralNetwork.h"

#include "Link.h"



NeuralNetwork::NeuralNetwork() :
	m_pNeurons(), m_nInputs(0), m_nOutputs(0)
{};


NeuralNetwork::NeuralNetwork(std::vector<Neuron*> pNeurons, size_t nInputs, size_t nOutputs) :
	m_pNeurons(pNeurons), m_nInputs(nInputs), m_nOutputs(nOutputs)
{}


NeuralNetwork::~NeuralNetwork()
{
	for (size_t i = 0; i < m_pNeurons.size(); i++)
	{
		if (m_pNeurons[i] != nullptr)
		{
			delete m_pNeurons[i];
			m_pNeurons[i] = nullptr;
		}
	}
}


void NeuralNetwork::Draw(sf::RenderWindow& window)
{
	for (Neuron* pNeuron : m_pNeurons)
	{
		pNeuron->Draw(window);
	}
}


std::vector<double> NeuralNetwork::Process(const std::vector<double>& inputs)
{
	std::vector<double> outputs;
	outputs.reserve(m_nOutputs);

	// index of current neuron
	size_t i = 0;

	// set outputs of input neurons to be equal to the inputs
	while (i < m_nInputs)
	{
		m_pNeurons[i]->SetOutput(inputs[i]);
		i++;
	}

	// set output of bias neuron to be 1
	m_pNeurons[i]->SetOutput(1.0);
	i++;

	// process hidden neurons
	while (i < m_pNeurons.size() - m_nOutputs)
	{
		m_pNeurons[i]->Process();
		i++;
	}
	
	// process output neurons, storing results in outputs vector
	while (i < m_pNeurons.size())
	{
		m_pNeurons[i]->Process();
		outputs.push_back(m_pNeurons[i]->Output());
		i++;
	}

	return outputs;
}


void NeuralNetwork::AddNeuron(int id, NeuronType type, double depth, double response)
{
	m_pNeurons.push_back(new Neuron(id, type, depth, response));
	m_neuronIDs.push_back(id);
	if (type == NeuronType::INPUT) m_nInputs++;
	if (type == NeuronType::OUTPUT) m_nOutputs++;
}


void NeuralNetwork::AddLink(double weight, int idFrom, int idTo, bool recurrent)
{
	int i = GetIndex(idFrom);
	int j = GetIndex(idTo);

	if (i == -1 || j == -1)
	{
		printf("Error in NeuralNetwor::AddLink - id not found\n");
		return;
	}
	m_pNeurons[j]->AddLinkIn(weight, m_pNeurons[i], recurrent);
}


// do not use for recurrent links
void NeuralNetwork::AddLinks(const std::vector<double>& weights,
	const std::vector<int>& idsFrom, int idTo)
{
	for (size_t k = 0; k < weights.size(); k++)
	{
		AddLink(weights[k], idsFrom[k], idTo, false);
	}
}


int NeuralNetwork::GetIndex(int id)
{
	for (size_t i = 0; i < m_neuronIDs.size(); i++)
	{
		if (id == m_neuronIDs[i]) return i;
	}
	return -1;
}

