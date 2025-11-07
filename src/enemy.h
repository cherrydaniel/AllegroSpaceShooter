#ifndef _GAME_ENEMY_H_
#define _GAME_ENEMY_H_

#include <allegro5/allegro.h>
#include <entt/entity/registry.hpp>
#include "common.h"
#include "input.h"
#include "comps.h"
#include "assetloader.h"
#include "maths.h"

#define ENEMY_PADDING 20.0f

enum EnemyType {ENEMY_TYPE_NONE=0, SQUID};

class Enemy
{
protected:
    entt::registry* registry;
    entt::entity entity;
    AssetMap* assets;
public:
    Enemy(
        entt::registry* registry,
        entt::entity entity,
        AssetMap* assets
    ) : registry{registry},
        entity{entity},
        assets{assets}
    {}
    virtual ~Enemy(){}
    virtual void init(float x, float y){}
    virtual void destroy(){}
    virtual void update(){}
    virtual void draw(float interpolation, double delta, const FRect& rect){}
    virtual EnemyType type(){ return EnemyType::ENEMY_TYPE_NONE; }
};

class SquidEnemy : public Enemy
{
private:
    ALLEGRO_BITMAP* texture;
public:
    static constexpr float SIZE = 40.0f;
    static constexpr float SPEED = 20.0f;
    static constexpr uint32_t SCORE = 100;
    static constexpr uint32_t HEALTH = 100;
    SquidEnemy(
        entt::registry* registry,
        entt::entity entity,
        AssetMap* assets
    ) : Enemy(registry, entity, assets) {}
    void init(float x, float y) override;
    void update() override;
    void draw(float interpolation, double delta, const FRect& rect) override;
};

class EnemyFactory
{
private:
    entt::registry* registry;
    AssetMap* assets;
public:
    void init(entt::registry* registry, AssetMap* assets);
    Enemy* create(entt::entity entity, EnemyType type);
};

struct EnemyComp
{
    static void on_destroy(entt::registry& registry, const entt::entity entity)
    {
        auto& comp = registry.get<EnemyComp>(entity);
        if (comp.enemy!=nullptr)
        {
            comp.enemy->destroy();
            delete comp.enemy;
            comp.enemy = nullptr;
        }
    }
    Enemy* enemy;
    EnemyComp() : enemy{nullptr} {}
};

#endif
