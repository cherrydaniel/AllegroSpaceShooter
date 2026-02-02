#ifndef _GAME_ENEMY_H_
#define _GAME_ENEMY_H_

#include <entt/entity/registry.hpp>
#include "assetloader.h"

class EnemySystem
{
private:
    entt::registry* registry;
    AssetMap* assets;
public:
    EnemySystem() {}
    ~EnemySystem() {}
    void init(entt::registry* registry, AssetMap* assets);
    void spawnSquidEnemy(float x, float y);
};

#endif
