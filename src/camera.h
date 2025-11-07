#ifndef _GAME_CAMERA_H_
#define _GAME_CAMERA_H_

#include <allegro5/allegro.h>
#include "common.h"
#include "maths.h"
#include "game.h"

struct Context;
class HandlerDirector;

class Camera
{
private:
    struct Context* ctx;
    float zoomScale = 1.f;
    float rotateDegrees = 0.f;
    ALLEGRO_TRANSFORM originalTransform;
public:
    FVec2 pos;
    FVec2 dim;
    Camera(struct Context* ctx, FVec2 pos, FVec2 dim)
        : ctx{ctx}, pos{pos}, dim{dim} {}
    ~Camera(){}
    void act(void);
    void applyProjection(void);
    void restoreProjection(void);
    void moveTo(FVec2 to, float time=0.0);
    void moveBy(FVec2 by, float time=0.0);
    void zoomTo(float to, float time=0.0);
    void zoomBy(float by, float time=0.0);
    void rotateTo(float to, float time=0.0);
    void rotateBy(float by, float time=0.0);
    void shake(float amp, float time=1.0);
};

#endif
