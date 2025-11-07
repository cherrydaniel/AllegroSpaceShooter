#ifndef _GAME_RND_H_
#define _GAME_RND_H_

#include <random>
#include <memory>

class RND
{
private:
    std::unique_ptr<std::random_device> rd;
    std::unique_ptr<std::mt19937> gen;
public:
    RND()
    {
        rd = std::make_unique<std::random_device>();
        gen = std::make_unique<std::mt19937>((*rd)());
    }
    int range(int min, int max)
    {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(*gen);
    };
};

#endif
