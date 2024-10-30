#pragma once
#include "defs.h"
#include "flecs.h"
#define MAX_COLORRAMP_STEPS 10
#define PLANET_RES 128
#define ATMOSPHERE_SCALE 1.05

typedef struct {
    usize len;
    Color colors[MAX_COLORRAMP_STEPS];
    i32 steps[MAX_COLORRAMP_STEPS];
} ColorRamp;

typedef struct {
    void (*render)(ecs_entity_t e);
} Renderable;

extern ECS_COMPONENT_DECLARE(Renderable);

typedef struct {
    void (*callback)(ecs_entity_t);
} Clickable;
extern ECS_COMPONENT_DECLARE(Clickable);

typedef struct {
    Texture2D land;
    Texture2D atmosphere;
    ColorRamp palette;
    i32 atmosphereOffset;
} Planet;
extern ECS_COMPONENT_DECLARE(Planet);

ColorRamp createColorRamp(i32* steps, Color* colors, usize len);
ColorRamp createColorRampAuto(Color* colors, usize len, i32 max);
Image colorPerlin(usize res, ColorRamp ramp);

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
ecs_entity_t createPlanet(v2 pos);

void PlanetModuleImport();
