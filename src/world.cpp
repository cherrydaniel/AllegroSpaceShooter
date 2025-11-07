#include "world.h"
#include "game.h"
#include "duration.h"

void LevelSystem::spawnEnemy()
{
    auto enemy = registry->create();
    auto& enemyComp = registry->emplace<EnemyComp>(enemy);
    enemyComp.enemy = enemyFactory.create(enemy, EnemyType::SQUID);
    enemyComp.enemy->init(SquidEnemy::SIZE, -SquidEnemy::SIZE);
}

void LevelSystem::init(entt::registry* registry, AssetMap* assets,
    const IVec2& dim)
{
    this->registry = registry;
    this->assets = assets;
    this->dim = dim;
    enemyFactory.init(registry, assets);
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
    distance+=ROAD_MOVE_SPEED;
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

void PlayerSystem::init(entt::registry* registry)
{
    this->registry = registry;
    auto player = registry->create();
    registry->emplace<PlayerControlComp>(player, 1);
    registry->emplace<VelocityComp>(player);
    registry->emplace<PositionComp>(player,
        VIEWPORT_WIDTH/2-PLAYER_WIDTH/2, VIEWPORT_HEIGHT/2-PLAYER_HEIGHT/2);
    registry->emplace<PhysicalComp>(player,
        0.0f, 0.0f, PLAYER_WIDTH, PLAYER_HEIGHT);
    registry->emplace<ScreenBoundComp>(player);
    registry->emplace<RoadBoundComp>(player);
    auto& playerComp = registry->emplace<PlayerComp>(player);
    playerComp.bitmap = al_create_bitmap(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    registry->emplace<ScoreComp>(player);
    registry->emplace<CollisionMarkerComp>(player);
    registry->emplace<WeaponComp>(player);
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
            case MOVE_LEFT_START:   ctlComp.stateFlags|=PLAYER_STATE_MOVING_LEFT;   break;
            case MOVE_LEFT_END:     ctlComp.stateFlags&=~PLAYER_STATE_MOVING_LEFT;  break;
            case MOVE_RIGHT_START:  ctlComp.stateFlags|=PLAYER_STATE_MOVING_RIGHT;  break;
            case MOVE_RIGHT_END:    ctlComp.stateFlags&=~PLAYER_STATE_MOVING_RIGHT; break;
            case MOVE_UP_START:     ctlComp.stateFlags|=PLAYER_STATE_MOVING_UP;     break;
            case MOVE_UP_END:       ctlComp.stateFlags&=~PLAYER_STATE_MOVING_UP;    break;
            case MOVE_DOWN_START:   ctlComp.stateFlags|=PLAYER_STATE_MOVING_DOWN;   break;
            case MOVE_DOWN_END:     ctlComp.stateFlags&=~PLAYER_STATE_MOVING_DOWN;  break;
        }
        auto weaponComp = registry->try_get<WeaponComp>(entity);
        if (weaponComp)
            weaponComp->weapon->handleInput(ev);
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

void RoadSystem::positionRoadSection(entt::entity entity)
{
    auto [posComp, roadComp] = registry->get<PositionComp, RoadComp>(entity);
    if (lastSection==entt::null)
    {
        posComp.pos.x = dim.w/2-ROAD_WIDTH/2;
        posComp.pos.y = dim.h-ROAD_SECTION_HEIGHT;
        roadComp.direction = FORWARD;
        lastSection = entity;
        return;
    }
    auto [lastPosComp, lastRoadComp] = registry->get<PositionComp, RoadComp>(lastSection);
    roadComp.direction = lastRoadComp.direction;
    if (directionChangeCountdown--<=0)
    {
        roadComp.direction = static_cast<RoadDirection>(rnd.range(0, 2));
        directionChangeCountdown = rnd.range(ROAD_DIRECTION_MIN, ROAD_DIRECTION_MAX);
    }
    posComp.pos.x = lastPosComp.pos.x;
    if (roadComp.direction==LEFT)
    {
        posComp.pos.x-=ROAD_SECTION_HEIGHT;
        if (posComp.pos.x<ROAD_PADDING)
        {
            posComp.pos.x+=ROAD_SECTION_HEIGHT;
            roadComp.direction = FORWARD;
        }
    }
    else if (roadComp.direction==RIGHT)
    {
        posComp.pos.x+=ROAD_SECTION_HEIGHT;
        if (posComp.pos.x>dim.w-ROAD_WIDTH-(ROAD_PADDING*2))
        {
            posComp.pos.x-=ROAD_SECTION_HEIGHT;
            roadComp.direction = FORWARD;
        }
    }
    posComp.pos.y = lastPosComp.pos.y-ROAD_SECTION_HEIGHT;
    lastSection = entity;
}

void RoadSystem::init(entt::registry* registry, const IVec2& dim)
{
    this->registry = registry;
    this->dim = dim;
    directionChangeCountdown = 20;
    const size_t numRoadSections = VIEWPORT_HEIGHT/ROAD_SECTION_HEIGHT+1;
    for (size_t i = 0; i<numRoadSections; i++)
    {
        auto section = registry->create();
        registry->emplace<RoadComp>(section);
        registry->emplace<PositionComp>(section);
        registry->emplace<VelocityComp>(section, 0, ROAD_MOVE_SPEED);
        registry->emplace<CollisionMarkerComp>(section);
        positionRoadSection(section);
    }
}

void RoadSystem::update()
{
    auto view = registry->view<const RoadComp, const PositionComp>();
    for (auto entity : view)
    {
        const auto& posComp = view.get<PositionComp>(entity);
        if (posComp.pos.y>=dim.h)
            positionRoadSection(entity);
    }
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

void PhysicsSystem::resolveRoadBound()
{
    auto view = registry->view<const PhysicalComp, const RoadBoundComp>();
    for (auto entity : view)
    {
        auto roadView = registry->view<const RoadComp, const PositionComp>();
        for (auto roadEntity : roadView)
        {
            const auto& physComp = view.get<const PhysicalComp>(entity);
            const auto& roadComp = roadView.get<const RoadComp>(roadEntity);
            const auto& roadPosComp = roadView.get<const PositionComp>(roadEntity);
            if (intersection(physComp.rect,
                {0.0f, roadPosComp.pos.y, roadPosComp.pos.x, ROAD_SECTION_HEIGHT}))
            {
                if (roadComp.direction==FORWARD)
                {
                    resolveCollisionX(entity, roadPosComp.pos.x);
                    markCollision(entity);
                    markCollision(roadEntity);
                }
                else
                {
                    float hitPos = roadPosComp.pos.x+(roadPosComp.pos.y-physComp.rect.y);
                    if (physComp.rect.x<hitPos)
                    {
                        resolveCollisionX(entity, hitPos);
                        markCollision(entity);
                        markCollision(roadEntity);
                    }
                }
            }
            else if (intersection(physComp.rect,
                {roadPosComp.pos.x+ROAD_WIDTH, roadPosComp.pos.y, dim.w-roadPosComp.pos.x+ROAD_WIDTH, ROAD_SECTION_HEIGHT}))
            {
                if (roadComp.direction==FORWARD)
                {
                    resolveCollisionX(entity, roadPosComp.pos.x+ROAD_WIDTH-physComp.rect.w);
                    markCollision(entity);
                    markCollision(roadEntity);
                }
                else
                {
                    float hitPos = roadPosComp.pos.x+ROAD_WIDTH-ROAD_SECTION_HEIGHT-(roadPosComp.pos.y-physComp.rect.y);
                    if (physComp.rect.x>hitPos)
                    {
                        resolveCollisionX(entity, hitPos);
                        markCollision(entity);
                        markCollision(roadEntity);
                    }
                }
            }
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
    resolveRoadBound();
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
    factory.init(registry, assets);
    hitObserver.init(registry);
    bulletHitSubject->addObserver(
        static_cast<Observer<struct BulletHitEvent>*>(&hitObserver));
}

void WeaponSystem::destroy()
{
    bulletHitSubject->removeObserver(
        static_cast<Observer<struct BulletHitEvent>*>(&hitObserver));
}

void WeaponSystem::update()
{
    auto view = registry->view<WeaponComp>();
    for (auto entity : view)
    {
        auto& weaponComp = view.get<WeaponComp>(entity);
        if (weaponComp.weapon)
            weaponComp.weapon->update();
    }
}

void WeaponSystem::equip(entt::entity entity, WeaponType weaponType, Side targetSide)
{
    auto& weaponComp = registry->get<WeaponComp>(entity);
    if (weaponComp.weapon)
    {
        weaponComp.weapon->destroy();
        delete weaponComp.weapon;
    }
    weaponComp.weapon = factory.create(entity, weaponType, targetSide);
    weaponComp.weapon->init();
}

void WeaponSystem::unequip(entt::entity entity)
{
    auto& weaponComp = registry->get<WeaponComp>(entity);
    if (weaponComp.weapon)
    {
        weaponComp.weapon->destroy();
        delete weaponComp.weapon;
    }
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
    al_clear_to_color(al_map_rgb(25, 10, 40));
    al_draw_bitmap(background, 0, 0, 0);
#if ROAD_ACTIVE
    // draw road
    {
        auto view = registry->view<
            const RoadComp, const PositionComp, const VelocityComp>();
        for (auto entity : view)
        {
            const auto& roadComp = view.get<const RoadComp>(entity);
            const auto& posComp = view.get<const PositionComp>(entity);
            const auto& velComp = view.get<const VelocityComp>(entity);
            float bottomOffset = 0.0f;
            FVec2 pos = posComp.pos;
            pos.x = std::lerp(pos.x, pos.x+velComp.vel.x*STEP_TIME, interpolation);
            pos.y = std::lerp(pos.y, pos.y+velComp.vel.y*STEP_TIME, interpolation);
            switch (roadComp.direction)
            {
                case LEFT: bottomOffset = ROAD_SECTION_HEIGHT-2; break;
                case RIGHT: bottomOffset = -ROAD_SECTION_HEIGHT+2; break;
            }
            const auto colMarkComp = registry->try_get<const CollisionMarkerComp>(entity);
            auto color = colMarkComp&&colMarkComp->mark
                ? al_map_rgb(200, 200, 200) : al_map_rgb(255, 25, 25);
            al_draw_line(pos.x, pos.y, pos.x+bottomOffset,
                pos.y+ROAD_SECTION_HEIGHT-2, color, 4);
            al_draw_line(pos.x+ROAD_WIDTH, pos.y, pos.x+ROAD_WIDTH+bottomOffset,
                pos.y+ROAD_SECTION_HEIGHT-2, color, 4);
        }
    }
#endif
    // draw enemies
    {
        auto view = registry->view<const EnemyComp, const PhysicalComp>();
        for (auto entity : view)
        {
            const auto& enemyComp = view.get<const EnemyComp>(entity);
            const auto& physComp = view.get<const PhysicalComp>(entity);
            enemyComp.enemy->draw(interpolation, delta, physComp.rect);
        }
    }
    // draw players
    {
        auto view = registry->view<const PhysicalComp, PlayerComp, const WeaponComp>();
        for (auto entity : view)
        {
            const auto& physComp = view.get<const PhysicalComp>(entity);
            const auto& weaponComp = view.get<const WeaponComp>(entity);
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
            al_use_shader(redShader);
            al_draw_scaled_bitmap(playerShip, 0, 0, bw, bh, x, y, w, h, 0);
            al_use_shader(nullptr);
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
            const auto velComp = registry->try_get<const VelocityComp>(entity);
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
            if (velComp)
                lerpRect();//TODO
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
    levelSystem.init(&registry, assets, dim);
    playerSystem.init(&registry);
#if ROAD_ACTIVE
    roadSystem.init(&registry, dim);
#endif
    physicsSystem.init(&registry, dim, &bulletHitSubject);
    weaponSystem.init(&registry, dim, &bulletHitSubject, assets);
    renderer.init(&registry, assets, mainBitmap);
    auto playerView = registry.view<const PlayerComp>();
    for (auto player : playerView)
        weaponSystem.equip(player, WeaponType::BASIC, Side::ENEMY);
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
#if ROAD_ACTIVE
    roadSystem.update();
#endif
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
    weaponSystem.destroy();
}
