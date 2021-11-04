#pragma once

#include "NeuronType.h"
#include <SFML/Graphics.hpp>
#include <vector>

class Link;

class Neuron
{
	friend class Link;

public:
	Neuron(int id, NeuronType type, double depth, double response);

	void AddLinkIn(double weight, Neuron* pFrom, bool recurrent);

	void Process();



	void Draw(sf::RenderWindow& window);

private:
	int m_id;
	NeuronType m_type;

	std::vector<Link> m_linksIn;
	double m_response;

	double m_output;

	double m_depth;

	sf::Vector2f m_pos;

	//sf::Font montserrat;
	//sf::Text m_text;

public:
	int ID() { return m_id; }
	double Depth() { return m_depth; }
	double Output() { return m_output; }
	void SetOutput(double val) { m_output = val; }
};
