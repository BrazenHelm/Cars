#include "Neuron.h"
#include "Link.h"
#include <iostream>




Neuron::Neuron(int id, NeuronType type, double depth, double response) :
	m_id(id), m_type(type), m_depth(depth), m_response(response)
{
	m_linksIn = std::vector<Link>();



	switch (type)
	{
		case NeuronType::INPUT: case NeuronType::BIAS:
			m_pos.x = 75.f;
			m_pos.y = 75.f * (1 + id);
			break;
		case NeuronType::HIDDEN:
			m_pos.x = 175.f;
			m_pos.y = 75.f * (1 + id - 4);
			break;
		case NeuronType::OUTPUT:
			m_pos.x = 275.f;
			m_pos.y = 75.f * (1 + id - 4);
			break;
		default:
			m_pos.x = 0.f;
			m_pos.y = 0.f;
			break;
	}

	//montserrat.loadFromFile("Assets/Montserrat-Regular.ttf");
	//m_text.setFont(montserrat);
	//m_text.setCharacterSize(16);
	//m_text.setFillColor(sf::Color::White);
	//m_text.setPosition(m_pos);
	//m_text.setOrigin(25, 25);
}


void Neuron::Draw(sf::RenderWindow& window)
{
	sf::CircleShape circle(25);
	circle.setPosition(m_pos);
	circle.setOrigin(25, 25);
	
	sf::Color color;
	if (m_type == NeuronType::INPUT) color = sf::Color::Red;
	if (m_type == NeuronType::BIAS) color = sf::Color::Magenta;
	if (m_type == NeuronType::HIDDEN) color = sf::Color::Blue;
	if (m_type == NeuronType::OUTPUT) color = sf::Color::Blue;

	circle.setFillColor(color);
	window.draw(circle);

	//m_text.setString(std::to_string(m_output));
	//window.draw(m_text);

	for (Link link : m_linksIn)
	{
		sf::Vertex verts[2];
		verts[0].position = link.m_pFrom->m_pos;
		verts[1].position = m_pos;
		verts[0].color = sf::Color(255, 255, 255, 255);
		verts[1].color = sf::Color(255, 255, 255, 255);
		window.draw(verts, 2, sf::Lines);
		//sf::Text linkText;
		//linkText.setFont(montserrat);
		//linkText.setCharacterSize(12);
		//linkText.setFillColor(sf::Color(230, 230, 230));
		//linkText.setPosition((verts[0].position + verts[1].position) / 2.f);
		//linkText.setString(std::to_string(link.m_weight));
		//window.draw(linkText);
	}
}


void Neuron::AddLinkIn(double weight, Neuron* pFrom, bool recurrent)
{
	m_linksIn.emplace_back(weight, pFrom, this, recurrent);
}


void Neuron::Process()
{
	double sum = 0.0;
	for (Link link : m_linksIn)
	{
		sum += link.Value();
	}
	m_output = 1.0 / (1.0 + exp(-sum / m_response));
}

