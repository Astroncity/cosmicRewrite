#include "flecs.h"
#include "planet.h"
#include "raylib.h"
#include "state.h"
#include "transform.h"
#include "window.h"
#include <stdio.h>

ecs_world_t* world;

v2 mouse;
f32 time;

const u32 screenWidth = 480;
const u32 screenHeight = 270;

typedef struct {
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
    setWindowFlags();
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    world = ecs_init();
    TransformImport();
    ECS_COMPONENT_DEFINE(world, Sprite);
    ecs_entity_t e = ecs_entity(world, {.name = "test"});
    ecs_set(world, e, Position, {50, 50});
    ecs_set(world, e, Velocity, {50, 50});
    ecs_set(world, e, Sprite,
            {.texture = LoadTexture("assets/images/player/playerDown.png"),
             .radius = 10,
             .drawSprite = true});
    ecs_add_id(world, e, _controllable);

    ColorRamp testRamp =
        createColorRampAuto((Color[]){GREEN, RED, BLUE}, 3, 255);

    Image noise = colorPerlin(64, testRamp);
    Texture2D tex = LoadTextureFromImage(noise);

    while (!WindowShouldClose()) {
        f32 scale = getWindowScale();
        mouse = getScreenMousePos(&mouse, scale, screenWidth, screenHeight);

        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            printf("pressed middle mouse button\n");
        }

        BeginTextureMode(target);
        ClearBackground(BLACK);

        render(world);
        ecs_progress(world, GetFrameTime());
        time += GetFrameTime();
        DrawTexture(tex, 20, 20, WHITE);

        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        drawScaledWindow(target, screenWidth, screenHeight, scale);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
