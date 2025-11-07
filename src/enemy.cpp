#include "enemy.h"

void SquidEnemy::init(float x, float y)
{
    texture = assets->getBitmap("enemy_squid");
    registry->emplace<DestroyOnScreenBottomComp>(entity);
    registry->emplace<VelocityComp>(entity, 0, SPEED);
    auto& hittableComp = registry->emplace<HittableComp>(entity);
    hittableComp.side = Side::ENEMY;
    registry->emplace<PositionComp>(entity, x, y);
    registry->emplace<PhysicalComp>(entity, x, y, SIZE, SIZE);
    auto& healthComp = registry->emplace<HealthComp>(entity);
    healthComp.hp = HEALTH;
    auto& scoreComp = registry->emplace<ScoreProviderComp>(entity);
    scoreComp.score = SCORE;
}

void SquidEnemy::update()
{

}

void SquidEnemy::draw(float interpolation, double delta, const FRect& rect)
{
    
}

void EnemyFactory::init(entt::registry* registry, AssetMap* assets)
{
    this->registry = registry;
    this->assets = assets;
}

Enemy* EnemyFactory::create(entt::entity entity, EnemyType type)
{
    switch (type)
    {
        case EnemyType::SQUID:
            return new SquidEnemy(registry, entity, assets);
    }
    return nullptr;
}
