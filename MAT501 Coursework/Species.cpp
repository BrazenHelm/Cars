#include "Species.h"
#include "Params.h"
#include "Random.h"


Species::Species(Genome& first, int id) :
	m_speciesID(id), m_bestFitness(first.Fitness()), m_nGensNoImprovement(0),
	m_age(0), m_leader(first), m_nToSpawn(0.0)
{
	m_pMembers.push_back(&first);
}


void Species::AddMember(Genome& newMember)
{
	m_pMembers.push_back(&newMember);
	
	if (newMember.Fitness() > m_bestFitness)
	{
		m_bestFitness = newMember.Fitness();
		m_nGensNoImprovement = 0;
		m_leader = newMember;
	}
}


void Species::Purge()
{
	m_pMembers.clear();
	m_age++;
	m_nGensNoImprovement++;
	m_nToSpawn = 0;
}


void Species::AdjustFitnesses()
{
	double total = 0.0;

	for (size_t i = 0; i < m_pMembers.size(); i++)
	{
		double fitness = m_pMembers[i]->Fitness();

		if (m_age < Params::YOUNG_AGE_THRESHOLD)
		{
			fitness *= (1.0 + Params::YOUNG_AGE_BONUS);
		}
		if (m_age > Params::OLD_AGE_THRESHOLD)
		{
			fitness *= (1.0 - Params::OLD_AGE_PENALTY);
		}

		total += fitness;

		m_pMembers[i]->SetAdjFitness(fitness / m_pMembers.size());
	}
}


void Species::CalculateNumToSpawn()
{
	for (size_t i = 0; i < m_pMembers.size(); i++)
	{
		m_nToSpawn += m_pMembers[i]->NToSpawn();
	}
}


Genome Species::Spawn()
{
	Genome baby;
	
	if (m_pMembers.size() == 1)
	{
		baby = *m_pMembers[0];
	}
	else
	{
		int maxIndex = (int)(Params::SURVIVAL_RATE * m_pMembers.size()) + 1;
		int choice = Random::Int(0, maxIndex);
		baby = *m_pMembers[choice];
	}

	return baby;
}

