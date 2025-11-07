#ifndef _GAME_THREADPOOL_H_
#define _GAME_THREADPOOL_H_

#include <functional>
#include <queue>
#include <allegro5/allegro.h>
#include "util/common.h"

struct ThreadData
{
    std::queue<std::function<void(void)>> tasks;
    ALLEGRO_MUTEX* mtx;
    ALLEGRO_COND* cv;
};

void* threadPoolSpawn(ALLEGRO_THREAD* t, void* arg);

class ThreadPool
{
private:
    size_t size;
    ALLEGRO_THREAD** threads;
    ThreadData tdata;
public:
    ThreadPool(size_t size);
    ~ThreadPool();
    void enqueue(std::function<void(void)>&& task);
};

#endif
