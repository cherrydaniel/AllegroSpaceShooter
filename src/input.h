#ifndef _GAME_INPUT_H_
#define _GAME_INPUT_H_

#include <allegro5/allegro.h>

enum InputAction
{
    ATTACK_START,
    ATTACK_END,
    MOVE_LEFT_START,
    MOVE_LEFT_END,
    MOVE_RIGHT_START,
    MOVE_RIGHT_END,
    MOVE_UP_START,
    MOVE_UP_END,
    MOVE_DOWN_START,
    MOVE_DOWN_END
};

// TODO: add union with specific props (joystick axis)
struct InputEvent
{
    uint8_t playerId;
    InputAction action;
};

bool mapAction(ALLEGRO_EVENT* ev, InputEvent* dst);

#endif
