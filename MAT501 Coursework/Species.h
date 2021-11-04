#pragma once
#include "Genome.h"

class Species
{
private:
	Genome m_leader;
	std::vector<Genome*> m_pMembers;
	int m_speciesID;
	double m_bestFitness;
	int m_nGensNoImprovement;
	int m_age;
	double m_nToSpawn;

public:
	Species(Genome& first, int id);
	
	void AdjustFitnesses();

	void AddMember(Genome& newMember);

	void Purge();

	void CalculateNumToSpawn();

	Genome Spawn();

	// use best fitness as criterion for sorting
	friend bool operator<(const Species& lhs, const Species& rhs)
	{
		return (lhs.m_bestFitness > rhs.m_bestFitness);
	}

public:
	Genome Leader() const { return m_leader; }
	double NToSpawn() const { return m_nToSpawn; }
	int Population() const { return m_pMembers.size(); }
	int NGensNoImprovement() const { return m_nGensNoImprovement; }
	double LeaderFitness() const { return m_leader.Fitness(); }
	double BestFitness() const { return m_bestFitness; }
	int Age() const { return m_age; }
	int ID() const { return m_speciesID; }
};

