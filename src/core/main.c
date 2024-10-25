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

void render(ecs_world_t* world, ecs_query_t* q) {

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

    // ColorRamp testRamp = createColorRampAuto(
    //    (Color[]){GREEN, RED, BLUE, SKYBLUE, MAGENTA, YELLOW}, 6, 255);

    ColorRamp terrainRamp = createColorRampAuto(
        (Color[]){
            (Color){0, 0, 139, 255},    // Deep Water (Royal Blue)
            (Color){0, 191, 255, 255},  // Shallow Water (Sky Blue)
            (Color){60, 179, 113, 255}, // Grasslands (Medium Sea Green)
            (Color){124, 252, 0, 255},  // Lush Hills (Lawn Green)
            (Color){205, 133, 63, 255}, // Rocky Mountains (Peru)
            (Color){255, 255, 240, 255} // Snowy Peaks (Ivory White)
        },
        6, 255);

    Image noiseSq = colorPerlin(PLANET_RES, terrainRamp);
    Image noiseD = dither(0, -PLANET_RES / 8, noiseSq);
    Image noise = cropToCircle(noiseD);
    Texture2D tex = LoadTextureFromImage(noise);

    Color atc = (Color){7, 217, 255, 100};
    const f32 atmScale = 1.05;

    Image atmS =
        GenImageColor(PLANET_RES * atmScale, PLANET_RES * atmScale, atc);
    Image atmD = dither(0, -PLANET_RES / 8, atmS);
    Image atmI = cropToCircle(atmD);
    Texture2D atm = LoadTextureFromImage(atmI);

    i32 atmo = (PLANET_RES * atmScale - PLANET_RES) / 2;

    ecs_query_t* q =
        ecs_query(world, {.terms = {{.id = ecs_id(Position)},
                                    {.id = ecs_id(Sprite), .inout = EcsIn}},
                          .cache_kind = EcsQueryCacheAuto});

    while (!WindowShouldClose()) {
        f32 scale = getWindowScale();
        mouse = getScreenMousePos(&mouse, scale, screenWidth, screenHeight);

        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            printf("pressed middle mouse button\n");
        }

        BeginTextureMode(target);
        ClearBackground(BLACK);

        render(world, q);
        ecs_progress(world, GetFrameTime());
        time += GetFrameTime();
        DrawTexture(tex, 150, 20, WHITE);
        DrawTexture(atm, 150 - atmo, 20 - atmo, WHITE);

        EndTextureMode();

        if (IsKeyPressed(KEY_SPACE)) {
            UnloadTexture(tex);
            UnloadImage(noise);
            UnloadImage(noiseSq);
            UnloadImage(noiseD);
            noiseSq = colorPerlin(PLANET_RES, terrainRamp);
            noiseD = dither(0, -PLANET_RES / 8, noiseSq);
            noise = cropToCircle(noiseD);
            tex = LoadTextureFromImage(noise);
        }

        BeginDrawing();
        ClearBackground(BLACK);
        drawScaledWindow(target, screenWidth, screenHeight, scale);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
