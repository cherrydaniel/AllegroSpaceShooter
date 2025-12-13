#include <string>
#include <inttypes.h>
#include <future>
#include <mutex>
#include <unordered_map>
#include "util/common.h"
#include "util/framecount.h"
#include "util/duration.h"
#include "game.h"
#include "input.h"

#if DEBUG_ON_SCREEN
CircularQueue<StringMessage<DEBUG_ON_SCREEN_STRLEN>, DEBUG_ON_SCREEN_QLEN> debugQueue;
#endif

#if DEBUG_ALLOC
static struct
{
    uint32_t totalAlloc = 0;
    uint32_t totalFreed = 0;
} allocationMetrics;

void* operator new(size_t size)
{
    void* ptr = malloc(size);
    if (!ptr)
        throw std::bad_alloc();
    allocationMetrics.totalAlloc+=size;
    return ptr;
}

void operator delete(void* ptr, size_t size) noexcept
{
    allocationMetrics.totalFreed+=size;
    free(ptr);
}
#endif

bool HandlerDirector::set(std::unique_ptr<Handler>&& h)
{
    if (handler.get())
    {
        handler->destroy();
        handler.reset();
    }
    handler = std::move(h);
    return handler->init();
}

void HandlerDirector::onInput(InputEvent* ev)
{
    if (handler.get())
        handler->onInput(ev);
}

void HandlerDirector::tick(void)
{
    if (!handler.get())
        return;
    // FN_DURATION();
    handler->tick();
}

void HandlerDirector::draw(float interpolation, double delta)
{
    if (handler.get())
        handler->draw(interpolation, delta);
}

void HandlerDirector::destroy()
{
    if (handler.get())
        handler->destroy();
}

bool TestHandler::init(void)
{
    ThreadPool* tpool = new ThreadPool(4);
    ALLEGRO_DISPLAY* disp = ctx->display.get();
    tpool->enqueue([disp]
    {
        PRINT_DEBUG("DISPLAY %p", disp);
    });
    futures.push_back(std::async(std::launch::async, [tpool]
    {
        al_rest(3);
        delete tpool;
    }));
    return true;
}

void TestHandler::onInput(InputEvent* ev)
{

}

void TestHandler::tick(void)
{

}

void TestHandler::draw(float interpolation, double delta)
{

}

void TestHandler::destroy()
{

}

bool SplashHandler::init(void)
{
    // BITMAPS
    assetLoader->loadBitmap("background",   "./assets/images/stars_texture.png");
    assetLoader->loadBitmap("explosions",   "./assets/images/explosions.png");
    assetLoader->loadBitmap("ship",         "./assets/images/ship.png");
    assetLoader->loadBitmap("ship2",        "./assets/images/ship2.png");
    assetLoader->loadBitmap("exhaust_01",   "./assets/images/exhaust_01.png");
    assetLoader->loadBitmap("exhaust_02",   "./assets/images/exhaust_02.png");
    assetLoader->loadBitmap("turret_01",    "./assets/images/turret_01.png");
    assetLoader->loadBitmap("turret_02",    "./assets/images/turret_02.png");
    assetLoader->loadBitmap("enemy_squid",  "./assets/images/enemy_squid.png");
    // SHADERS
    assetLoader->loadShader("red_shader",   "./assets/shaders/red.glsl");
    // MISC
    assetLoader->loadUserData("userdata",   "./data/user.data");
    assetLoader->loadSettings("settings",   "./data/settings.data");
    return true;
}

void SplashHandler::tick(void)
{
    PRINT_DEBUG("requested=%d, loaded=%d, failed=%d",
        assetLoader->numRequested(),
        assetLoader->numLoaded(),
        assetLoader->numFailed());
    if (assetLoader->numRequested()==assetLoader->numLoaded())
        ctx->handlerDirector->set(std::make_unique<GameHandler>(ctx, move(assets)));
        // ctx->handlerDirector->set(std::make_unique<TestHandler>(ctx, move(assets)));
}

#define LOADING_TEXT "Loading..."

void SplashHandler::draw(float interpolation, double delta)
{
    al_clear_to_color(al_map_rgb(30, 20, 40));
    int bx, by, bw, bh;
    al_get_text_dimensions(ctx->font.get(), LOADING_TEXT, &bx, &by, &bw, &bh);
    bw+=bx;
    bh+=by;
    al_draw_text(ctx->font.get(), al_map_rgb(255, 255, 255),
        VIEWPORT_WIDTH/2-bw/2, VIEWPORT_HEIGHT/2-bh/2, 0, LOADING_TEXT);
}

bool GameHandler::init(void)
{
    return world.init(
        VIEWPORT_WIDTH,
        VIEWPORT_HEIGHT,
        assets.get(),
        ctx->mainBitmap.get()
    );
}

void GameHandler::onInput(InputEvent *ev)
{
    world.onInput(ev);
}

void GameHandler::tick(void)
{
    world.tick();
}

void GameHandler::draw(float interpolation, double delta)
{
    world.draw(interpolation, delta);
}

void GameHandler::destroy()
{
    world.destroy();
}

bool Game::init()
{
    FN_DURATION();
    ctx->handlerDirector = std::make_unique<HandlerDirector>();
    CHECK_MSG(al_init(), "Allegro init failed");
    CHECK_MSG(al_init_primitives_addon(), "Allegro primitive addon init failed");
    CHECK_MSG(al_init_image_addon(), "Allegro image addon init failed");
    CHECK_MSG(al_init_font_addon(), "Allegro font addon init failed");
    CHECK_MSG(al_init_ttf_addon(), "Allegro TTF addon init failed");
    CHECK_MSG(al_install_keyboard(), "Allegro install keyboard failed");
    CHECK_MSG(al_install_joystick(), "Allegro install joystick failed");
    al_set_new_display_flags(
        ALLEGRO_PROGRAMMABLE_PIPELINE|
        ALLEGRO_OPENGL|
        // ALLEGRO_FULLSCREEN_WINDOW|
        ALLEGRO_WINDOWED|
        ALLEGRO_RESIZABLE
    );
    ctx->display = DisplayPointer(
        al_create_display(DISPLAY_WIDTH, DISPLAY_HEIGHT));
    CHECK_MSG(ctx->display.get(), "Display creation failed");
    al_set_window_title(ctx->display.get(), GAME_TITLE);
    al_hide_mouse_cursor(ctx->display.get());
    ctx->font = FontPointer(
        al_load_font("./assets/fonts/BoldPixels1.4.ttf", 16, 0));
    CHECK_MSG(ctx->font.get(), "Main font load failed");
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR);
    ctx->mainBitmap = BitmapPointer(
        al_create_bitmap(VIEWPORT_WIDTH, VIEWPORT_HEIGHT));
    CHECK_MSG(ctx->mainBitmap.get(), "Main bitmap creation failed");
    CHECK_MSG(ctx->handlerDirector->set(std::make_unique<SplashHandler>(ctx.get())),
        "Setting splash handler failed");
    return true;
}

void Game::gameloop()
{
#if DEBUG_FPS
    FrameCounter fc;
#endif
#if DEBUG_ON_SCREEN
    StringMessage<DEBUG_ON_SCREEN_STRLEN> debugstr;
#endif
    ALLEGRO_EVENT_QUEUE* evq = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0/FPS);
    ALLEGRO_EVENT ev;
    InputEvent inputEvent;
    al_register_event_source(evq, al_get_timer_event_source(timer));
    al_register_event_source(evq, al_get_display_event_source(ctx->display.get()));
    al_register_event_source(evq, al_get_keyboard_event_source());
    al_register_event_source(evq, al_get_joystick_event_source());
    bool redraw = false;
    bool haltDraw = false;
    double thisTime = 0;
    double prevTime = 0;
    double accTime = 0;
    double delta = 0;
    al_start_timer(timer);
    while (true)
    {
        al_wait_for_event(evq, &ev);
        switch (ev.type)
        {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                goto finalize;
            case ALLEGRO_EVENT_TIMER:
                redraw = true;
                break;
            case ALLEGRO_EVENT_DISPLAY_RESIZE:
                al_acknowledge_resize(ev.display.source);
                break;
            case ALLEGRO_EVENT_DISPLAY_HALT_DRAWING:
                haltDraw = true;
                al_acknowledge_drawing_halt(ev.display.source);
                break;
            case ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING:
                haltDraw = false;
                al_acknowledge_drawing_resume(ev.display.source);
                break;
            default:
                if (mapAction(&ev, &inputEvent))
                    ctx->handlerDirector->onInput(&inputEvent);
        }
        if (al_event_queue_is_empty(evq)&&redraw&&!haltDraw)
        {
            ctx->evloop.tick();
            thisTime = al_get_time();
            if (prevTime)
                delta = thisTime-prevTime;
            accTime += delta;
#if DEBUG_FPS
            fc.tick(delta);
#endif
            while (accTime>=STEP_TIME)
            {
                ctx->handlerDirector->tick();
                accTime-=STEP_TIME;
            }
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_set_target_bitmap(ctx->mainBitmap.get());
            al_clear_to_color(al_map_rgb(0, 0, 0));
            ctx->handlerDirector->draw(accTime/STEP_TIME, delta);
#if DEBUG_FPS
            al_draw_textf(ctx->font.get(), al_map_rgb(25, 255, 25), 32, 32, 0,
                "FPS: %d", fc.fps());
#endif
#if DEBUG_ALLOC
            al_draw_textf(ctx->font.get(), al_map_rgb(25, 255, 25), 32, 52, 0,
                "Heap usage: %" PRIu32 " bytes",
                allocationMetrics.totalAlloc-allocationMetrics.totalFreed);
#endif
#if DEBUG_ON_SCREEN
            size_t dbIdx = 0;
            while (debugQueue.read(&debugstr))
            {
                al_draw_text(ctx->font.get(), al_map_rgb(255, 255, 255),
                    32, 72+(20*dbIdx), 0, debugstr.str);
                dbIdx++;
            }
#endif
            al_set_target_backbuffer(ctx->display.get());
            // -- center image with letterboxing [START]
            float bw = (float)al_get_bitmap_width(ctx->mainBitmap.get());
            float bh = (float)al_get_bitmap_height(ctx->mainBitmap.get());
            float w = (float)al_get_display_width(ctx->display.get());
            float h = (float)al_get_display_height(ctx->display.get());
            if (w/h>bw/bh)
            {
                float dw = h*(bw/bh);
                al_draw_scaled_bitmap(ctx->mainBitmap.get(), 0, 0, bw, bh,
                    w/2-dw/2, 0, dw, h, 0);
            }
            else
            {
                float dh = w*(bh/bw);
                al_draw_scaled_bitmap(ctx->mainBitmap.get(), 0, 0, bw, bh,
                    0, h/2-dh/2, w, dh, 0);
            }
            // -- center image with letterboxing [END]
            al_flip_display();
            redraw = false;
            prevTime = thisTime;
        }
    }
finalize:
    al_destroy_timer(timer);
    al_destroy_event_queue(evq);
}
