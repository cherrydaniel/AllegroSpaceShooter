#ifndef _GAME_AYSNC_SCOPE_H_
#define _GAME_AYSNC_SCOPE_H_

#include <future>
#include <vector>
#include <functional>

class AsyncScope
{
private:
    std::vector<std::future<void>> futures;
public:
    void dispatch(std::function<void(void)> fn)
    {
        futures.push_back(std::async(std::launch::async, fn));
    }
};

#endif
