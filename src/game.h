#ifndef _GAME_GAME_H_
#define _GAME_GAME_H_

#include <cstdarg>
#include <cstring>
#include <string>
#include <memory>
#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "input.h"
#include "world.h"
#include "circularqueue.h"
#include "assetloader.h"
#include "allegro_util.h"
#include "threadpool.h"
#include "eventloop.h"
#include "duration.h"

#define DEBUG_FPS 1
#define DEBUG_ALLOC 1
#define DEBUG_CAM 1
#define DEBUG_ON_SCREEN 1
#define DEBUG_ON_SCREEN_QLEN 24
#define DEBUG_ON_SCREEN_STRLEN 1024
#define DEBUG_WIREFRAME 1
#define FPS 60.0f
#define DISPLAY_WIDTH 960.0f//1920
#define DISPLAY_HEIGHT 540.0f//1080
#define VIEWPORT_WIDTH 640.0f
#define VIEWPORT_HEIGHT 360.0f
#define TILE_SIZE 8.0f
#define STEP_TIME (1.0f/25.0f)
#define STEP_TIME_SUBSTEPS 6.0f
#define ROAD_ACTIVE 0

class Handler;
class Player;
class World;
struct Context;
class HandlerDirector;

template <size_t N>
struct StringMessage
{
    char str[N];
    StringMessage(){}
    StringMessage(const char* v){ strcpy_s(str, v); }
    StringMessage(const StringMessage& other){ strcpy_s(str, other.str); }
    StringMessage(StringMessage&& other){ strcpy_s(str, other.str); }
    StringMessage& operator=(const StringMessage& other)
    {
        if (this==&other)
            return *this;
        strcpy_s(str, other.str);
        return *this;
    }
};

#if DEBUG_ON_SCREEN
extern CircularQueue<StringMessage<DEBUG_ON_SCREEN_STRLEN>, DEBUG_ON_SCREEN_QLEN> debugQueue;
#endif

struct Context
{
    std::unique_ptr<HandlerDirector> handlerDirector;
    DisplayPointer display;
    FontPointer font;
    BitmapPointer mainBitmap;
    EventLoop evloop;
    double deltaMultiplier = 1.0f;
};

inline void debug(const char* fmt, ...)
{
#if DEBUG_ON_SCREEN
    char str[DEBUG_ON_SCREEN_STRLEN];
    va_list args;
    va_start(args, fmt);
    vsnprintf(str, sizeof str, fmt, args);
    va_end(args);
    debugQueue.emplace(str);
#endif
}

class Handler
{
public:
    virtual ~Handler(){}
    virtual bool init(void){ return true; };
    virtual void onInput(InputEvent* ev){};
    virtual void tick(void){};
    virtual void draw(float interpolation, double delta){};
    virtual void destroy(){};
};

class HandlerDirector
{
private:
    std::unique_ptr<Handler> handler;
public:
    bool set(std::unique_ptr<Handler>&& h);
    void onInput(InputEvent* ev);
    void tick(void);
    void draw(float interpolation, double delta);
    void destroy();
};

class TestHandler : public Handler
{
private:
    struct Context* ctx;
    std::unique_ptr<AssetMap> assets;
    World world;
    std::vector<std::future<void>> futures;
public:
    TestHandler(struct Context* ctx, std::unique_ptr<AssetMap>&& assets) :
        ctx{ctx},
        assets{std::move(assets)}
    {}
    ~TestHandler(){}
    bool init(void) override;
    void onInput(InputEvent* ev) override;
    void tick(void) override;
    void draw(float interpolation, double delta) override;
    void destroy() override;
};

class SplashHandler : public Handler
{
private:
    struct Context* ctx;
    std::unique_ptr<AssetMap> assets;
    std::unique_ptr<AssetLoader> assetLoader;
    Duration duration{"ASSET LOADING"};
public:
    SplashHandler(struct Context* ctx) : ctx{ctx}
    {
        assets = std::make_unique<AssetMap>();
        assetLoader = std::make_unique<AssetLoader>(&ctx->evloop, assets.get());
    }
    bool init(void) override;
    void tick(void) override;
    void draw(float interpolation, double delta) override;
};

class GameHandler : public Handler
{
private:
    struct Context* ctx;
    std::unique_ptr<AssetMap> assets;
    World world;
public:
    GameHandler(struct Context* ctx, std::unique_ptr<AssetMap>&& assets) :
        ctx{ctx},
        assets{std::move(assets)}
    {}
    ~GameHandler(){};
    bool init(void) override;
    void onInput(InputEvent* ev) override;
    void tick(void) override;
    void draw(float interpolation, double delta) override;
    void destroy() override;
};

class Game
{
private:
    std::unique_ptr<struct Context> ctx;
public:
    Game() : ctx{std::make_unique<struct Context>()} {}
    bool init();
    void gameloop();
};

#endif
