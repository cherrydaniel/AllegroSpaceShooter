#ifndef _GAME_DURATION_H_
#define _GAME_DURATION_H_

#include <chrono>
#include <cstring>
#include "common.h"

#define LABEL_LEN 100

class Duration
{
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    char label[LABEL_LEN];
public:
    Duration(const char* label) :
        start{std::chrono::high_resolution_clock::now()}
    {
        strcpy_s(this->label, label);
    }
    ~Duration()
    {
        auto end = std::chrono::high_resolution_clock::now();
        long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        PRINT_DEBUG(ANSI_COLOR_GREEN ANSI_COLOR_BOLD "[%s] %lldms" ANSI_COLOR_RESET, label, duration);
    }
};

#define FN_DURATION() Duration fn_duration_{__FUNCTION__};

#endif
