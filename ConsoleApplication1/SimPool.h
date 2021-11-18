#pragma once
#define REPS 1000

#include "Game.h"
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

class SimPool {
public:
	class SimThread {
	public:
		SimPool* parent;
		std::thread thread;
		std::vector<Player*> players;
		RandomEngine* rng;
		SimThread(SimPool* parent, std::vector<Player*>& players);
		void run();
	};

	struct Job {
		int p1, p2;
	};
	const int MAX_THREADS = std::thread::hardware_concurrency();
	std::vector<SimThread*> threads;
	std::vector<Player*> players;
	std::queue<Job> jobs;
	std::mutex queue_mutex;
	std::condition_variable condition;
	bool terminate_pool;

	int results[400][400];

	SimPool(std::vector<Player*>& players);
	void add_job(Job job);
	void shutdown();
};



