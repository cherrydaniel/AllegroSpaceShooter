#ifndef _GAME_USERDATA_H_
#define _GAME_USERDATA_H_

#include "file.h"

struct UserData
{
    uint32_t singlePlayerHighscore;
    uint32_t multiPlayerHighscore;
    UserData() :
        singlePlayerHighscore{0},
        multiPlayerHighscore{0}
    {}
};

bool readUserData(UserData* data, const char* path);

bool writeUserData(const UserData& data, const char* path);

#endif
