#ifndef _GAME_POOL_H_
#define _GAME_POOL_H_

#include <memory>

class PoolBase; // fw decl

class PoolNodeBase
{
protected:
    bool inUse;
public:
    friend class PoolBase;
};

template <typename T>
class PoolNode : public PoolNodeBase
{
private:
    T _value;
public:
    virtual ~PoolNode(){}
    virtual T* value(){ return &_value; }
};

class PoolBase
{
protected:
    bool acquireNode(PoolNodeBase& node)
    {
        if (!node.inUse)
        {
            node.inUse = true;
            return true;
        }
        return false;
    }
public:
    virtual void release(PoolNodeBase* node){ node->inUse = false; }
};

class PoolPointerDeleter
{
private:
    PoolBase* pool;
    PoolNodeBase* node;
public:
    PoolPointerDeleter(PoolBase* pool, PoolNodeBase* node)
        : pool{pool}, node{node} {}
    void operator()(void*) const
    {
        if (pool&&node)
            pool->release(node);
    }
};

template <typename T>
using PoolPointer = std::unique_ptr<T, PoolPointerDeleter>;

template <typename T, size_t N>
class Pool : public PoolBase
{
private:
    PoolNode<T> nodes[N];
public:
    virtual PoolNode<T>* acquire()
    {
        for (size_t i = 0; i<N; i++)
        {
            if (acquireNode(nodes[i]))
                return &nodes[i];
        }
        return nullptr;
    }
    virtual PoolPointer<T> acquireUnique()
    {
        PoolNode<T>* node = acquire();
        return PoolPointer<T>(node->value(), PoolPointerDeleter(this, node));
    }
    size_t capacity(){ return N; }
};

class RAIIPoolNode
{
private:
    PoolBase* pool;
    PoolNodeBase* node;
public:
    RAIIPoolNode(PoolBase* pool, PoolNodeBase* node)
        : pool{pool}, node{node} {}
    ~RAIIPoolNode(){ pool->release(node); }
};

#endif
