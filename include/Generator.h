#ifndef GENERATOR_H
#define GENERATOR_H

#include <random>

class Generator
{
public:
	Generator() : generator(randev())
	{
		std::uniform_int_distribution<int> roll(1,6);	// default one to be sure sth is set
	}

	int roll_int(int a, int b)
	{
		int min, max;
		if (a < b) { min = a; max = b; }
		else if (a > b) { min = b; max = a; }
		else return a;
		std::uniform_int_distribution<int> roll(min,max);
		return roll(generator);
	}

	double roll_double(double a, double b)
	{
		double min, max;
		if (a < b) { min = a; max = b; }
		else if (a > b) { min = b; max = a; }
		else return a;
		std::uniform_real_distribution<double> roll(min,max);
		return roll(generator);
	}

private:
	std::random_device randev;
	std::mt19937 generator;
};

#endif // GENERATOR_H
