#include "planet.h"
#include "raylib.h"
#include <stdio.h>

#define PLANET_RES

ColorRamp createColorRampAuto(Color* colors, usize len, i32 max) {
    i32 step = max / len;
    i32 steps[len];

    for (usize i = 0; i < len - 1; i++) {
        steps[i] = (i + 1) * step;
    }
    steps[len - 1] = max;

    // TEST:
    for (usize i = 0; i < len; i++) {
        printf("%d\n", steps[i]);
    }

    return (ColorRamp){len, colors, steps};
}

ColorRamp createColorRamp(i32* steps, Color* colors, usize len) {
    return (ColorRamp){len, colors, steps};
}

// Function to interpolate between colors in the ColorRamp
Color getColorFromRamp(float t, ColorRamp ramp) {
    for (usize i = 0; i < ramp.len - 1; i++) {
        if (t <= ramp.steps[i]) return ramp.colors[i];
    }
    return ramp.colors[ramp.len - 1];
}

Image colorPerlin(usize res, ColorRamp ramp) {
    Image noise = GenImagePerlinNoise(res, res, 0, 0, 9);
    Color* pixels = LoadImageColors(noise);
    Image blank = GenImageColor(res, res, BLANK);

    for (usize i = 0; i < res * res; i++) {
        i32 x = i % res;
        i32 y = i / res;

        ImageDrawPixel(&blank, x, y, getColorFromRamp(pixels[i].r, ramp));
    }

    // Create a new image from colored pixels
    UnloadImageColors(pixels);
    return blank;
}
