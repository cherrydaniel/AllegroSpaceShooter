#ifndef _GAME_FIXEDSTACK_H_
#define _GAME_FIXEDSTACK_H_

template <typename T, size_t N>
class FixedStack
{
private:
    T arr[N];
    size_t idx;
public:
    FixedStack() : idx{0} {}
    ~FixedStack(){}
    bool push(T item)
    {
        if (idx==N)
            return false;
        arr[idx++] = item;
        return true;
    }
    T pop(){ return arr[--idx]; }
    size_t size(){ return idx; }
    size_t capacity(){ return N; }
};

#endif
