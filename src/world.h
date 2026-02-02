#ifndef _GAME_WORLD_H_
#define _GAME_WORLD_H_

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/bitmap.h>
#include <entt/entity/registry.hpp>
#include "util/allegro_util.h"
#include "util/maths.h"
#include "util/rnd.h"
#include "util/observer.h"
#include "input.h"
#include "assetloader.h"
#include "enemy.h"

class World;
class WeaponSystem;

struct BulletHitEvent
{
    entt::entity bullet;
    entt::entity target;
};

class LevelSystem
{
private:
    RND rnd;
    EnemySystem* enemySystem;
    entt::registry* registry;
    AssetMap* assets;
    IVec2 dim;
    double distance;
    double spawnGap;
    double nextSpawn;
    void spawnEnemy();
public:
    void init(EnemySystem* enemySystem, entt::registry* registry,
        AssetMap* assets, const IVec2& dim);
    void reset();
    void update();
    double getDistance();
};

class PlayerSystem
{
private:
    entt::registry* registry;
    WeaponSystem* weaponSystem;
public:
    void init(entt::registry* registry, WeaponSystem* weaponSystem);
    void handleInput(InputEvent* ev);
    void update();
    void destroy();
    void onPlayerDestroy(entt::registry& registry, const entt::entity entity);
};

class AISystem
{
private:
    entt::registry* registry;
public:
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
    void resolvePlayerEnemy();
    void resolvePowerups();
    void resolveBullets();
public:
    void init(
        entt::registry* registry,
        const IVec2& dim,
        Subject<struct BulletHitEvent>* bulletHitSubject
    );
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

class BackgroundSystem
{
private:
    IVec2 dim;
    LevelSystem* levelSystem;
    BitmapPointer bmp;
public:
    void init(
        const IVec2& dim,
        AssetMap* assets,
        LevelSystem* levelSystem,
        ALLEGRO_BITMAP* mainBitmap
    );
    void draw(float interpolation, double delta);
};

class WeaponSystem
{
private:
    entt::registry* registry;
    IVec2 dim;
    HitObserver hitObserver;
    Subject<struct BulletHitEvent>* bulletHitSubject;
    AssetMap* assets;
public:
    void init(
        entt::registry* registry,
        const IVec2& dim,
        Subject<struct BulletHitEvent>* bulletHitSubject,
        AssetMap* assets
    );
    void update();
    void destroy();
    void equipSingleBlaster(entt::entity entity);
    void equipDualBlaster(entt::entity entity);
    void equipBeam(entt::entity entity);
    void equipSpreadGun(entt::entity entity);
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
    void init(
        entt::registry* registry,
        AssetMap* assets,
        ALLEGRO_BITMAP* mainBitmap
    );
    void draw(float interpolation, double delta);
};

class World
{
private:
    RND rnd;
    entt::registry registry;
    IVec2 dim;
    BackgroundSystem backgroundSystem;
    EnemySystem enemySystem;
    LevelSystem levelSystem;
    PlayerSystem playerSystem;
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
