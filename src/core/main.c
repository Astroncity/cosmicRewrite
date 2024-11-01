#include "flecs.h"
#include "planet.h"
#include "raylib.h"
#include "state.h"
#include "transform.h"
#include "window.h"
#include <stdio.h>

ecs_world_t* world;

v2* mouse;
f32 time;
Font globalFont;

const u32 screenWidth = 480;
const u32 screenHeight = 270;

void render(ecs_world_t* world, ecs_query_t* q) {

    ecs_iter_t it = ecs_query_iter(world, q);

    while (ecs_query_next(&it)) {
        const Renderable* s = ecs_field(&it, Renderable, 1);

        for (int i = 0; i < it.count; i++) {
            s[i].render(it.entities[i]);
        }
    }
}

// TEST:
Texture2D playerTex;
void renderPlayer(ecs_entity_t e) {
    const Position* pos = ecs_get(world, e, Position);
    DrawTexture(playerTex, pos->x, pos->y, WHITE);
}

int main(void) {
    setWindowFlags();
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    globalFont = LoadFont("assets/fonts/spaceMono.ttf");

    planetTest();

    world = ecs_init();
    ECS_IMPORT(world, TransformModule);
    ECS_IMPORT(world, PlanetModule);

    ecs_entity_t e = ecs_entity(world, {.name = "test"});
    ecs_set(world, e, Position, {50, 50});
    ecs_set(world, e, Velocity, {50, 50});
    ecs_set(world, e, Renderable, {renderPlayer});

    playerTex = LoadTexture("assets/images/player/playerDown.png");
    ecs_add_id(world, e, _controllable);

    ecs_query_t* q = ecs_query(
        world, {.terms = {{.id = ecs_id(Position)},
                          {.id = ecs_id(Renderable), .inout = EcsIn}},
                .cache_kind = EcsQueryCacheAuto});

    mouse = malloc(sizeof(v2));
    const f32 scale = 1.5;

    createPlanet((v2){screenWidth / 2.0 - PLANET_RES * scale / 2,
                      screenHeight / 2.0 - PLANET_RES * scale / 2 + 20},
                 scale);

    Texture2D background = genCosmicBackground();

    while (!WindowShouldClose()) {
        f32 scale = getWindowScale();
        *mouse = getScreenMousePos(mouse, scale, screenWidth, screenHeight);

        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            printf("pressed middle mouse button\n");
        }

        BeginTextureMode(target);
        ClearBackground(BLACK);
        DrawTextureEx(background, (v2){0, 0}, 0, 1, WHITE);

        render(world, q);
        ecs_progress(world, GetFrameTime());
        time += GetFrameTime();

        EndTextureMode();
        BeginDrawing();
        ClearBackground(BLACK);
        drawScaledWindow(target, screenWidth, screenHeight, scale);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
