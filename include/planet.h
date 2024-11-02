#pragma once
#include "defs.h"
#include "flecs.h"

#define MAX_COLORRAMP_STEPS 10
#define PLANET_RES 128
#define ATMOSPHERE_SCALE 1.05
#define PLANET_NAME_MAXLEN 32
#define PLANET_NAME_SIZE 22
#define PLANET_MAX_SCROLLABLE 10

extern ECS_COMPONENT_DECLARE(Planet);
extern ECS_TAG_DECLARE(_scrollablePlanet);
extern ECS_COMPONENT_DECLARE(Renderable);
extern ECS_COMPONENT_DECLARE(Clickable);
extern ECS_SYSTEM_DECLARE(HandleClickables);

typedef struct {
    usize len;
    Color colors[MAX_COLORRAMP_STEPS];
    i32 steps[MAX_COLORRAMP_STEPS];
} ColorRamp;

typedef struct {
    u32 renderLayer;
    void (*render)(ecs_entity_t e);
} Renderable;

typedef struct {
    void (*onClick)(ecs_entity_t e);
    void (*onHover)(ecs_entity_t e);
    void (*hoverReset)(ecs_entity_t e);
    v2 hitbox;
} Clickable;

typedef struct {
    Texture2D land;
    Texture2D atmosphere;
    ColorRamp palette;
    Color avg;
    i32 atmosphereOffset;
    u8 order;
    f32 scale;
    char name[PLANET_NAME_MAXLEN];
} Planet;

ColorRamp createColorRamp(i32* steps, Color* colors, usize len);
ColorRamp createColorRampAuto(Color* colors, usize len, i32 max);
Image colorPerlin(usize res, ColorRamp ramp, f32 scale);

Image dither(i32 circleOffsetx, i32 circleOffsety, Image m);
Image cropToCircle(Image img);
Color* generateHarmonizedColors(Color baseColor, i32 colorCount,
                                i32 hueShift, f32 saturationFactor,
                                f32 brightnessFactor);

void planetTest();
Color getRandomColor();

void drawColorRamp(const ColorRamp* ramp);
Color brightenColor(Color c);

Color averageRamp(const ColorRamp* ramp);
ecs_entity_t createPlanet(v2 pos, f32 scale);
ecs_entity_t createPlanetContainer(i32 count);
void scrollPlanet(ecs_entity_t container, bool direction, bool increase,
                  bool* done);

void PlanetModuleImport(ecs_world_t* world);

Texture2D genCosmicBackground();
