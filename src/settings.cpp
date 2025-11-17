#include "settings.h"

bool readSettings(Settings* settings, const char* path)
{
    FileReader fr{path};
    if (!fr.isOpen())
        return false;
    settings->soundOn = fr.readU8();
    return true;
}

bool writeSettings(const Settings& settings, const char* path)
{
    FileWriter fw{path};
    if (!fw.isOpen())
        return false;
    fw.writeU8(settings.soundOn ? 1 : 0);
    return true;
}
