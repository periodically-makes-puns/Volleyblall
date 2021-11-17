#pragma once
#include <random>
#include <algorithm>
#include <cassert>


class RandomEngine {
public:
	enum class Action { SRV, REC, SET, SPK, BLK, PSS };

	std::mt19937_64 gen;
	RandomEngine();
	double getReal();
	short roll(Action ID, short skl, short extra);
};

