#ifndef _GAME_WEAPONS_H_
#define _GAME_WEAPONS_H_

#include <algorithm>
#include <allegro5/allegro.h>
#include <entt/entity/registry.hpp>
#include "util/common.h"
#include "util/maths.h"
#include "input.h"
#include "comps.h"
#include "assetloader.h"

enum BulletType {SINGLE=0, DUAL, BEAM, SPREAD};

struct WeaponSpecComp
{

};

struct PlayerControlledWeaponComp
{
    
};

struct EnemyControlledWeaponComp
{

};

struct WeaponShooterComp
{
    uint32_t shooterId;
};

enum WeaponType {NONE=0, BASIC, DOUBLE_BLASTER};

class Weapon
{
protected:
    entt::registry* registry;
    entt::entity entity;
    Side targetSide;
    AssetMap* assets;
public:
    Weapon(
        entt::registry* registry,
        entt::entity entity,
        Side targetSide,
        AssetMap* assets
    ) : registry{registry},
        entity{entity},
        targetSide{targetSide},
        assets{assets}
    {}
    virtual ~Weapon(){}
    virtual void init(){}
    virtual void destroy(){}
    virtual void handleInput(InputEvent* ev){}
    virtual void update(){}
    virtual void draw(float interpolation, double delta, AssetMap* assets, const FRect& shooter){}
    virtual WeaponType type(){ return WeaponType::NONE; }
};

class BasicWeapon : public Weapon
{
private:
    static constexpr uint32_t STEPS_PER_BULLET = 10;
    static constexpr float BULLET_WIDTH = 4.0f;
    static constexpr float BULLET_HEIGHT = 10.0f;
    static constexpr float BULLET_SPEED = 50.0f;
    static constexpr uint32_t BULLET_DAMAGE = 100;
    static constexpr float TURRET_OFFSET = 4.0f;
    static constexpr float TURRET_OFFSET_MOVE = 1.0f;
    bool firing;
    uint32_t steps;
    float turretOffset;
    void fireBullet();
public:
    BasicWeapon(
        entt::registry* registry,
        entt::entity entity,
        Side targetSide,
        AssetMap* assets
    ) : Weapon(registry, entity, targetSide, assets),
        firing{false},
        steps{0},
        turretOffset{0.0f}
    {}
    ~BasicWeapon(){}
    void init() override;
    void handleInput(InputEvent* ev) override;
    void update() override;
    void draw(float interpolation, double delta, AssetMap* assets, const FRect& shooter) override;
    WeaponType type() override{ return WeaponType::BASIC; }
};

class DoubleBlaster : public Weapon
{
private:
    static constexpr uint32_t STEPS_PER_BULLET = 4;
    static constexpr float BULLET_WIDTH = 2.0f;
    static constexpr float BULLET_HEIGHT = 20.0f;
    static constexpr float BULLET_SPEED = 40.0f;
    static constexpr float BULLET_GAP = 7.0f;
    static constexpr uint32_t BULLET_DAMAGE = 50;
    static constexpr float TURRET_OFFSET = 4.0f;
    static constexpr float TURRET_OFFSET_MOVE = 1.0f;
    bool firing;
    uint32_t steps;
    float turretOffset;
    void fireBullet();
public:
    DoubleBlaster(
        entt::registry* registry,
        entt::entity entity,
        Side targetSide,
        AssetMap* assets
    ) : Weapon(registry, entity, targetSide, assets),
        firing{false},
        steps{0},
        turretOffset{0.0f}
    {}
    ~DoubleBlaster(){}
    void init() override;
    void handleInput(InputEvent* ev) override;
    void update() override;
    void draw(float interpolation, double delta, AssetMap* assets, const FRect& shooter) override;
    WeaponType type() override{ return WeaponType::DOUBLE_BLASTER; }
};

class WeaponFactory
{
private:
    entt::registry* registry;
    AssetMap* assets;
public:
    void init(entt::registry* registry, AssetMap* assets);
    Weapon* create(entt::entity entity, WeaponType type, Side targetSide);
};

struct WeaponComp
{
    static void on_destroy(entt::registry& registry, const entt::entity entity)
    {
        auto& comp = registry.get<WeaponComp>(entity);
        delete comp.weapon;
        comp.weapon = nullptr;
    }
    Weapon* weapon;
    WeaponComp() : weapon{nullptr} {}
};

#endif
