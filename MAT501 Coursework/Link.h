#pragma once

class Neuron;

class Link
{
	friend class Neuron;

public:
	Link(double weight, Neuron* pFrom, Neuron* pTo, bool recurrent) :
		m_weight(weight), m_pFrom(pFrom), m_pTo(pTo), m_isRecurrent(recurrent)
	{};

	double Value();

private:
	double m_weight;
	Neuron* m_pFrom;
	Neuron* m_pTo;
	bool m_isRecurrent;
};