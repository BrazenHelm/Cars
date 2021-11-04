#include "Params.h"


int Params::POP_SIZE;
int Params::N_INPUTS;
int Params::N_OUTPUTS;
double Params::TICK_RATE;
double Params::SIM_TIME_BASE;
double Params::SIM_TIME_INCR;
int Params::YOUNG_AGE_THRESHOLD;
int Params::OLD_AGE_THRESHOLD;
double Params::YOUNG_AGE_BONUS;
double Params::OLD_AGE_PENALTY;
double Params::SURVIVAL_RATE;
double Params::CROSSOVER_RATE;
double Params::CHANCE_TO_ADD_NEURON;
double Params::CHANCE_TO_ADD_LINK;
double Params::CHANCE_TO_MUTATE_WEIGHT;
double Params::CHANCE_OF_TOTAL_WEIGHT_MUTATION;
double Params::MAX_WEIGHT_PERTURBATION;
int Params::N_TRIES_TO_MAKE_NEW_NEURON;
int Params::N_TRIES_TO_MAKE_NEW_LINK;
int Params::MAX_NEURONS;
int Params::N_GENS_ALLOWED_NO_IMPROVEMENT;
double Params::COMPATIBILITY_THRESHOLD;



Params::Params()
{
	// fundamental parameters
	POP_SIZE = 100;
	N_INPUTS = 6;
	N_OUTPUTS = 4;				// do not change from 4

	// time parameters
	TICK_RATE = 60.0;			// simulated fps (default 60)
	SIM_TIME_BASE = 5.0;		// time to simulate on generation 1 in seconds
	SIM_TIME_INCR = 2.0;		// extra time at each generation in seconds

	// evolution parameters
	SURVIVAL_RATE = 0.2;		// survival rate between generations
	CROSSOVER_RATE = 0.8;		// chance to apply crossover

	// species parameters
	N_GENS_ALLOWED_NO_IMPROVEMENT = 15;
	COMPATIBILITY_THRESHOLD = 0.40;
	YOUNG_AGE_THRESHOLD = 10;	// a species this young has its fitness boosted
	YOUNG_AGE_BONUS = 0.3;		// by this much
	OLD_AGE_THRESHOLD = 50;		// a species this old has its fitnesses penalised
	OLD_AGE_PENALTY = 0.3;		// by this much

	// mutation parameters
	CHANCE_TO_ADD_NEURON = 0.04 / 3.0;
	CHANCE_TO_ADD_LINK = 0.07 / 3.0;
	CHANCE_TO_MUTATE_WEIGHT = 0.2 / 3.0;
	CHANCE_OF_TOTAL_WEIGHT_MUTATION = 0.1;
	MAX_WEIGHT_PERTURBATION = 0.5;
	N_TRIES_TO_MAKE_NEW_NEURON = 10;
	N_TRIES_TO_MAKE_NEW_LINK = 10;
	MAX_NEURONS = 100;
}

