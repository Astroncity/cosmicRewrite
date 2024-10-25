#pragma once
#include "defs.h"
#define MAX_COLORRAMP_STEPS 10

typedef struct {
    usize len;
    Color colors[MAX_COLORRAMP_STEPS];
    i32 steps[MAX_COLORRAMP_STEPS];
} ColorRamp;

ColorRamp createColorRamp(i32* steps, Color* colors, usize len);
ColorRamp createColorRampAuto(Color* colors, usize len, i32 max);
Image colorPerlin(usize res, ColorRamp ramp);
