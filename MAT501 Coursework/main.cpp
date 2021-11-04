#include <iostream>
#include <sstream>
#include "Controller.h"
#include "Params.h"
#include "Random.h"


void ShowCommandsList();

int main()
{
	Params params;

	const int NUM_GENERATIONS = 5;
	const int SIMULATION_TIME = 5;	// in seconds

	Controller controller;
	controller.Start(Params::POP_SIZE, Params::N_INPUTS, Params::N_OUTPUTS);

	printf("========== MAT 501 Coursework Application ==========\n");
	ShowCommandsList();

	while (true)
	{
		printf("\n> ");
		std::string command;
		std::getline(std::cin, command);

		std::stringstream ss(command);
		std::istream_iterator<std::string> begin(ss);
		std::istream_iterator<std::string> end;
		std::vector<std::string> words(begin, end);
		
		if (words.size() > 0)
		{
			std::string firstWord = words[0];

			if (firstWord == "quit")
			{
				break;
			}

			if (firstWord == "reset")
			{
				controller.Reset();
				printf("Reset completed\n");
				continue;
			}

			if (firstWord == "help")
			{
				ShowCommandsList();
				continue;
			}

			if (firstWord == "seed")
			{
				if (words.size() >= 2)
				{
					unsigned int seed = std::stoi(words[1]);
					Random::Seed(seed);
					continue;
				}
				else
				{
					Random::Seed();
					continue;
				}
			}

			if (words.size() >= 2 && words[0] == "write")
			{
				controller.WriteFitnessesToFile(words[1]);
				printf("Writing completed\n");
				continue;
			}

			if (words.size() >= 3 && words[0] == "set" && words[1] == "track")
			{
				controller.SetTrack(words[2]);
				continue;
			}

			if (words.size() >= 4)
			{
				if (words[0] == "do" && words[2] == "gens" && words[3] == "fast")
				{
					int nGens = std::stoi(words[1]);
					while (nGens--)
					{
						controller.DoGeneration(true);
					}
					printf("Generations completed\n");
					continue;
				}
				if (words[0] == "do" && words[2] == "gens" && words[3] == "slow")
				{
					int nGens = std::stoi(words[1]);
					while (nGens--)
					{
						controller.DoGeneration(false);
					}
					printf("Generations completed\n");
					continue;
				}
				if (words[0] == "do" && words[2] == "gens" && words[3] == "test")
				{
					int nGens = std::stoi(words[1]);
					while (nGens--)
					{
						controller.DoTestedGeneration();
					}
					printf("Generations completed\n");
					continue;
				}
				if (words[0] == "show" && words[1] == "best")
				{
					int nToShow = std::stoi(words[2]);
					double simTime = std::stod(words[3]);
					controller.ShowOffBest(nToShow, simTime);
					printf("Simulation completed\n");
					continue;
				}
			}
		}

		printf("Invalid command, try again (type \"help\" for commands list)\n");
	}

	system("pause");
	return 0;
}


void ShowCommandsList()
{
	std::cout << "\nCommands List\n"
		<< "* do x gens fast : perform x generations at max speed - no rendering\n"
		<< "* do x gens slow : perform x generations rendering all cars - limits tick rate\n"
		<< "* do x gens test : perform x generations fast, testing after each generation\n"
		<< "* help           : show this commands list\n"
		<< "* reset          : reset population to random minimal networks\n"
		<< "* seed           : seeds rng with a randomly generated unsigned int\n"
		<< "* seed x         : seeds rng with the given unsigned int x\n"
		<< "* set track x    : set the track - options for x are square, train and test\n"
		<< "* show best x y  : simulate x best brains from last generation for y seconds\n"
		<< "* quit           : quit the program\n"
		<< "* write x        : write fitness scores to file with filename - should end .csv x\n";
}

