#ifndef _GAME_SETTINGS_H_
#define _GAME_SETTINGS_H_

#include "file.h"

struct Settings
{
    bool soundOn = false;
    
};

bool readSettings(Settings* settings, const char* path);

bool writeSettings(const Settings& settings, const char* path);

#endif
