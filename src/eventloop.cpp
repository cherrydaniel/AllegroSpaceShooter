#include "eventloop.h"

void EventLoop::tick()
{
    while (!mainThreadCallbacks.empty())
    {
        EventLoopCallback cb;
        {
            std::lock_guard<std::mutex> lock{mtx};
            cb = std::move(mainThreadCallbacks.front());
            mainThreadCallbacks.pop();
        }
        cb();
    }
}

void EventLoop::runOnMain(EventLoopCallback&& fn)
{
    std::lock_guard<std::mutex> lock{mtx};
    mainThreadCallbacks.emplace(std::move(fn));
}
