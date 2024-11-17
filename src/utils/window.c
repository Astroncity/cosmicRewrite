#include "window.h"
#include "state.h"

v2 v2Clamp(v2 vec, v2 min, v2 max) {
    return (v2){MIN(MAX(vec.x, min.x), max.x), MIN(MAX(vec.y, min.y), max.y)};
}

v2 getScreenMousePos(v2* mouse, f32 scale, i32 sw, i32 sh) {
    v2 mOld = GetMousePosition();
    mouse->x = (mOld.x - (GetScreenWidth() - (sw * scale)) * 0.5f) / scale;
    mouse->y = (mOld.y - (GetScreenHeight() - (sh * scale)) * 0.5f) / scale;
    *mouse = v2Clamp(*mouse, (v2){0, 0}, (v2){(f32)sw, (f32)sh});

    return *mouse;
}

void drawScaledWindow(RenderTexture2D target, f32 sw, f32 sh, f32 scale) {
    f32 tw = (f32)target.texture.width;
    f32 th = (f32)target.texture.height;
    Rect rect1 = {0.0f, 0.0f, tw, -th};
    f32 x = (GetScreenWidth() - (sw * scale)) * 0.5f;
    f32 y = (GetScreenHeight() - (sh * scale)) * 0.5f;

    Rect rect2 = {x, y, sw * scale, sh * scale};

    DrawTexturePro(target.texture, rect1, rect2, (v2){0, 0}, 0.0f, WHITE);
}

void setWindowFlags(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(screenWidth, screenHeight, "Planet Generation Test");
    InitAudioDevice();
    SetMasterVolume(1);
    SetTargetFPS(60);
    SetWindowSize(screenWidth * 2, screenHeight * 2);
}

f32 getWindowScale() {
    return MIN((f32)GetScreenWidth() / screenWidth,
               (float)GetScreenHeight() / screenHeight);
    return MIN((f32)GetScreenWidth() / screenWidth,
               (float)GetScreenHeight() / screenHeight);
}
