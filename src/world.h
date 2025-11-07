#ifndef _GAME_WORLD_H_
#define _GAME_WORLD_H_

#include <cmath>
#include <algorithm>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <entt/entity/registry.hpp>
#include "common.h"
#include "input.h"
#include "maths.h"
#include "rnd.h"
#include "observer.h"
#include "fixedstack.h"
#include "comps.h"
#include "assetloader.h"
#include "weapons.h"
#include "userdata.h"
#include "enemy.h"

#define ROAD_DIRECTION_MIN          4
#define ROAD_DIRECTION_MAX          8
#define ROAD_WIDTH                  400.0f
#define ROAD_PADDING                10.0f
#define ROAD_SECTION_HEIGHT         40.0f
#define ROAD_MOVE_SPEED             10.0f
#define PLAYER_MOVE_SPEED           300.0f
#define PLAYER_MOVE_FORCE           2.5f
#define PLAYER_WIDTH                40.0f
#define PLAYER_HEIGHT               40.0f

class World;

struct BulletHitEvent
{
    entt::entity bullet;
    entt::entity target;
};

class LevelSystem
{
private:
    RND rnd;
    entt::registry* registry;
    AssetMap* assets;
    EnemyFactory enemyFactory;
    IVec2 dim;
    double distance;
    double spawnGap;
    double nextSpawn;
    void spawnEnemy();
public:
    void init(entt::registry* registry, AssetMap* assets, const IVec2& dim);
    void reset();
    void update();
    double getDistance();
};

class PlayerSystem
{
private:
    entt::registry* registry;
public:
    void init(entt::registry* registry);
    void handleInput(InputEvent* ev);
    void update();
};

class AISystem
{
private:
    entt::registry* registry;
public:
};

class RoadSystem
{
private:
    RND rnd;
    entt::registry* registry;
    IVec2 dim;
    entt::entity lastSection;
    uint8_t directionChangeCountdown;
    void positionRoadSection(entt::entity entity);
public:
    RoadSystem() : lastSection{entt::null} {}
    void init(entt::registry* registry, const IVec2& dim);
    void update();
};

class PhysicsSystem
{
private:
    entt::registry* registry;
    IVec2 dim;
    Subject<struct BulletHitEvent>* bulletHitSubject;
    void markCollision(entt::entity entity);
    void resetCollisionMarkers();
    void resolveCollisionX(entt::entity entity, float x);
    void resolveCollisionY(entt::entity entity, float y);
    void resolveDeadOnScreenBottom();
    void resolveScreenBound();
    void resolveRoadBound();
    void resolvePlayerEnemy();
    void resolvePowerups();
    void resolveBullets();
public:
    void init(entt::registry* registry, const IVec2& dim,
        Subject<struct BulletHitEvent>* bulletHitSubject);
    void applyMovement();
    void checkCollisions();
};

class HitObserver : public Observer<struct BulletHitEvent>
{
private:
    entt::registry* registry;
public:
    void init(entt::registry* registry);
    void onEvent(struct BulletHitEvent event, void* data) override;
};

class WeaponSystem
{
private:
    entt::registry* registry;
    IVec2 dim;
    WeaponFactory factory;
    HitObserver hitObserver;
    Subject<struct BulletHitEvent>* bulletHitSubject;
    AssetMap* assets;
public:
    void init(entt::registry* registry, const IVec2& dim,
        Subject<struct BulletHitEvent>* bulletHitSubject,
        AssetMap* assets);
    void destroy();
    void update();
    void equip(entt::entity entity, WeaponType weaponType, Side targetSide);
    void unequip(entt::entity entity);
};

class Renderer
{
private:
    entt::registry* registry;
    AssetMap* assets;
    ALLEGRO_BITMAP* mainBitmap;
    ALLEGRO_BITMAP* background;
    ALLEGRO_BITMAP* playerShip;
    ALLEGRO_SHADER* redShader;
public:
    void init(entt::registry* registry, AssetMap* assets, ALLEGRO_BITMAP* mainBitmap);
    void draw(float interpolation, double delta);
};

class World
{
private:
    RND rnd;
    entt::registry registry;
    IVec2 dim;
    LevelSystem levelSystem;
    PlayerSystem playerSystem;
    RoadSystem roadSystem;
    PhysicsSystem physicsSystem;
    WeaponSystem weaponSystem;
    Renderer renderer;
    AssetMap* assets;
    Subject<struct BulletHitEvent> bulletHitSubject;
public:
    World(){}
    ~World(){}
    bool init(int w, int h, AssetMap* assets, ALLEGRO_BITMAP* mainBitmap);
    void onInput(InputEvent* ev);
    void tick(void);
    void draw(float interpolation, double delta);
    void destroy();
};

#endif
