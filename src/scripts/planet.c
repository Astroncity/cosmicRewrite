#include "planet.h"
#include "raylib.h"
#include "state.h"
#include "transform.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

#define DARKEN(c, f) ((Color){(c.r * f), (c.g * f), (c.b * f), (c.a)})
#define DIST(x1, y1, x2, y2) (sqrtf(powf(x1 - x2, 2) + powf(y1 - y2, 2)))

ECS_COMPONENT_DECLARE(Planet);
ECS_COMPONENT_DECLARE(Clickable);
ECS_COMPONENT_DECLARE(Renderable);

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
    r.len = len;
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

Color HSVtoRGB(int h, int s, int v) {
    f32 r, g, b;
    f32 f, p, q, t;

    f32 S = s / 100.0;
    f32 V = v / 100.0;

    if (s == 0) {
        r = g = b = V;
    } else {
        f = (h % 60) / 60.0;
        p = V * (1 - S);
        q = V * (1 - S * f);
        t = V * (1 - S * (1 - f));

        switch (h / 60) {
        case 0:
            r = V;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = V;
            b = p;
            break;
        case 2:
            r = p;
            g = V;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = V;
            break;
        case 4:
            r = t;
            g = p;
            b = V;
            break;
        case 5:
            r = V;
            g = p;
            b = q;
            break;
        }
    }

    return (Color){(u8)(r * 255), (u8)(g * 255), (u8)(b * 255), 255};
}

void RGBtoHSV(Color c, i32* h, i32* s, i32* v) {
    f32 r = c.r / 255.0;
    f32 g = c.g / 255.0;
    f32 b = c.b / 255.0;

    f32 max = fmax(r, fmax(g, b));
    f32 min = fmin(r, fmin(g, b));
    f32 d = max - min;

    if (d == 0) {
        *h = 0;
    } else if (max == r) {
        *h = 60 * fmod((g - b) / d, 6);
        if (*h < 0) *h += 360;
    } else if (max == g) {
        *h = 60 * ((b - r) / d + 2);
    } else if (max == b) {
        *h = 60 * ((r - g) / d + 4);
    }

    *s = (max == 0) ? 0 : (d / max) * 100;
    *v = max * 100;
}

Color* generateHarmonizedColors(Color baseColor, int colorCount,
                                int hueShift, float saturationFactor,
                                float brightnessFactor) {
    Color* colors = (Color*)malloc(sizeof(Color) * colorCount);
    i32 hue, saturation, brightness;

    // Convert the base color to HSV
    RGBtoHSV(baseColor, &hue, &saturation, &brightness);

    // Generate colors by adjusting the hue, saturation, and brightness
    for (int i = 0; i < colorCount; i++) {
        int newHue = (hue + i * hueShift) % 360;
        int newSaturation = (int)(saturation * saturationFactor);
        int newBrightness = (int)(brightness * brightnessFactor);

        // Store the new color in the array
        colors[i] = HSVtoRGB(newHue, newSaturation, newBrightness);
    }

    return colors;
}

Image colorPerlin(usize res, ColorRamp ramp) {
    i32 s = GetRandomValue(-100, 100);
    i32 s2 = GetRandomValue(-100, 100);

    f32 scaleBase = 5;
    scaleBase += GetRandomValue(-250, 500) / 100.0;

    Image noise1 = GenImagePerlinNoise(res, res, s, s * 2, scaleBase);
    Image noise2 = GenImagePerlinNoise(res, res, s2, s2 * 2, scaleBase * 2);

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

void printTestRes(const char* txt, bool passed) {
    // use ansi escape codes to color the output
    if (passed) {
        printf("\033[1;32m[PASSED]");
        printf("\033[0m");
        printf(" -- %s\n", txt);
    } else {
        printf("\033[1;31m[FAILED]");
        printf("\033[0m");
        printf(" -- %s\n", txt);
    }
}

Color averageRamp(const ColorRamp* ramp) {
    i32 rs = 0;
    i32 gs = 0;
    i32 bs = 0;

    for (usize i = 0; i < ramp->len; i++) {
        Color c = ramp->colors[i];
        rs += c.r;
        gs += c.g;
        bs += c.b;
    }

    return (Color){rs / ramp->len, gs / ramp->len, bs / ramp->len, 255};
}

void drawColorRamp(const ColorRamp* ramp) {
    for (usize i = 0; i < ramp->len; i++) {
        Color c = ramp->colors[i];
        DrawRectangle(i * 10 + 10, 0, 10, 10, c);
    }

    DrawRectangle(0, 10, 10, 10, averageRamp(ramp));
}

void planetTest() {
    printf("\033[1;33m--------[RUNNING TESTS]--------]\n");
    Color a = RED;
    i32 h, s, v;
    RGBtoHSV(a, &h, &s, &v);
    Color b = HSVtoRGB(h, s, v);

    bool passed = (abs(a.r - b.r) < 5) && (abs(a.g - b.g) < 5) &&
                  (abs(a.b - b.b) < 5);

    printTestRes("RGB to HSV to RGB", passed);
}

Color brightenColor(Color c) {
    i32 max = fmax(c.r, fmax(c.g, c.b));
    i32 diff = 255 - max;
    return (Color){c.r + diff, c.g + diff, c.b + diff, c.a};
}

Color getRandomColor() {
    return (Color){GetRandomValue(0, 255), GetRandomValue(0, 255),
                   GetRandomValue(0, 255), 255};
}

void planetRender(ecs_entity_t e) {
    const Planet* p = ecs_get(world, e, Planet);
    const Position* pos = ecs_get(world, e, Position);

    DrawTextureEx(p->land, (v2){pos->x, pos->y}, 0, 2, WHITE);
    DrawTextureEx(
        p->atmosphere,
        (v2){pos->x - p->atmosphereOffset, pos->y - p->atmosphereOffset}, 0,
        2, WHITE);
    drawColorRamp(&p->palette);
}

ecs_entity_t createPlanet(v2 pos) {
    Color* cls = generateHarmonizedColors(brightenColor(getRandomColor()),
                                          6, 25, 1, 1);
    ColorRamp ramp = createColorRampAuto(cls, 6, 255);

    Color atmColor = brightenColor(averageRamp(&ramp));
    atmColor.a = GetRandomValue(100, 200); // atmosphere density

    // terrain noise
    Image noiseSq = colorPerlin(PLANET_RES, ramp);
    Image noiseShadow = dither(0, -PLANET_RES / 8, noiseSq);
    Image noise = cropToCircle(noiseShadow);
    Texture2D tex = LoadTextureFromImage(noise);

    // atmosphere
    Image atmSolid = GenImageColor(PLANET_RES * ATMOSPHERE_SCALE,
                                   PLANET_RES * ATMOSPHERE_SCALE, atmColor);
    Image atmShadow = dither(0, -PLANET_RES / 8, atmSolid);
    Image atmC = cropToCircle(atmShadow);
    Texture2D atm = LoadTextureFromImage(atmC);

    i32 atmosphereOffset = (PLANET_RES * ATMOSPHERE_SCALE - PLANET_RES);

    ecs_entity_t e = ecs_entity(world, {.name = "planet"});
    ecs_set(world, e, Planet,
            {.land = tex,
             .atmosphere = atm,
             .palette = ramp,
             .atmosphereOffset = atmosphereOffset});

    ecs_set(world, e, Position, {pos.x, pos.y});
    ecs_set(world, e, Renderable, {planetRender});

    // cleanup
    UnloadImage(noiseSq);
    UnloadImage(noiseShadow);
    UnloadImage(noise);
    UnloadImage(atmSolid);
    UnloadImage(atmShadow);
    UnloadImage(atmC);
    free(cls);
    cls = NULL;

    return e;
}

void PlanetModuleImport() {
    ECS_MODULE(world, PlanetModule);

    ECS_COMPONENT_DEFINE(world, Planet);
    ECS_COMPONENT_DEFINE(world, Clickable);
    ECS_COMPONENT_DEFINE(world, Renderable);
}
