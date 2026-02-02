#include <algorithm>
#include <allegro5/bitmap.h>
#include <allegro5/bitmap_draw.h>
#include <allegro5/color.h>
#include <allegro5/shader.h>
#include <cmath>
#include <cstdio>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <entt/entity/registry.hpp>
#include "comps.h"
#include "world.h"
#include "game.h"
#include "input.h"
#include "comps.h"
#include "assetloader.h"
#include "weapons.h"
#include "enemy.h"
#include "consts.h"
#include "util/duration.h"
#include "util/common.h"
#include "util/maths.h"
#include "util/observer.h"

void LevelSystem::spawnEnemy()
{
    float x = rnd.range(40, dim.h-40);
    enemySystem->spawnSquidEnemy(x, -64.0f);
}

void LevelSystem::init(EnemySystem* enemySystem,
    entt::registry* registry, AssetMap* assets, const IVec2& dim)
{
    this->enemySystem = enemySystem;
    this->registry = registry;
    this->assets = assets;
    this->dim = dim;
    reset();
}

void LevelSystem::reset()
{
    distance = 0.0f;
    spawnGap = 500.0f;
    nextSpawn = 500.0f;
}

void LevelSystem::update()
{
    distance+=WORLD_MOVE_SPEED;
    if (distance>=nextSpawn)
    {
        spawnEnemy();
        nextSpawn+=spawnGap;
    }
}

double LevelSystem::getDistance()
{
    return distance;
}

void PlayerSystem::init(entt::registry* registry, WeaponSystem* weaponSystem)
{
    this->registry = registry;
    this->weaponSystem = weaponSystem;
    registry->on_destroy<PlayerComp>()
        .connect<&PlayerSystem::onPlayerDestroy>(this);
    auto player = registry->create();
    registry->emplace<PlayerControlComp>(player, 1);
    registry->emplace<VelocityComp>(player);
    registry->emplace<PositionComp>(player,
        VIEWPORT_WIDTH/2-PLAYER_WIDTH/2, VIEWPORT_HEIGHT/2-PLAYER_HEIGHT/2);
    registry->emplace<PhysicalComp>(player,
        0.0f, 0.0f, PLAYER_WIDTH, PLAYER_HEIGHT);
    registry->emplace<ScreenBoundComp>(player);
    auto& playerComp = registry->emplace<PlayerComp>(player);
    playerComp.bitmap = al_create_bitmap(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    registry->emplace<ScoreComp>(player);
    registry->emplace<CollisionMarkerComp>(player);
    weaponSystem->equipSingleBlaster(player);
}

void PlayerSystem::handleInput(InputEvent* ev)
{
    auto view = registry->view<PlayerControlComp>();
    for (auto entity : view)
    {
        auto& ctlComp = view.get<PlayerControlComp>(entity);
        if (ev->playerId!=ctlComp.playerId)
            continue;
        switch (ev->action)
        {
            case MOVE_LEFT_START:
                ctlComp.stateFlags|=PLAYER_STATE_MOVING_LEFT;
                break;
            case MOVE_LEFT_END:
                ctlComp.stateFlags&=~PLAYER_STATE_MOVING_LEFT;
                break;
            case MOVE_RIGHT_START:
                ctlComp.stateFlags|=PLAYER_STATE_MOVING_RIGHT;
                break;
            case MOVE_RIGHT_END:
                ctlComp.stateFlags&=~PLAYER_STATE_MOVING_RIGHT;
                break;
            case MOVE_UP_START:
                ctlComp.stateFlags|=PLAYER_STATE_MOVING_UP;
                break;
            case MOVE_UP_END:
                ctlComp.stateFlags&=~PLAYER_STATE_MOVING_UP;
                break;
            case MOVE_DOWN_START:
                ctlComp.stateFlags|=PLAYER_STATE_MOVING_DOWN;
                break;
            case MOVE_DOWN_END:
                ctlComp.stateFlags&=~PLAYER_STATE_MOVING_DOWN;
                break;
            case ATTACK_START: {
                ctlComp.stateFlags|=PLAYER_STATE_FIRING;
                auto weaponComp = registry->try_get<WeaponSpecComp>(entity);
                if (weaponComp)
                    weaponComp->steps = 0;
                break;
            }
            case ATTACK_END:
                ctlComp.stateFlags&=~PLAYER_STATE_FIRING;
                break;
        }
    }
};

void PlayerSystem::update()
{
    auto view = registry->view<const PlayerControlComp, VelocityComp>();
    for (auto entity : view)
    {
        const auto& ctlComp = view.get<PlayerControlComp>(entity);
        auto& velComp = view.get<VelocityComp>(entity);
        if (ctlComp.stateFlags&PLAYER_STATE_MOVING_LEFT)
        {
            velComp.vel.x = std::max(
                -PLAYER_MOVE_SPEED*STEP_TIME, velComp.vel.x-PLAYER_MOVE_FORCE);
        }
        else if (ctlComp.stateFlags&PLAYER_STATE_MOVING_RIGHT)
        {
            velComp.vel.x = std::min(
                PLAYER_MOVE_SPEED*STEP_TIME, velComp.vel.x+PLAYER_MOVE_FORCE);
        }
        else
        {
            if (std::abs(velComp.vel.x)<0.05f)
                velComp.vel.x = 0.0f;
            else if (velComp.vel.x>0)
                    velComp.vel.x = std::max(0.0f, velComp.vel.x-PLAYER_MOVE_FORCE);
            else
                velComp.vel.x = std::min(0.0f, velComp.vel.x+PLAYER_MOVE_FORCE);
        }
        if (ctlComp.stateFlags&PLAYER_STATE_MOVING_UP)
        {
            velComp.vel.y = std::max(
                -PLAYER_MOVE_SPEED*STEP_TIME, velComp.vel.y-PLAYER_MOVE_FORCE);
        }
        else if (ctlComp.stateFlags&PLAYER_STATE_MOVING_DOWN)
        {
            velComp.vel.y = std::min(
                PLAYER_MOVE_SPEED*STEP_TIME, velComp.vel.y+PLAYER_MOVE_FORCE);
        }
        else
        {
            if (std::abs(velComp.vel.y)<0.05f)
                velComp.vel.y = 0.0f;
            else if (velComp.vel.y>0)
                velComp.vel.y = std::max(0.0f, velComp.vel.y-PLAYER_MOVE_FORCE);
            else
                velComp.vel.y = std::min(0.0f, velComp.vel.y+PLAYER_MOVE_FORCE);
        }
    }
}

void PlayerSystem::destroy()
{
    registry->on_destroy<PlayerComp>()
        .disconnect<&PlayerSystem::onPlayerDestroy>(this);
}

void PlayerSystem::onPlayerDestroy(entt::registry &registry, const entt::entity entity)
{
    auto& comp = registry.get<PlayerComp>(entity);
    al_destroy_bitmap(comp.bitmap);
}

void PhysicsSystem::markCollision(entt::entity entity)
{
    auto colMarkComp = registry->try_get<CollisionMarkerComp>(entity);
    if (colMarkComp)
        colMarkComp->mark = true;
}

void PhysicsSystem::resetCollisionMarkers()
{
    auto view = registry->view<CollisionMarkerComp>();
    for (auto entity : view)
    {
        auto& colMarkComp = view.get<CollisionMarkerComp>(entity);
        colMarkComp.mark = false;
    }
}

void PhysicsSystem::resolveCollisionX(entt::entity entity, float x)
{
    {
        auto physComp = registry->try_get<PhysicalComp>(entity);
        if (physComp)
            physComp->rect.x = x;
    }
    {
        auto posComp = registry->try_get<PositionComp>(entity);
        if (posComp)
            posComp->pos.x = x;
    }
    {
        auto velComp = registry->try_get<VelocityComp>(entity);
        if (velComp)
            velComp->vel.x = 0.0f;
    }
}

void PhysicsSystem::resolveCollisionY(entt::entity entity, float y)
{
    {
        auto physComp = registry->try_get<PhysicalComp>(entity);
        if (physComp)
            physComp->rect.y = y;
    }
    {
        auto posComp = registry->try_get<PositionComp>(entity);
        if (posComp)
            posComp->pos.y = y;
    }
    {
        auto velComp = registry->try_get<VelocityComp>(entity);
        if (velComp)
            velComp->vel.y = 0.0f;
    }
}

void PhysicsSystem::resolveDeadOnScreenBottom()
{
    auto view = registry->view<const PhysicalComp, const DestroyOnScreenBottomComp>();
    for (auto entity : view)
    {
        const auto physComp = view.get<const PhysicalComp>(entity);
        if (physComp.rect.y>dim.h)
            registry->destroy(entity);
    }
}

void PhysicsSystem::resolveScreenBound()
{
    auto view = registry->view<const PhysicalComp, const ScreenBoundComp>();
    for (auto entity : view)
    {
        const auto physComp = view.get<const PhysicalComp>(entity);
        if (physComp.rect.x>dim.w-physComp.rect.w)
        {
            resolveCollisionX(entity, dim.w-physComp.rect.w);
            markCollision(entity);
        }
        else if (physComp.rect.x<0)
        {
            resolveCollisionX(entity, 0.0f);
            markCollision(entity);
        }
        if (physComp.rect.y>dim.h-physComp.rect.h)
        {
            resolveCollisionY(entity, dim.h-physComp.rect.h);
            markCollision(entity);
        }
        else if (physComp.rect.y<0)
        {
            resolveCollisionY(entity, 0.0f);
            markCollision(entity);
        }
    }
}

void PhysicsSystem::resolvePlayerEnemy()
{
    // TODO
}

void PhysicsSystem::resolvePowerups()
{
    // TODO
}

void PhysicsSystem::resolveBullets()
{
    auto view = registry->view<const BulletComp, const PhysicalComp>();
    for (auto bullet : view)
    {
        const auto& bulletComp = view.get<BulletComp>(bullet);
        const auto& physComp = view.get<PhysicalComp>(bullet);
        auto hittableView = registry->view<const HittableComp, const PhysicalComp>();
        for (auto hittableEntity : hittableView)
        {
            const auto& hittableComp = hittableView.get<HittableComp>(hittableEntity);
            if (hittableComp.side!=bulletComp.targetSide)
                continue;
            const auto& hittablePhysComp = hittableView.get<PhysicalComp>(hittableEntity);
            if (!intersection(physComp.rect, hittablePhysComp.rect))
                continue;
            bulletHitSubject->notify({bullet, hittableEntity});
        }
        if (!registry->valid(bullet))
            continue;
        if (physComp.rect.x<-physComp.rect.w||
            physComp.rect.x>dim.w||
            physComp.rect.y<-physComp.rect.h||
            physComp.rect.y>dim.h)
        {
            registry->destroy(bullet);
        }
    }
}

void PhysicsSystem::init(entt::registry* registry, const IVec2& dim,
    Subject<struct BulletHitEvent>* bulletHitSubject)
{
    this->registry = registry;
    this->dim = dim;
    this->bulletHitSubject = bulletHitSubject;
}

void PhysicsSystem::applyMovement()
{
    auto view = registry->view<const VelocityComp, PositionComp>();
    for (auto entity : view)
    {
        const auto& velComp = view.get<VelocityComp>(entity);
        auto& posComp = view.get<PositionComp>(entity);
        posComp.pos+=velComp.vel;
        auto physCompPtr = registry->try_get<PhysicalComp>(entity);
        if (physCompPtr)
        {
            physCompPtr->rect.x = posComp.pos.x;
            physCompPtr->rect.y = posComp.pos.y;
        }
    }
}

void PhysicsSystem::checkCollisions()
{
    resetCollisionMarkers();
    resolveDeadOnScreenBottom();
    resolveScreenBound();
    resolvePlayerEnemy();
    resolvePowerups();
    resolveBullets();
}

void HitObserver::init(entt::registry* registry)
{
    this->registry = registry;
}

void HitObserver::onEvent(struct BulletHitEvent event, void* data)
{
    const auto& bulletComp = registry->get<const BulletComp>(event.bullet);
    PRINT_DEBUG("BOOM! damage: %d", bulletComp.damage);
    auto healthComp = registry->try_get<HealthComp>(event.target);
    auto hitShaderComp = registry->try_get<HitShaderComp>(event.target);
    if (hitShaderComp)
        hitShaderComp->timeout = 0.1f;
    if (healthComp)
    {
        if (bulletComp.damage>=healthComp->hp)
        {
            PRINT_DEBUG("DED");
            auto enemyComp = registry->try_get<EnemyComp>(event.target);
            auto scoreComp = registry->try_get<ScoreComp>(bulletComp.shooter);
            if (enemyComp&&scoreComp)
            {
                auto scoreProviderComp = registry->try_get<ScoreProviderComp>(
                    event.target);
                if (scoreProviderComp)
                    scoreComp->score+=scoreProviderComp->score;
            }
            registry->destroy(event.target);
        }
        else
        {
            healthComp->hp-=bulletComp.damage;
        }
    }
    registry->destroy(event.bullet);
}

void WeaponSystem::init(entt::registry* registry, const IVec2& dim,
    Subject<struct BulletHitEvent>* bulletHitSubject, AssetMap* assets)
{
    this->registry = registry;
    this->dim = dim;
    this->bulletHitSubject = bulletHitSubject;
    this->assets = assets;
    hitObserver.init(registry);
    bulletHitSubject->addObserver(
        static_cast<Observer<struct BulletHitEvent>*>(&hitObserver));
}

void WeaponSystem::update()
{
    auto view = registry->view<WeaponSpecComp, const PhysicalComp>();
    for (auto entity : view)
    {
        auto& weaponComp = view.get<WeaponSpecComp>(entity);
        auto playerCtlComp = registry->try_get<PlayerControlComp>(entity);
        auto enemyComp = registry->try_get<EnemyComp>(entity);
        bool firing = false;
        Faction faction = Faction::PLAYER;
        Faction targetSide = Faction::ENEMY;
        if (playerCtlComp)
        {
            if (playerCtlComp->stateFlags&PLAYER_STATE_FIRING)
                firing = true;
        }
        else if (enemyComp)
        {
            faction = Faction::ENEMY;
            targetSide = Faction::PLAYER;
            // TODO: handle enemy firing state
        }
        if (!firing)
            return;
        if (!weaponComp.steps)
        {
            // FIRE BULLET [START]
            auto bullet = registry->create();
            auto& bulletComp = registry->emplace<BulletComp>(bullet);
            bulletComp.targetSide = targetSide;
            bulletComp.shooter = entity;
            bulletComp.damage = weaponComp.bulletDamage;
            switch (weaponComp.bulletType)
            {
                case BulletType::SINGLE:
                {
                    float speed = weaponComp.bulletSpeed;
                    if (targetSide==Faction::ENEMY)
                        speed*=-1;
                    registry->emplace<VelocityComp>(bullet, 0, speed);
                    const auto& physRect = view.get<const PhysicalComp>(entity).rect;
                    float x = physRect.x+(physRect.w/2)-(BLASTER_BULLET_WIDTH/2);
                    float y = physRect.y-BLASTER_BULLET_HEIGHT-speed;
                    if (targetSide==Faction::PLAYER)
                        y+=physRect.h-BLASTER_BULLET_HEIGHT;
                    registry->emplace<PositionComp>(bullet, x, y);
                    registry->emplace<PhysicalComp>(bullet, x, y,
                        BLASTER_BULLET_WIDTH, BLASTER_BULLET_HEIGHT);
                    break;
                }
                case BulletType::DUAL:
                {
                    auto bulletB = registry->create();
                    auto& bulletCompB = registry->emplace<BulletComp>(bulletB);
                    bulletCompB.targetSide = targetSide;
                    bulletCompB.shooter = entity;
                    bulletCompB.damage = weaponComp.bulletDamage;
                    float speed = weaponComp.bulletSpeed;
                    if (targetSide==Faction::ENEMY)
                        speed*=-1;
                    const auto& physRect = view.get<const PhysicalComp>(entity).rect;
                    for (size_t i = 0; i<2; i++)
                    {
                        entt::entity bulletEntity = i==0 ? bullet : bulletB;
                        registry->emplace<VelocityComp>(bulletEntity, 0, speed);
                        float x = physRect.x+(physRect.w/2)-(BLASTER_BULLET_WIDTH/2)
                            -(BLASTER_DUAL_GAP/2.0f)+(BLASTER_DUAL_GAP*i);
                        float y = physRect.y-BLASTER_BULLET_HEIGHT-speed;
                        if (targetSide==Faction::PLAYER)
                            y+=physRect.h-BLASTER_BULLET_HEIGHT;
                        registry->emplace<PositionComp>(bulletEntity, x, y);
                        registry->emplace<PhysicalComp>(bulletEntity, x, y,
                            BLASTER_BULLET_WIDTH, BLASTER_BULLET_HEIGHT);
                    }
                    break;
                }
                case BulletType::SPREAD:
                {
                    // TODO
                    break;
                }
                case BulletType::BEAM:
                {
                    // TODO
                    break;
                }
            }
            // FIRE BULLET [END]
        }
        weaponComp.steps++;
        if (weaponComp.steps>=weaponComp.stepsPerBullet)
            weaponComp.steps = 0;
    }
}

void WeaponSystem::destroy()
{
    bulletHitSubject->removeObserver(
        static_cast<Observer<struct BulletHitEvent>*>(&hitObserver));
}

void WeaponSystem::equipSingleBlaster(entt::entity entity)
{
    auto& weaponComp = registry->emplace<WeaponSpecComp>(entity);
    weaponComp.bulletType = BulletType::SINGLE;
    weaponComp.bulletDamage = 100;
    weaponComp.bulletSpeed = 50;
    weaponComp.stepsPerBullet = 10;
    weaponComp.steps = 0;
}

void WeaponSystem::equipDualBlaster(entt::entity entity)
{
    auto& weaponComp = registry->emplace<WeaponSpecComp>(entity);
    weaponComp.bulletType = BulletType::DUAL;
    weaponComp.bulletDamage = 80;
    weaponComp.bulletSpeed = 60;
    weaponComp.stepsPerBullet = 5;
    weaponComp.steps = 0;
}

void WeaponSystem::equipBeam(entt::entity entity)
{

}

void WeaponSystem::equipSpreadGun(entt::entity entity)
{

}

void Renderer::init(entt::registry* registry, AssetMap* assets, ALLEGRO_BITMAP* mainBitmap)
{
    this->registry = registry;
    this->assets = assets;
    this->mainBitmap = mainBitmap;
    ALLEGRO_BITMAP* bgSource = assets->getBitmap("background");
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR);
    background = al_create_bitmap(VIEWPORT_WIDTH, VIEWPORT_WIDTH);
    al_set_target_bitmap(background);
    al_draw_scaled_bitmap(bgSource,
        0, 0, al_get_bitmap_width(bgSource), al_get_bitmap_height(bgSource),
        0, 0, VIEWPORT_WIDTH, VIEWPORT_WIDTH, 0);
    al_set_target_bitmap(mainBitmap);
    playerShip = assets->getBitmap("ship2");
    redShader = assets->getShader("red_shader");
}

void Renderer::draw(float interpolation, double delta)
{
    {
        auto view = registry->view<HitShaderComp>();
        for (auto entity : view)
        {
            auto& hitShaderComp = view
                .get<HitShaderComp>(entity);
            hitShaderComp.timeout-=delta;
            if (hitShaderComp.timeout<0.0f)
                hitShaderComp.timeout = 0.0f;
        }
    }
    al_clear_to_color(al_map_rgb(25, 10, 40));
    al_draw_bitmap(background, 0, 0, 0);
    // draw enemies
    {
        auto view = registry->view<
            const EnemyComp,
            const PhysicalComp,
            const PhysicalBoundTextureComp
        >();
        for (auto entity : view)
        {
            const auto& physComp = view
                .get<PhysicalComp>(entity);
            const auto& textureComp = view
                .get<PhysicalBoundTextureComp>(entity);
            auto hitShaderComp = registry
                ->try_get<HitShaderComp>(entity);
            if (hitShaderComp->timeout>=0.01f)
            {
                al_draw_tinted_scaled_bitmap(
                    textureComp.texture,
                    al_map_rgb_f(10.0f, 0.0f, 0.0f),
                    0,
                    0,
                    al_get_bitmap_width(textureComp.texture),
                    al_get_bitmap_height(textureComp.texture),
                    physComp.rect.x+textureComp.offset.x,
                    physComp.rect.y+textureComp.offset.y,
                    textureComp.size.w,
                    textureComp.size.h,
                    0
                );
            }
            else
            {
                al_draw_scaled_bitmap(
                    textureComp.texture,
                    0,
                    0,
                    al_get_bitmap_width(textureComp.texture),
                    al_get_bitmap_height(textureComp.texture),
                    physComp.rect.x+textureComp.offset.x,
                    physComp.rect.y+textureComp.offset.y,
                    textureComp.size.w,
                    textureComp.size.h,
                    0
                );
            }
        }
    }
    // draw players
    {
        auto view = registry->view<const PhysicalComp, PlayerComp>();
        for (auto entity : view)
        {
            const auto& physComp = view
                .get<const PhysicalComp>(entity);
            auto& playerComp = view.get<PlayerComp>(entity);
            playerComp.animationTime+=delta;
            int bw = al_get_bitmap_width(playerShip);
            int bh = al_get_bitmap_height(playerShip);
            const float scale = 1.6f;
            float w = physComp.rect.w*scale;
            float h = physComp.rect.h*scale;
            float x = physComp.rect.x+physComp.rect.w/2-w/2;
            float y = physComp.rect.y+physComp.rect.h/2-h/2;
            // ALLEGRO_BITMAP* exhaust = assets->getBitmap(
            //     std::fmod(playerComp.animationTime, 1.0f)>0.5f
            //     ? "exhaust_01" : "exhaust_02");
            // int exhaustBW = al_get_bitmap_width(exhaust);
            // int exhaustBH = al_get_bitmap_height(exhaust);
            // al_draw_scaled_bitmap(exhaust, 0, 0, exhaustBW, exhaustBH,
            //     x+w/2-16, y+h-16, 32, 32, 0);

            al_set_target_bitmap(playerComp.bitmap);
            al_clear_to_color(al_map_rgba(0, 0, 0, 0));
            al_draw_scaled_bitmap(
                playerShip,
                0,
                0,
                bw,
                bh,
                x,
                y,
                w,
                h,
                0
            );
            al_set_target_bitmap(mainBitmap);
            al_draw_bitmap(playerComp.bitmap, 0, 0, 0);
            
            // if (weaponComp.weapon)
            //     weaponComp.weapon->draw(interpolation, delta, assets, {x, y, w, h});
        }
    }
    // draw bullets
    {
        auto view = registry->view<const BulletComp, const PhysicalComp>();
        for (auto entity : view)
        {
            const auto& physComp = view.get<PhysicalComp>(entity);
            auto velComp = registry->try_get<const VelocityComp>(entity);
            FRect rect = physComp.rect;
            if (velComp)
                lerpRect(rect, velComp->vel, interpolation);
            al_draw_filled_rectangle(rect.x, rect.y,
                rect.x+rect.w, rect.y+rect.h, al_map_rgb(255, 20, 80));
        }
    }
#if DEBUG_WIREFRAME
    // draw wireframes
    {
        auto view = registry->view<const PhysicalComp>();
        for (auto entity : view)
        {
            const auto& physComp = view.get<const PhysicalComp>(entity);
            const auto velComp = registry->try_get<const VelocityComp>(entity);
            FRect rect = physComp.rect;
            // if (velComp)
            //     lerpRect();//TODO
            if (velComp)
            {
                rect.x = std::lerp(rect.x, rect.x+velComp->vel.x*STEP_TIME, interpolation);
                rect.y = std::lerp(rect.y, rect.y+velComp->vel.y*STEP_TIME, interpolation);
            }
            const auto colMarkComp = registry->try_get<const CollisionMarkerComp>(entity);
            auto color = colMarkComp&&colMarkComp->mark
                ? al_map_rgb(200, 200, 200) : al_map_rgba(25, 255, 25, 50);
            al_draw_rectangle(rect.x, rect.y, rect.x+rect.w, rect.y+rect.h, color, 1);
        }
    }
#endif
    // debug
    {
        auto view = registry->view<
            const PlayerComp, const PhysicalComp, const VelocityComp>();
        for (auto entity : view)
        {
            const auto& physComp = view.get<const PhysicalComp>(entity);
            const auto& velComp = view.get<const VelocityComp>(entity);
            debug("rect=%s, vel=%s", physComp.rect.toString().c_str(), velComp.vel.toString().c_str());
            auto scoreComp = registry->try_get<const ScoreComp>(entity);
            if (scoreComp)
                debug("score=%d", scoreComp->score);
        }
    }
}

bool World::init(int w, int h, AssetMap* assets, ALLEGRO_BITMAP* mainBitmap)
{
    FN_DURATION();
    dim.w = w;
    dim.h = h;
    enemySystem.init(&registry, assets);
    levelSystem.init(&enemySystem, &registry, assets, dim);
    weaponSystem.init(&registry, dim, &bulletHitSubject, assets);
    playerSystem.init(&registry, &weaponSystem);
    physicsSystem.init(&registry, dim, &bulletHitSubject);
    renderer.init(&registry, assets, mainBitmap);
    return true;
}

void World::onInput(InputEvent* ev)
{
    playerSystem.handleInput(ev);
}

void World::tick()
{
    levelSystem.update();
    // 1. Update input + player weapon fire
    playerSystem.update();
    weaponSystem.update();
    // 2. Update AI + enemy weapon fire - TODO
    // 3. Update positions
    physicsSystem.applyMovement();
    // 4. Resolve collisions - screenbound, roadbound, player-enemy, bullets
    physicsSystem.checkCollisions();
    // 5. Update health + effects - TODO
}

void World::draw(float interpolation, double delta)
{
    renderer.draw(interpolation, delta);
    debug("Distance: %.0f", levelSystem.getDistance());
}

void World::destroy()
{
    playerSystem.destroy();
    weaponSystem.destroy();
}
