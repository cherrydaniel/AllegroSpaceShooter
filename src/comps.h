#ifndef _GAME_COMPS_H_
#define _GAME_COMPS_H_

#include <allegro5/allegro.h>
#include <cstdint>
#include <entt/entity/registry.hpp>
#include "util/common.h"
#include "util/maths.h"
#include "consts.h"

// entt doesn't play nice with empty structs as components
#define MARKER_COMP(name) struct name { char __marker_placeholder__; };

MARKER_COMP(ScreenBoundComp);

MARKER_COMP(DestroyOnScreenBottomComp);

MARKER_COMP(EnemyComp);

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
#define PLAYER_STATE_FIRING         BIT(4)

struct PlayerControlComp
{
    uint8_t playerId;
    uint8_t stateFlags;
    PlayerControlComp(uint8_t playerId)
        : playerId{playerId}, stateFlags{0} {}
};

struct PlayerComp
{
    double animationTime;
    ALLEGRO_BITMAP* bitmap;
};

struct HittableComp
{
    Faction side;
    HittableComp(Faction side) : side{side} {}
};

struct HealthComp
{
    uint32_t hp;
    HealthComp(uint32_t hp) : hp{hp} {}
};

struct ScoreComp
{
    uint32_t score;
    ScoreComp(uint32_t score=0) : score{score} {}
};

struct ScoreProviderComp
{
    uint32_t score;
    ScoreProviderComp(uint32_t score=0) : score{score} {}
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

struct PhysicalBoundTextureComp
{
    ALLEGRO_BITMAP* texture;
    FVec2 size;
    FVec2 offset;
    PhysicalBoundTextureComp(ALLEGRO_BITMAP* texture, FVec2 size, FVec2 offset)
        : texture{texture}, size{size}, offset{offset} {}
    PhysicalBoundTextureComp(ALLEGRO_BITMAP* texture, FVec2 size)
        : texture{texture}, size{size}, offset{0.0, 0.0} {}
    PhysicalBoundTextureComp(ALLEGRO_BITMAP* texture)
        : texture{texture}, size{0.0, 0.0}, offset{0.0, 0.0} {}
    PhysicalBoundTextureComp()
        : texture{nullptr}, size{0.0, 0.0}, offset{0.0, 0.0} {}
};

struct HitShaderComp
{
    double timeout;
    HitShaderComp() : timeout(0.0f) {}
};

#endif
