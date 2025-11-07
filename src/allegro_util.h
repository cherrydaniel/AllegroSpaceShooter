#ifndef _GAME_AL_UTIL_H_
#define _GAME_AL_UTIL_H_

#include <memory>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "common.h"

struct BitmapDeleter
{
    void operator()(ALLEGRO_BITMAP* p) const{ PRINT_DEBUG("DESTROYING BITMAP"); al_destroy_bitmap(p); }
};

struct FontDeleter
{
    void operator()(ALLEGRO_FONT* p) const{ PRINT_DEBUG("DESTROYING FONT"); al_destroy_font(p); }
};

struct DisplayDeleter
{
    void operator()(ALLEGRO_DISPLAY* p) const{ PRINT_DEBUG("DESTROYING DISPLAY"); al_destroy_display(p); }
};

struct ShaderDeleter
{
    void operator()(ALLEGRO_SHADER* p) const{ PRINT_DEBUG("DESTROYING SHADER"); al_destroy_shader(p); }
};

using BitmapPointer = std::unique_ptr<ALLEGRO_BITMAP, BitmapDeleter>;
using FontPointer = std::unique_ptr<ALLEGRO_FONT, FontDeleter>;
using DisplayPointer = std::unique_ptr<ALLEGRO_DISPLAY, DisplayDeleter>;
using ShaderPointer = std::unique_ptr<ALLEGRO_SHADER, ShaderDeleter>;

#endif
