#pragma once


struct Params
{
	static int POP_SIZE;
	static int N_INPUTS;
	static int N_OUTPUTS;

	static double TICK_RATE;
	static double SIM_TIME_BASE;
	static double SIM_TIME_INCR;

	static int N_GENS_ALLOWED_NO_IMPROVEMENT;
	static double COMPATIBILITY_THRESHOLD;
	static int YOUNG_AGE_THRESHOLD;
	static double YOUNG_AGE_BONUS;
	static int OLD_AGE_THRESHOLD;
	static double OLD_AGE_PENALTY;

	static double SURVIVAL_RATE;
	static double CROSSOVER_RATE;

	static double CHANCE_TO_ADD_NEURON;
	static double CHANCE_TO_ADD_LINK;
	static double CHANCE_TO_MUTATE_WEIGHT;
	static double CHANCE_OF_TOTAL_WEIGHT_MUTATION;
	static double MAX_WEIGHT_PERTURBATION;
	static int N_TRIES_TO_MAKE_NEW_NEURON;
	static int N_TRIES_TO_MAKE_NEW_LINK;
	static int MAX_NEURONS;



	Params();
};

