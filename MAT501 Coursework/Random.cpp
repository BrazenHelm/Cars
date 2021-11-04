#include "Random.h"
#include <time.h>


std::mt19937 Random::rng;



double Random::Double(double min, double max)
{
	auto dist = std::uniform_real_distribution<double>(min, max);
	return dist(rng);
}


int Random::Int(int min, int max)
{
	auto dist = std::uniform_int_distribution<int>(min, max);
	return dist(rng);
}


void Random::Seed()
{
	unsigned int seed = std::random_device{}();
	printf("Seeding with %i\n", seed);
	rng.seed(seed);
}


void Random::Seed(unsigned int seed)
{
	printf("Seeding with %i\n", seed);
	rng.seed(seed);
}

