#pragma once

#include "defs.h"

#define MAG(v) sqrt(v.x* v.x + v.y * v.y)

v2 getScreenMousePos(v2* mouse, f32 scale, i32 sw, i32 sh);
void drawScaledWindow(RenderTexture2D target, f32 sw, f32 sh, f32 scale);
v2 v2Clamp(v2 vec, v2 min, v2 max);
void setWindowFlags(void);
f32 getWindowScale();
