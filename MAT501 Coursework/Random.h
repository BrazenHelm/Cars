#pragma once
#include <random>

class Random
{
private:
	static std::mt19937 rng;

public:
	// returns a double from a uniform distribution on [min, max]
	static double Double(double min, double max);

	// returns an int from a uniform distribution on [min, max]
	static int Int(int min, int max);

	// seeds rng
	static void Seed();
	static void Seed(unsigned int seed);
};

