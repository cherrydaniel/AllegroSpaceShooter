#include "weapons.h"

void BasicWeapon::init()
{
    // TODO: handle assets
}

void BasicWeapon::fireBullet()
{
    const auto& physComp = registry->get<const PhysicalComp>(entity);
    auto bullet = registry->create();
    auto& bulletComp = registry->emplace<BulletComp>(bullet);
    bulletComp.targetSide = targetSide;
    bulletComp.shooter = entity;
    bulletComp.damage = BULLET_DAMAGE;
    float speed = BULLET_SPEED;
    if (targetSide==Faction::ENEMY)
        speed*=-1;
    registry->emplace<VelocityComp>(bullet, 0, speed);
    float x = physComp.rect.x+(physComp.rect.w/2)-(BULLET_WIDTH/2);
    float y = physComp.rect.y-BULLET_HEIGHT-speed;
    if (targetSide==Faction::PLAYER)
        y+=physComp.rect.h-BULLET_HEIGHT;
    registry->emplace<PositionComp>(bullet, x, y);
    registry->emplace<PhysicalComp>(bullet, x, y, BULLET_WIDTH, BULLET_HEIGHT);
    turretOffset = TURRET_OFFSET;
}

void BasicWeapon::handleInput(InputEvent* ev)
{
    switch (ev->action)
    {
        case ATTACK_START:
            firing = true;
            break;
        case ATTACK_END:
            firing = false;
            steps = 0;
            break;
    }
}

void BasicWeapon::update()
{
    turretOffset = std::max(turretOffset-TURRET_OFFSET_MOVE, 0.0f);
    if (!firing)
        return;
    if (!steps)
        fireBullet();
    steps++;
    if (steps>=STEPS_PER_BULLET)
        steps = 0;
}

void BasicWeapon::draw(
    float interpolation, double delta, AssetMap* assets, const FRect& shooter)
{
    // float offset = std::lerp(turretOffset,
    //     std::max(turretOffset-TURRET_OFFSET_MOVE, 0.0f),
    //     interpolation);
    // ALLEGRO_BITMAP* wpn = assets->getBitmap("turret_01");
    // int bw = al_get_bitmap_width(wpn);
    // int bh = al_get_bitmap_height(wpn);
    // al_draw_scaled_bitmap(wpn, 0, 0, bw, bh,
    //     shooter.x+shooter.w/2-16, shooter.y+shooter.h/2-16+offset, 32, 32, 0);
}

void DoubleBlaster::init()
{
    // TODO: handle assets
}

void DoubleBlaster::fireBullet()
{
    const auto& physComp = registry->get<const PhysicalComp>(entity);
    auto bulletA = registry->create();
    auto bulletB = registry->create();
    auto& bulletCompA = registry->emplace<BulletComp>(bulletA);
    bulletCompA.targetSide = targetSide;
    bulletCompA.shooter = entity;
    bulletCompA.damage = BULLET_DAMAGE;
    auto& bulletCompB = registry->emplace<BulletComp>(bulletB);
    bulletCompB.targetSide = targetSide;
    bulletCompB.shooter = entity;
    bulletCompB.damage = BULLET_DAMAGE;
    float speed = BULLET_SPEED;
    if (targetSide==Faction::ENEMY)
        speed*=-1;
    registry->emplace<VelocityComp>(bulletA, 0, speed);
    registry->emplace<VelocityComp>(bulletB, 0, speed);
    float x = physComp.rect.x+(physComp.rect.w/2)-(BULLET_WIDTH/2);
    float y = physComp.rect.y+(physComp.rect.h*0.13)-speed;
    if (targetSide==Faction::PLAYER)
        y = physComp.rect.h-BULLET_HEIGHT-y;
    float gap = BULLET_GAP/2;
    registry->emplace<PositionComp>(bulletA, x-gap, y);
    registry->emplace<PositionComp>(bulletB, x+gap, y);
    registry->emplace<PhysicalComp>(bulletA, x-gap, y, BULLET_WIDTH, BULLET_HEIGHT);
    registry->emplace<PhysicalComp>(bulletB, x+gap, y, BULLET_WIDTH, BULLET_HEIGHT);
    turretOffset = TURRET_OFFSET;
}

void DoubleBlaster::handleInput(InputEvent* ev)
{
    switch (ev->action)
    {
        case ATTACK_START:
            firing = true;
            break;
        case ATTACK_END:
            firing = false;
            steps = 0;
            break;
    }
}

void DoubleBlaster::update()
{
    turretOffset = std::max(turretOffset-TURRET_OFFSET_MOVE, 0.0f);
    if (!firing)
        return;
    if (!steps)
        fireBullet();
    steps++;
    if (steps>=STEPS_PER_BULLET)
        steps = 0;
}

void DoubleBlaster::draw(
    float interpolation, double delta, AssetMap* assets, const FRect& shooter)
{
    // float offset = std::lerp(turretOffset,
    //     std::max(turretOffset-TURRET_OFFSET_MOVE, 0.0f),
    //     interpolation);
    // ALLEGRO_BITMAP* wpn = assets->getBitmap("turret_02");
    // int bw = al_get_bitmap_width(wpn);
    // int bh = al_get_bitmap_height(wpn);
    // al_draw_scaled_bitmap(wpn, 0, 0, bw, bh,
    //     shooter.x+shooter.w/2-16, shooter.y+shooter.h/2-16+offset, 32, 32, 0);
}

void WeaponFactory::init(entt::registry* registry, AssetMap* assets)
{
    this->registry = registry;
    this->assets = assets;
}

Weapon* WeaponFactory::create(entt::entity entity, WeaponType type, Faction targetSide)
{
    switch (type)
    {
        case WeaponType::BASIC:
            return new BasicWeapon(registry, entity, targetSide, assets);
        case WeaponType::DOUBLE_BLASTER:
            return new DoubleBlaster(registry, entity, targetSide, assets);
    }
    return nullptr;
}
