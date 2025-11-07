#include "userdata.h"

bool readUserData(UserData* data, const char* path)
{
    std::ifstream fs{path};
    if (!fs.is_open())
        return false;
    data->singlePlayerHighscore = fileReadU32(fs);
    data->multiPlayerHighscore = fileReadU32(fs);
    return true;
}

bool writeUserData(const UserData& data, const char* path)
{
    std::ofstream fs{path};
    if (!fs.is_open())
        return false;
    fileWriteU32(fs, data.singlePlayerHighscore);
    fileWriteU32(fs, data.multiPlayerHighscore);
    return true;
}
