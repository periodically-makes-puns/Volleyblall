#include "Game.h"

Game::Game(Player* p1, Player* p2, RandomEngine* rng) {
	this->p1 = p1;
	this->p2 = p2;
	p1->rng = rng;
	p2->rng = rng;
	this->rng = rng;
}

bool Game::run_point(bool server) {
	// true -> player 1, false -> player 2
	Player* inactive = (server) ? this->p1 : this->p2;
	Player* playing = (!server) ? this->p1 : this->p2;
	inactive->clear_mods();
	playing->clear_mods();

	short roll = inactive->roll_srv();
	if (roll == 0) return !server;
	if (roll >= 9) roll -= 10;
	unsigned int exchanges = 1;
	bool active = !server;
	bool isWipe = false;
	while (true) { // Exchange cycle
		roll = playing->roll_rec(-roll, isWipe);
		isWipe = false;
		if (roll == 0) return !active;

		roll = playing->roll_set(roll, roll > 1);
		if (roll == 0) return !active;

		short sps = 0;
		if (roll != 6) {
			sps = playing->roll_spk(roll);
			if (playing->indecisive && (rng->getReal() < 0.1 || exchanges > 5)) {
				inactive->mBlk += 3;
				inactive->mRec += 3;
			}
		}

		std::swap(inactive, playing);
		active = !active;
		exchanges++;

		short blk = 0;
		if (roll != 6) {
			// roll block;
			blk = playing->roll_blk(0);
			switch (blk) {
			case -1:
				playing->mRec += 7;
				blk = 10;
				break;
			case 0:
				blk = 10;
				break;
			case 1:
				blk = 3;
				break;
			case 2:
				blk = 0;
				break;
			case 3:
				blk = -2;
				break;
			}
		}
		if (roll != 6 && inactive->resourceful && blk < 1 && rng->getReal() < 0.333) {
			sps = -2;
			std::swap(inactive, playing);
			active = !active;
		} else {
			if (roll != 6) roll = playing->roll_pss(blk + 2 - roll, playing->height - inactive->height);
			if (blk != 10) {
				double r, pred;
				switch (roll) {
				case 6: //dump
					r = rng->getReal();
					pred = (double) playing->blk / 40;
					pred += (playing->vigilant) ? 0.25 : 0;
					pred -= (playing->unsuspecting) ? 0.15 : 0;
					if (r < 0.2 + pred) return active;
					else if (r > 0.9 || (r > 0.6 && playing->unsuspecting)) return !active;
					else sps = 3;
					break;
				case 4: // wipe
					isWipe = true;
					sps += 2;
					break;
				case 3:
					sps++;
					break;
				case 0:
					return active;
				}
			}
		}
		roll = sps;
	}
}

inline int max(int a, int b) {
	return (a > b) ? a : b;
}

inline int min(int a, int b) {
	return (a < b) ? a : b;
}

bool Game::run_set(bool server) {
	int s1 = 0, s2 = 0;
	while (max(s1, s2) < 25 || max(s1, s2) - min(s1, s2) < 2) {
		p1->buff = p2->buff = 0;
		if (s1 > s2 + 4) {
			if (p1->moody) p1->buff++;
			if (p2->moody) p2->buff--;
			if (p1->complacent) p1->buff--;
			if (p2->resilient) p2->buff++;
		} else if (s2 > s1 + 4) {
			if (p2->moody) p2->buff++;
			if (p1->moody) p1->buff--;
			if (p2->complacent) p2->buff--;
			if (p1->resilient) p1->buff++;
		}
		server = this->run_point(server);
		 (server) ? s1++ : s2++;
		 // I love optimizing if statements.
	}
	return s1 > s2;
}

bool Game::run_match() {
	int s1 = 0, s2 = 0, setNum = 0;
	short arrogantMods[2][5] = { { 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0 } };
	if (p1->statSum() >= p2->statSum() + 10 && p1->arrogant) {
		for (int i = 0; i < 5; i++)
			arrogantMods[0][i] = rng->getReal() * 4;
	}
	if (p2->statSum() >= p1->statSum() + 10 && p2->arrogant) {
		for (int i = 0; i < 5; i++)
			arrogantMods[1][i] = rng->getReal() * 4;
	}
	p1->srv -= arrogantMods[0][0];
	p1->rec -= arrogantMods[0][1];
	p1->set -= arrogantMods[0][2];
	p1->spk -= arrogantMods[0][3];
	p1->blk -= arrogantMods[0][4];
	p2->srv -= arrogantMods[1][0];
	p2->rec -= arrogantMods[1][1];
	p2->set -= arrogantMods[1][2];
	p2->spk -= arrogantMods[1][3];
	p2->blk -= arrogantMods[1][4];
	while (max(s1, s2) < 2) {
		bool server = setNum % 2;
		//if (setNum == 2) server = (short)(TableUtil::getReal() * 2);
		(this->run_set(server)) ? s1++ : s2++;
		setNum++;
	}
	p1->srv += arrogantMods[0][0];
	p1->rec += arrogantMods[0][1];
	p1->set += arrogantMods[0][2];
	p1->spk += arrogantMods[0][3];
	p1->blk += arrogantMods[0][4];
	p2->srv += arrogantMods[1][0];
	p2->rec += arrogantMods[1][1];
	p2->set += arrogantMods[1][2];
	p2->spk += arrogantMods[1][3];
	p2->blk += arrogantMods[1][4];
	return s1 > s2;
}
