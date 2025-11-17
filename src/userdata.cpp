#include "userdata.h"

bool readUserData(UserData* data, const char* path)
{
    FileReader fr{path};
    if (!fr.isOpen())
        return false;
    data->singlePlayerHighscore = fr.readU32();
    data->multiPlayerHighscore = fr.readU32();
    return true;
}

bool writeUserData(const UserData& data, const char* path)
{
    std::ofstream fs{path};
    FileWriter fw{path};
    if (!fw.isOpen())
        return false;
    fw.writeU32(data.singlePlayerHighscore);
    fw.writeU32(data.multiPlayerHighscore);
    return true;
}
