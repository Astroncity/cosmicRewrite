#pragma once
#include "defs.h"

typedef struct {
    usize len;
    Color* colors;
    i32* steps;
} ColorRamp;

ColorRamp createColorRamp(i32* steps, Color* colors, usize len);
ColorRamp createColorRampAuto(Color* colors, usize len, i32 max);
Image colorPerlin(usize res, ColorRamp ramp);
