#include "SimPool.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <map>
#include <cstring>
#include <thread>

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
	using namespace std::chrono;
	int N = (int)players.size();
	high_resolution_clock::time_point ctime = high_resolution_clock::now();
	SimPool pool(players);

	for (int i = 0; i < N; i++)
		for (int j = i + 1; j < N; j++)
			pool.add_job(SimPool::Job(i, j));

	pool.shutdown();

	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++) {
			wfile << pool.results[i][j];
			if (j == N - 1) wfile << std::endl;
			else wfile << ", ";
		}

	wfile.close();
	tfile.close();
	nanoseconds t = high_resolution_clock::now() - ctime;
	std::cout << "\33[2K\r";
	std::cout << t.count() / 1000000000.0 << "s" << std::endl;

	std::cin.get();
}
