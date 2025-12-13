#ifndef _GAME_WEAPONS_H_
#define _GAME_WEAPONS_H_

#define BLASTER_BULLET_WIDTH 4
#define BLASTER_BULLET_HEIGHT 10
#define BLASTER_DUAL_GAP 10.0f

enum BulletType {SINGLE=0, DUAL, BEAM, SPREAD};

struct WeaponSpecComp
{
    BulletType bulletType;
    uint32_t bulletDamage;
    uint32_t bulletSpeed;
    uint32_t stepsPerBullet;
    uint32_t steps;
};

enum WeaponType {NONE=0, BASIC, DOUBLE_BLASTER};

#endif
