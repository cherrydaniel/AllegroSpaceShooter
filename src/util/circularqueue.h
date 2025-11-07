#ifndef _GAME_CIRCQ_H_
#define _GAME_CIRCQ_H_

template <typename T, size_t N>
class CircularQueue
{
private:
    T buf[N+1];
    size_t begin;
    size_t end;
public:
    CircularQueue() : begin{0}, end{0} {}
    bool push(T& item)
    {
        if (full())
            return false;
        buf[end++] = item;
        if (end==N+1)
            end = 0;
        return true;
    }
    template <typename... Args>
    bool emplace(Args&&... args)
    {
        if (full())
            return false;
        buf[end++] = T(std::forward<Args>(args)...);
        if (end==N+1)
            end = 0;
        return true;
    }
    bool read(T* dst)
    {
        if (empty())
            return false;
        *dst = buf[begin++];
        if (begin==N+1)
            begin = 0;
        return true;
    }
    size_t size(){ return begin<=end ? end-begin : end+N-1-begin; }
    bool empty(){ return !size(); }
    bool full(){ return size()==N; }
    size_t capacity(){ return N; }
};

#endif
