#include "Game.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <map>
#include <cstring>

constexpr int REPS = 1000;

int wins[400][400];
long long rtime[400][400];

int main() {
	std::ifstream pdata("player_data.txt");
	std::ofstream wfile("wins.csv");
	std::ofstream tfile("time.csv");
	std::string line;
	std::vector<Player*> players;
	Player* p = NULL;
	while (!pdata.eof()) {
		getline(pdata, line);
		p = new Player(line, NULL);
		players.push_back(p);
	}
	Player *p1, *p2;
	using namespace std::chrono;
	
	high_resolution_clock::time_point ctime = high_resolution_clock::now();
	int s1 = 0, s2 = 0, N = players.size();
	RandomEngine rng;
	for (int pi = 0; pi < N; pi++) {
		p1 = players[pi];
		for (int pj = pi + 1; pj < N; pj++) {
			p2 = players[pj];
			high_resolution_clock::time_point prev = high_resolution_clock::now();
			for (int _ = 0; _ < REPS; _++) {
				Game g(p1, p2, &rng);
				(g.run_match()) ? wins[pi][pj]++ : wins[pj][pi]++;
			}
			nanoseconds dt = high_resolution_clock::now() - prev;
			rtime[pi][pj] = rtime[pj][pi] = dt.count();
			//std::cout << "\33[2K\r" << p1->name << ", " << p2->name << " " <<
			//	wins[pi][pj] << "-" << wins[pj][pi];
		}
	}
	
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++) {
			wfile << wins[i][j];
			if (j == N - 1) wfile << std::endl;
			else wfile << ", ";
		}

	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++) {
			tfile << rtime[i][j];
			if (j == N - 1) tfile << std::endl;
			else tfile << ", ";
		}

	wfile.close();
	tfile.close();
	nanoseconds t = high_resolution_clock::now() - ctime;
	//std::cout << "\33[2K\r";
	std::cout << t.count() / 1000000000.0 << "s" << std::endl;
}
