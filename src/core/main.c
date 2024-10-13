#include "defs.h"
#include "flecs.h"
#include "state.h"
#include "transform.h"
#include <raylib.h>
#include <stdio.h>

ecs_world_t* world;

const u32 screenWidth = 480;
const u32 screenHeight = 270;

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define MAG(v) sqrt(v.x* v.x + v.y * v.y)

v2 mouse;
f32 time;

v2 v2Clamp(v2 vec, v2 min, v2 max) {
    return (v2){MIN(MAX(vec.x, min.x), max.x),
                MIN(MAX(vec.y, min.y), max.y)};
}

v2 getScreenMousePos(v2* mouse, f32 scale, i32 sw, i32 sh) {
    v2 mouseOLD = GetMousePosition();
    mouse->x =
        (mouseOLD.x - (GetScreenWidth() - (sw * scale)) * 0.5f) / scale;
    mouse->y =
        (mouseOLD.y - (GetScreenHeight() - (sh * scale)) * 0.5f) / scale;
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

typedef struct Sprite {
    Texture2D texture;
    int radius;
    bool drawSprite;
} Sprite;

ECS_COMPONENT_DECLARE(Sprite);

void render(ecs_world_t* world) {
    ecs_query_t* q =
        ecs_query(world, {.terms = {{.id = ecs_id(Position)},
                                    {.id = ecs_id(Sprite), .inout = EcsIn}},
                          .cache_kind = EcsQueryCacheAuto});

    ecs_iter_t it = ecs_query_iter(world, q);

    while (ecs_query_next(&it)) {
        Position* p = ecs_field(&it, Position, 0);
        const Sprite* s = ecs_field(&it, Sprite, 1);

        for (int i = 0; i < it.count; i++) {
            if (s[i].drawSprite) {
                DrawTexture(s[i].texture, p[i].x, p[i].y, WHITE);
            } else {
                DrawCircle(p[i].x, p[i].y, s[i].radius, WHITE);
            }
        }
    }
}

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(screenWidth, screenHeight, "Planet Generation Test");
    InitAudioDevice();
    SetMasterVolume(1);
    SetTargetFPS(60);
    SetWindowSize(screenWidth * 2, screenHeight * 2);
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    Shader scanlines = LoadShader(0, "scanlines.frag.glsl");

    world = ecs_init();
    TransformImport();
    ECS_COMPONENT_DEFINE(world, Sprite);
    ecs_entity_t e = ecs_entity(world, {.name = "test"});
    ecs_set(world, e, Position, {50, 50});
    ecs_set(world, e, Velocity, {50, 50});
    ecs_set(world, e, Sprite,
            {.texture = LoadTexture("assets/images/mainMenu.png"),
             .radius = 10,
             .drawSprite = true});
    ecs_add_id(world, e, _controllable);

    while (!WindowShouldClose()) {
        f32 scale = MIN((f32)GetScreenWidth() / screenWidth,
                        (float)GetScreenHeight() / screenHeight);
        mouse = getScreenMousePos(&mouse, scale, screenWidth, screenHeight);

        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            printf("pressed middle mouse button\n");
        }

        BeginTextureMode(target);
        ClearBackground(BLACK);

        render(world);
        ecs_progress(world, GetFrameTime());
        time += GetFrameTime();

        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        BeginShaderMode(scanlines);
        drawScaledWindow(target, screenWidth, screenHeight, scale);
        EndShaderMode();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
