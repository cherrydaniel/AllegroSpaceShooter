#include "settings.h"

bool readSettings(Settings* settings, const char* path)
{
    std::ifstream fs{path};
    if (!fs.is_open())
        return false;
    settings->soundOn = fileReadU8(fs);
    return true;
}

bool writeSettings(const Settings& settings, const char* path)
{
    std::ofstream fs{path};
    if (!fs.is_open())
        return false;
    fileWriteU8(fs, settings.soundOn ? 1 : 0);
    return true;
}
