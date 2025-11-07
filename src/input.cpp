#include "input.h"
#include "common.h"

bool mapAction(ALLEGRO_EVENT* ev, InputEvent* dst)
{
    dst->playerId = 1; // TODO map to player
    if (ev->type==ALLEGRO_EVENT_KEY_DOWN||ev->type==ALLEGRO_EVENT_KEY_UP)
    {
        switch (ev->keyboard.keycode)
        {
            case ALLEGRO_KEY_SPACE:
                dst->action = ev->type==ALLEGRO_EVENT_KEY_DOWN ? ATTACK_START : ATTACK_END;
                return true;
            case ALLEGRO_KEY_LEFT:
                dst->action = ev->type==ALLEGRO_EVENT_KEY_DOWN ? MOVE_LEFT_START : MOVE_LEFT_END;
                return true;
            case ALLEGRO_KEY_RIGHT:
                dst->action = ev->type==ALLEGRO_EVENT_KEY_DOWN ? MOVE_RIGHT_START : MOVE_RIGHT_END;
                return true;
            case ALLEGRO_KEY_DOWN:
                dst->action = ev->type==ALLEGRO_EVENT_KEY_DOWN ? MOVE_DOWN_START : MOVE_DOWN_END;
                return true;
            case ALLEGRO_KEY_UP:
                dst->action = ev->type==ALLEGRO_EVENT_KEY_DOWN ? MOVE_UP_START : MOVE_UP_END;
                return true;
        }
    }
    if (ev->type==ALLEGRO_EVENT_JOYSTICK_AXIS)
    {
        PRINT_DEBUG("stick: %d, axis: %d, pos: %f",
            ev->joystick.stick, ev->joystick.axis, ev->joystick.pos);
    }
    if (ev->type==ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN)
    {
        PRINT_DEBUG("button down: %d", ev->joystick.button);
    }
    if (ev->type==ALLEGRO_EVENT_JOYSTICK_BUTTON_UP)
    {
        PRINT_DEBUG("button up: %d", ev->joystick.button);
    }
    return false;
}
