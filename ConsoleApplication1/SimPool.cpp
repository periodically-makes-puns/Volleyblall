#include "SimPool.h"


typedef struct Job {
    int p1, p2;
} Job;

SimPool::SimThread::SimThread(SimPool* parent, std::vector<Player*>& players) {
    this->parent = parent;
    thread = std::thread(std::bind(&SimPool::SimThread::run, this));
    for (auto it : players) this->players.push_back(new Player(*it));
    rng = new RandomEngine();
}

void SimPool::SimThread::run() {
    while (true) {
        Job job;
        {
            std::unique_lock<std::mutex> lock(parent->queue_mutex);
            parent->condition.wait(lock, [this]() {
                return !parent->jobs.empty() || parent->terminate_pool;
            });
            if (parent->jobs.empty()) {
                lock.unlock();
                break;
            }
            job = parent->jobs.front();
            parent->jobs.pop();
        }

        Game g(players[job.p1], players[job.p2], rng);
        int w = 0, l = 0;
        for (int i = 0; i < REPS; i++) (g.run_match()) ? w++ : l++;
        
        parent->results[job.p1][job.p2] = w;
        parent->results[job.p2][job.p1] = l;
    }
}

SimPool::SimPool(std::vector<Player*>& players) {
    this->players = players;
    terminate_pool = false;
    for (int i = 0; i < MAX_THREADS; i++) {
        threads.push_back(new SimThread(this, players));
    }
    memset(results, 0, sizeof(results));
}

void SimPool::add_job(Job job) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        jobs.push(job);
    }
    condition.notify_one();
}

void SimPool::shutdown() {
    terminate_pool = true;
    condition.notify_all();
    for (auto thread : threads) thread->thread.join();
}
