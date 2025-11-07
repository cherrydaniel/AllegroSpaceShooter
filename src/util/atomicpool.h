#ifndef _GAME_ATOMICPOOL_H_
#define _GAME_ATOMICPOOL_H_

#include <mutex>
#include "pool.h"

template <typename T, size_t N>
class AtomicPool : public Pool<T, N>
{
private:
    std::mutex mtx;
public:
    virtual PoolNode<T>* acquire() override
    {
        std::lock_guard<std::mutex> lock{mtx};
        return Pool<T, N>::acquire();
    }
    virtual void release(PoolNodeBase* node) override
    {
        std::lock_guard<std::mutex> lock{mtx};
        PoolBase::release(node);
    }
};

#endif
