#pragma once
#include "defs.h"
#define MAX_COLORRAMP_STEPS 10
#define PLANET_RES 128

typedef struct {
    usize len;
    Color colors[MAX_COLORRAMP_STEPS];
    i32 steps[MAX_COLORRAMP_STEPS];
} ColorRamp;

ColorRamp createColorRamp(i32* steps, Color* colors, usize len);
ColorRamp createColorRampAuto(Color* colors, usize len, i32 max);
Image colorPerlin(usize res, ColorRamp ramp);

Image dither(i32 circleOffsetx, i32 circleOffsety, Image m);
Image cropToCircle(Image img);
Color* generateHarmonizedColors(Color baseColor, int colorCount,
                                int hueShift, float saturationFactor,
                                float brightnessFactor);

void planetTest();
Color getRandomColor();

void drawColorRamp(ColorRamp* ramp);
Color brightenColor(Color c);

Color averageRamp(ColorRamp* ramp);
