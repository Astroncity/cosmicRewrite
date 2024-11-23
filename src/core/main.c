#include "flecs.h"
#include "planet.h"
#include "render.h"
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
const Planet* selectedPlanet_p;

/* const u32 screenWidth = 480; */
/* const u32 screenHeight = 270; */

const u32 screenWidth = 640;
const u32 screenHeight = 360;

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

    globalFont = LoadFontEx("assets/fonts/spaceMono.ttf", 64, 0, 0);

    planetTest();

    world = ecs_init();
    ECS_IMPORT(world, TransformModule);
    ECS_IMPORT(world, RendererModule);
    ECS_IMPORT(world, PlanetModule);
    ECS_IMPORT(world, UIModule);
    playerTex = LoadTexture("assets/images/player/playerDown.png");

    mouse = malloc(sizeof(v2));
    Texture2D background = genCosmicBackground();

    textbox_e testBox =
        createTextbox("Planet Information", (v2){10, 20},
                      (v2){screenWidth / 2.0 - PLANET_RES * 1.5 / 2.0, 200});
    const char* pthSm = "assets/images/testIconSmall.png";

    TextboxPush(testBox, "DANGER", 16, LoadTexture(pthSm));
    TextboxPush(testBox, "ATMOSPHERE", 16, LoadTexture(pthSm));
    TextboxPush(testBox, "TERRAIN", 16, LoadTexture(pthSm));

    ecs_entity_t testContainer = createPlanetContainer(2);
    bool done = true;
    bool lastDir = false;
    Texture2D lastText;

    while (!WindowShouldClose()) {
        f32 scale = getWindowScale();
        *mouse = getScreenMousePos(mouse, scale, screenWidth, screenHeight);

        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            printf("pressed middle mouse button\n");
        }

        BeginTextureMode(target);
        ClearBackground(BLACK);

        if (selectedPlanet_p == NULL) {
            DrawTextureEx(background, (v2){0, 0}, 0, 1, WHITE);
        } else {
            if (lastText.id != selectedPlanet_p->background.id) {
                printf("changed\n");
            }
            DrawTextureEx(selectedPlanet_p->background, (v2){0, 0}, 0, 1, WHITE);
            lastText = selectedPlanet_p->background;
        }

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

        EndTextureMode();
        BeginDrawing();
        ClearBackground(BLACK);
        drawScaledWindow(target, screenWidth, screenHeight, scale);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
