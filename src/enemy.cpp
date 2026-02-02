#include "comps.h"
#include "consts.h"
#include "util/maths.h"
#include "enemy.h"

void EnemySystem::init(entt::registry* registry, AssetMap* assets)
{
    this->registry = registry;
    this->assets = assets;
}

void EnemySystem::spawnSquidEnemy(float x, float y)
{
    auto entity = registry->create();
    registry->emplace<PhysicalBoundTextureComp>(entity,
        assets->getBitmap("enemy_squid"),
        FVec2{64.0f, 64.0f},
        FVec2{-8.0f, -8.0f});
    registry->emplace<DestroyOnScreenBottomComp>(entity);
    registry->emplace<VelocityComp>(entity, 0, 5.0f);
    registry->emplace<HittableComp>(entity, Faction::ENEMY);
    registry->emplace<PositionComp>(entity, x, y);
    registry->emplace<PhysicalComp>(entity, x, y, 48.0f, 48.0f);
    registry->emplace<EnemyComp>(entity);
    registry->emplace<HealthComp>(entity, 400);
    registry->emplace<ScoreProviderComp>(entity, 100);
    registry->emplace<HitShaderComp>(entity);
}
