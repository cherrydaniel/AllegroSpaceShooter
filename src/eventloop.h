#ifndef _GAME_EVENTLOOP_H_
#define _GAME_EVENTLOOP_H_

#include <functional>
#include <queue>
#include <mutex>

using EventLoopCallback = std::function<void(void)>;

class EventLoop
{
private:
    std::queue<EventLoopCallback> mainThreadCallbacks;
    std::mutex mtx;
public:
    void tick();
    void runOnMain(EventLoopCallback&& fn);
};

#endif
