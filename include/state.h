#pragma once
#include "defs.h"
#include "flecs.h"

extern ecs_world_t* world;
extern const u32 screenWidth;
extern const u32 screenHeight;
extern Font globalFont;
extern v2* mouse;

enum GameState {
    MAIN_MENU,
    PLANET_SELECT,
    GAME,
    PAUSE,
    GAME_OVER,
};
