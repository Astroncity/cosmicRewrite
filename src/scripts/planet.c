#include "planet.h"
#include "raylib.h"
#include <stdio.h>

#define PLANET_RES

ColorRamp createColorRampAuto(Color* colors, usize len, i32 max) {
    i32 step = max / len;
    ColorRamp r;

    for (usize i = 0; i < len - 1; i++) {
        r.steps[i] = (i + 1) * step;
    }
    for (usize i = 0; i < len; i++) {
        r.colors[i] = colors[i];
    }
    r.steps[len - 1] = max;
    return r;
}

ColorRamp createColorRamp(i32* steps, Color* colors, usize len) {
    ColorRamp r;
    for (usize i = 0; i < len; i++) {
        r.steps[i] = steps[i];
        r.colors[i] = colors[i];
    }
    r.len = len;
    return r;
}

// Function to interpolate between colors in the ColorRamp
Color getColorFromRamp(float t, ColorRamp ramp) {
    for (usize i = 0; i < ramp.len - 1; i++) {
        if (t <= ramp.steps[i]) return ramp.colors[i];
    }
    return ramp.colors[ramp.len - 1];
}

i32 tmpScale = 9;
Image colorPerlin(usize res, ColorRamp ramp) {
    i32 s = GetRandomValue(-100, 100);

    Image noise = GenImagePerlinNoise(res, res, s, s * 2, tmpScale);
    tmpScale--;
    Color* pixels = LoadImageColors(noise);
    Image blank = GenImageColor(res, res, BLANK);

    for (usize i = 0; i < res * res; i++) {
        i32 x = i % res;
        i32 y = i / res;

        ImageDrawPixel(&blank, x, y, getColorFromRamp(pixels[i].r, ramp));
    }

    // Create a new image from colored pixels
    UnloadImageColors(pixels);
    UnloadImage(noise);
    return blank;
}
