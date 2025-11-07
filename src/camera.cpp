#include "camera.h"

void Camera::act(void)
{
    // TODO: Make all updates here before applying projection
    
}

void Camera::applyProjection(void)
{
    originalTransform = *al_get_current_transform();
    ALLEGRO_TRANSFORM trans;
    float w = (float)al_get_bitmap_width(ctx->mainBitmap.get());
    float h = (float)al_get_bitmap_height(ctx->mainBitmap.get());
    al_identity_transform(&trans);
    al_translate_transform(&trans, w/2-pos.x-dim.x/2, h/2-pos.y-dim.y/2);
    al_rotate_transform(&trans, rotateDegrees);
    al_scale_transform(&trans, w/dim.x*zoomScale, h/dim.y*zoomScale);
    al_translate_transform(&trans, dim.x/2, dim.y/2);
    al_use_transform(&trans);
}

void Camera::restoreProjection(void)
{
    al_use_transform(&originalTransform);
#if DEBUG_CAM
    al_draw_rectangle(16, 16, VIEWPORT_WIDTH-16, VIEWPORT_HEIGHT-16,
        al_map_rgb(255, 255, 255), 2);
    al_draw_rectangle(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT,
        al_map_rgb(255, 255, 255), 2);
    al_draw_filled_rectangle(VIEWPORT_WIDTH/2-16, VIEWPORT_HEIGHT/2-1,
        VIEWPORT_WIDTH/2+16, VIEWPORT_HEIGHT/2+1, al_map_rgb(255, 255, 255));
    al_draw_filled_rectangle(VIEWPORT_WIDTH/2-1, VIEWPORT_HEIGHT/2-16,
        VIEWPORT_WIDTH/2+1, VIEWPORT_HEIGHT/2+16, al_map_rgb(255, 255, 255));
    al_draw_textf(ctx->font.get(),
        al_map_rgb(255, 255, 255), 32, VIEWPORT_HEIGHT-32-16, 0,
        "[CAM] {%.2f, %.2f} %d%% %.2fRAD", pos.x, pos.y, (int)(zoomScale*100.f), rotateDegrees);
#endif
}

void Camera::moveTo(FVec2 to, float time)
{
    pos.x = to.x;
    pos.y = to.y;
}

void Camera::moveBy(FVec2 by, float time)
{
    pos.x += by.x;
    pos.y += by.y;
}

void Camera::zoomTo(float to, float time)
{
    zoomScale = to;
}

void Camera::zoomBy(float by, float time)
{
    zoomScale+=by;
}

void Camera::rotateTo(float to, float time)
{
    rotateDegrees = to;
}

void Camera::rotateBy(float by, float time)
{
    rotateDegrees+=by;
}

void Camera::shake(float amp, float time)
{
}
