#pragma once
#include "defs.h"
#include "flecs.h"
#include "planet.h"

extern ecs_world_t* world;
extern const u32 screenWidth;
extern const u32 screenHeight;
extern Font globalFont;
extern v2* mouse;
extern const Planet* selectedPlanet_p;

enum GameState {
    MAIN_MENU,
    PLANET_SELECT,
    GAME,
    PAUSE,
    GAME_OVER,
};
