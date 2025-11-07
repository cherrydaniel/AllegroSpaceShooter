#include "threadpool.h"

void* threadPoolSpawn(ALLEGRO_THREAD* t, void* arg)
{
    ThreadData* tdata = static_cast<ThreadData*>(arg);
    while (!al_get_thread_should_stop(t))
    {
        al_lock_mutex(tdata->mtx);
        while (tdata->tasks.empty()&&!al_get_thread_should_stop(t))
        {
            al_wait_cond(tdata->cv, tdata->mtx);
            PRINT_DEBUG("signalled");
        }
        PRINT_DEBUG("Release! alive=%d", al_get_thread_should_stop(t) ? 0 : 1);
        if (al_get_thread_should_stop(t))
        {
            PRINT_DEBUG("STOPPING!");
            al_unlock_mutex(tdata->mtx);
            break;
        }
        std::function<void(void)> task = std::move(tdata->tasks.front());
        tdata->tasks.pop();
        al_unlock_mutex(tdata->mtx);
        PRINT_DEBUG("EXEC TASK");
        task();
    }
    PRINT_DEBUG("THREAD DONE!");
    return nullptr;
}

ThreadPool::ThreadPool(size_t size)
{
    PRINT_DEBUG("ThreadPool CONSTRUCTOR %d", size);
    this->size = size;
    threads = new ALLEGRO_THREAD*[size];
    tdata.mtx = al_create_mutex();
    tdata.cv = al_create_cond();
    for (size_t i = 0; i<size; i++)
    {
        threads[i] = al_create_thread(threadPoolSpawn, &tdata);
        al_start_thread(threads[i]);
    }
}

ThreadPool::~ThreadPool()
{
    PRINT_DEBUG("ThreadPool DESTRUCTOR - START");
    for (size_t i = 0; i<size; i++)
        al_set_thread_should_stop(threads[i]);
    al_broadcast_cond(tdata.cv);
    for (size_t i = 0; i<size; i++)
    {
        PRINT_DEBUG("ThreadPool DESTRUCTOR - %d", i);
        al_join_thread(threads[i], nullptr);
        PRINT_DEBUG("ThreadPool DESTRUCTOR join - %d", i);
        al_destroy_thread(threads[i]);
        PRINT_DEBUG("ThreadPool DESTRUCTOR destroy - %d", i);
    }
    PRINT_DEBUG("ThreadPool DESTRUCTOR - DESTROYED");
    al_destroy_cond(tdata.cv);
    al_destroy_mutex(tdata.mtx);
    delete[] threads;
    PRINT_DEBUG("ThreadPool DESTRUCTOR - DONE");
}

void ThreadPool::enqueue(std::function<void(void)>&& task)
{
    al_lock_mutex(tdata.mtx);
    tdata.tasks.push(std::move(task));
    al_unlock_mutex(tdata.mtx);
    al_broadcast_cond(tdata.cv);
}