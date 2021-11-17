#pragma once
#include "./Player.h"
#include <utility>

class Game {
public:
	Game(Player* p1, Player* p2,
		RandomEngine* rng);
	Player *p1, *p2;
	RandomEngine* rng;
	bool run_point(bool server);
	bool run_set(bool server);
	bool run_match();
};

