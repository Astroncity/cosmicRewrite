#include "flecs.h"
#include "planet.h"
#include "state.h"
#include "transform.h"
#include "uiFramework.h"
#include "window.h"
#include <raylib.h>
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

int compareRenderable(ecs_entity_t e1, const void* ptr1, ecs_entity_t e2,
                      const void* ptr2) {
    (void)e1;
    (void)e2;
    const Renderable* r1 = ptr1;
    const Renderable* r2 = ptr2;

    return r1->renderLayer - r2->renderLayer;
}

// TEST:
Texture2D playerTex;
void renderPlayer(ecs_entity_t e) {
    const position_c* pos = ecs_get(world, e, position_c);
    DrawTexture(playerTex, pos->x, pos->y, WHITE);
}

int main(void) {
    setWindowFlags();
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    globalFont = LoadFontEx("assets/fonts/spaceMono.ttf", 256, 0, 0);

    planetTest();

    world = ecs_init();
    ECS_IMPORT(world, TransformModule);
    ECS_IMPORT(world, PlanetModule);
    ECS_IMPORT(world, UIModule);

    ecs_entity_t e = ecs_entity(world, {.name = "test"});
    ecs_set(world, e, position_c, {50, 50});
    ecs_set(world, e, velocity_c, {50, 50});
    ecs_set(world, e, Renderable, {2, renderPlayer});

    playerTex = LoadTexture("assets/images/player/playerDown.png");
    ecs_add_id(world, e, _controllable);

    ecs_query_t* q = ecs_query(
        world, {.terms = {{.id = ecs_id(position_c)},
                          {.id = ecs_id(Renderable), .inout = EcsIn}},
                .cache_kind = EcsQueryCacheAuto,
                .order_by = ecs_id(Renderable),
                .order_by_callback = compareRenderable});

    mouse = malloc(sizeof(v2));
    // const f32 scale = 1.5;

    /*createPlanet((v2){screenWidth / 2.0 - PLANET_RES * scale / 2,
                      screenHeight / 2.0 - PLANET_RES * scale / 2 + 20},
                 scale);*/

    Texture2D background = genCosmicBackground();

    textbox_e testBox = createTextbox((v2){10, 20});
    ecs_entity_t l1 = TextboxPush(
        testBox, "test", LoadTexture("assets/images/testIcon.png"));
    label_c* l1_c = ecs_get_mut(world, l1, label_c);

    ecs_entity_t testContainer = createPlanetContainer(5);
    bool done = true;
    bool lastDir = false;

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

        if (IsKeyPressed(KEY_RIGHT) && done) {
            scrollPlanet(testContainer, false, true, &done);
            lastDir = false;
        } else if (IsKeyPressed(KEY_LEFT) && done) {
            scrollPlanet(testContainer, true, true, &done);
            lastDir = true;
        }

        if (!done) {
            scrollPlanet(testContainer, lastDir, false, &done);
        }
        // set label to true false based on if the planet is at the end
        l1_c->text = done ? "true" : "false";

        EndTextureMode();
        BeginDrawing();
        ClearBackground(BLACK);
        drawScaledWindow(target, screenWidth, screenHeight, scale);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
