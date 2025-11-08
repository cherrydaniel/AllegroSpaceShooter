#ifndef _GAME_COMPS_H_
#define _GAME_COMPS_H_

#include <entt/entity/registry.hpp>
#include "util/common.h"
#include "util/maths.h"
#include "consts.h"

// --- MARKER COMPS

struct ScreenBoundComp {};

struct DestroyOnScreenBottomComp {};

// --- DATA COMPS

struct PositionComp
{
    FVec2 pos;
    PositionComp()
    {
        pos.x = 0;
        pos.y = 0;
    }
    PositionComp(float x, float y)
    {
        pos.x = x;
        pos.y = y;
    }
};

struct VelocityComp
{
    FVec2 vel;
    VelocityComp()
    {
        vel.x = 0;
        vel.y = 0;
    }
    VelocityComp(float x, float y)
    {
        vel.x = x;
        vel.y = y;
    }
};

struct PhysicalComp
{
    FRect rect;
    PhysicalComp() = default;
    PhysicalComp(float x, float y, float w, float h)
    {
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;
    }
};

#define PLAYER_STATE_MOVING_LEFT    BIT(0)
#define PLAYER_STATE_MOVING_RIGHT   BIT(1)
#define PLAYER_STATE_MOVING_UP      BIT(2)
#define PLAYER_STATE_MOVING_DOWN    BIT(3)

struct PlayerControlComp
{
    uint8_t playerId;
    uint8_t stateFlags;
    PlayerControlComp(uint8_t playerId)
        : playerId{playerId}, stateFlags{0} {}
};

struct PlayerComp
{
    static void on_destroy(entt::registry& registry, const entt::entity entity)
    {
        auto& comp = registry.get<PlayerComp>(entity);
        al_destroy_bitmap(comp.bitmap);
    }
    double animationTime;
    ALLEGRO_BITMAP* bitmap;
};

struct HittableComp
{
    Faction side;
};

struct HealthComp
{
    uint32_t hp;
};

struct ScoreComp
{
    uint32_t score;
    ScoreComp(uint32_t score=0) : score{score} {}
};

struct ScoreProviderComp
{
    uint32_t score;
};

struct CollisionMarkerComp
{
    bool mark;
};

struct BulletComp
{
    entt::entity shooter;
    Faction targetSide;
    uint32_t damage;
};

#endif
