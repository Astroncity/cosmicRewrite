#include "planet.h"
#include "raylib.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

#define DARKEN(c, f) ((Color){(c.r * f), (c.g * f), (c.b * f), (c.a)})
#define DIST(x1, y1, x2, y2) (sqrtf(powf(x1 - x2, 2) + powf(y1 - y2, 2)))

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

Image averageImages(Image m1, Image m2) {
    assert(m1.width == m2.width && m1.height == m2.height &&
           "Images must be the same size");

    Image ret = GenImageColor(m1.width, m1.height, BLANK);
    Color* p1 = LoadImageColors(m1);
    Color* p2 = LoadImageColors(m2);

    for (usize i = 0; i < (usize)(m1.width * m1.height); i++) {
        i32 x = i % m1.width;
        i32 y = i / m1.width;

        Color c1 = p1[i];
        Color c2 = p2[i];

        Color c = (Color){(c1.r + c2.r) / 2, (c1.g + c2.g) / 2,
                          (c1.b + c2.b) / 2, (c1.a + c2.a) / 2};

        ImageDrawPixel(&ret, x, y, c);
    }

    UnloadImageColors(p1);
    UnloadImageColors(p2);
    return ret;
}

Image cropToCircle(Image img) {
    Color* p = LoadImageColors(img);
    Image ret = GenImageColor(img.width, img.height, BLANK);

    i32 circle = img.width / 2;

    for (usize i = 0; i < (usize)(img.width * img.height); i++) {
        i32 x = i % img.width;
        i32 y = i / img.width;

        if (sqrtf(powf(x - circle, 2) + powf(y - circle, 2)) < circle) {
            ImageDrawPixel(&ret, x, y, p[i]);
        }
    }

    UnloadImageColors(p);
    return ret;
}

Image dither(i32 circleOffsetx, i32 circleOffsety, Image m) {
    assert(m.width == m.height && "Image must be square");

    // darken pixels the futher they are from the shadow circle
    Image ret = GenImageColor(m.width, m.height, BLANK);
    Color* p = LoadImageColors(m);

    i32 cx = m.width / 2 + circleOffsetx;
    i32 cy = m.height / 2 + circleOffsety;

    for (usize i = 0; i < (usize)(m.width * m.height); i++) {
        i32 x = i % m.width;
        i32 y = i / m.width;

        i32 d = DIST(x, y, cx, cy) / 1.05;
        f32 f = 1 - (d / (f32)((f32)m.width / 2));
        f = f < 0 ? 0 : f;

        ImageDrawPixel(&ret, x, y, DARKEN(p[i], f));
    }

    UnloadImageColors(p);
    return ret;
}

Image colorPerlin(usize res, ColorRamp ramp) {
    i32 s = GetRandomValue(-100, 100);
    i32 s2 = GetRandomValue(-100, 100);

    Image noise1 = GenImagePerlinNoise(res, res, s, s * 2, 5);
    Image noise2 = GenImagePerlinNoise(res, res, s2, s2 * 2, 5 * 2);

    Image noise = averageImages(noise1, noise2);

    Color* noiseCl = LoadImageColors(noise);
    Image buf1 = GenImageColor(res, res, BLANK);

    for (usize i = 0; i < res * res; i++) {
        i32 x = i % res;
        i32 y = i / res;

        ImageDrawPixel(&buf1, x, y, getColorFromRamp(noiseCl[i].r, ramp));
    }

    UnloadImage(noise1);
    UnloadImage(noise2);
    UnloadImageColors(noiseCl);
    return buf1;
}
