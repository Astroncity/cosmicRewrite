#include "planet.h"
#include "raylib.h"
#include "state.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#define DARKEN(c, f) ((Color){(c.r * f), (c.g * f), (c.b * f), (c.a)})
#define DIST(x1, y1, x2, y2) (sqrtf(powf(x1 - x2, 2) + powf(y1 - y2, 2)))
#define PLANET_NAMES_PATH "assets/planet_names.txt"

ECS_COMPONENT_DECLARE(Planet);
ECS_COMPONENT_DECLARE(Clickable);
ECS_COMPONENT_DECLARE(Renderable);
ECS_SYSTEM_DECLARE(HandleClickables);

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

/**
 * Generates a Perlin noise-based image with colors applied from a
 * ColorRamp.
 *
 * @param res          The resolution of the generated image (width and
 * height).
 * @param ramp         The ColorRamp to use for coloring the Perlin noise.
 * @param customScale  The scale for Perlin noise; set to -1 to use the
 * default scale.
 *
 * @return An Image object generated based on the provided resolution, color
 * ramp, and scale.
 */
Image colorPerlin(usize res, ColorRamp ramp, f32 customScale) {
    i32 s = GetRandomValue(-100, 100);
    i32 s2 = GetRandomValue(-100, 100);

    f32 scaleBase = 5;
    scaleBase += GetRandomValue(-250, 500) / 100.0;

    if (customScale != -1) {
        scaleBase = customScale;
    }

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

void drawPlanetName(const Color avg, const v2* pos, const char* name,
                    f32 scale) {
    const i32 spacing = 1;
    i32 len = MeasureTextEx(globalFont, name, PLANET_NAME_SIZE, spacing).x;
    v2 center = {pos->x + PLANET_RES * scale / 2.0,
                 pos->y + PLANET_RES * scale / 2.0};
    v2 textPos = {center.x - len / 2.0,
                  center.y - PLANET_RES * scale / 2.0 - 30};

    DrawTextEx(globalFont, name, textPos, PLANET_NAME_SIZE, spacing, avg);
}

void planetRender(ecs_entity_t e) {
    const Planet* p = ecs_get(world, e, Planet);
    const Position* pos = ecs_get(world, e, Position);

    DrawTextureEx(p->land, (v2){pos->x, pos->y}, 0, p->scale, WHITE);
    DrawTextureEx(p->atmosphere,
                  (v2){pos->x - p->atmosphereOffset * (p->scale / 2.0),
                       pos->y - p->atmosphereOffset * (p->scale / 2.0)},
                  0, p->scale, WHITE);
    drawColorRamp(&p->palette);
    drawPlanetName(p->avg, &(v2){pos->x, pos->y}, p->name, p->scale);
}

char* getPlanetName() {
    static long pos = 0;
    FILE* file = fopen(PLANET_NAMES_PATH, "r");
    if (file == NULL) {
        perror("Error reading file");
        return NULL;
    }

    char* name = malloc(sizeof(char) * PLANET_NAME_MAXLEN);
    if (name == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    fseek(file, pos, SEEK_SET); // Go to the last known position
    if (fgets(name, PLANET_NAME_MAXLEN, file) != NULL) {
        printf("%s\n", name);
        pos = ftell(file); // Update position for the next call
    } else {
        perror("Error reading name or end of file reached");
        free(name);
        name = NULL;
    }

    fclose(file); // Close the file each time
    return name;
}

void onPlanetHover(ecs_entity_t e) {
    const Planet* p = ecs_get(world, e, Planet);
    const Position* pos = ecs_get(world, e, Position);

    v2 center = {pos->x + PLANET_RES * p->scale / 2.0,
                 pos->y + PLANET_RES * p->scale / 2.0};
    f32 rad = (PLANET_RES * ATMOSPHERE_SCALE * p->scale) / 2 + 1;
    DrawCircleLines(center.x, center.y, rad, GRUV_BLUE);
}

void onPlanetExitHover(ecs_entity_t e) {
    (void)e;
    return;
}

ecs_entity_t createPlanet(v2 pos, f32 scale) {
    Color* cls = generateHarmonizedColors(brightenColor(getRandomColor()),
                                          6, 25, 1, 1);
    ColorRamp ramp = createColorRampAuto(cls, 6, 255);

    Color atmColor = brightenColor(averageRamp(&ramp));
    atmColor.a = GetRandomValue(100, 200); // atmosphere density

    // terrain noise
    Image noiseSq = colorPerlin(PLANET_RES, ramp, -1);
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

    // name gen
    char* name = getPlanetName();
    if (name == NULL) {
        name = "NAME ERROR";
    }

    if (!strcmp(name, "NAME ERROR")) {
        free(name);
    }

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Planet,
            {.land = tex,
             .atmosphere = atm,
             .palette = ramp,
             .atmosphereOffset = atmosphereOffset,
             .avg = atmColor,
             .scale = scale});

    strncpy(ecs_get_mut(world, e, Planet)->name, name, PLANET_NAME_MAXLEN);
    ecs_set(world, e, Position, {pos.x, pos.y});
    ecs_set(world, e, Renderable, {planetRender});
    ecs_set(world, e, Clickable,
            {onPlanetHover,
             onPlanetHover,
             onPlanetExitHover,
             {PLANET_RES * scale, PLANET_RES * scale}});

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

void HandleClickables(ecs_iter_t* it) {
    const Clickable* c = ecs_field(it, Clickable, 1);
    const Position* p = ecs_field(it, Position, 0);

    for (int i = 0; i < it->count; i++) {
        Rect box = {p[i].x, p[i].y, c[i].hitbox.x, c[i].hitbox.y};

        if (CheckCollisionPointRec(*mouse, box)) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                c[i].onClick(it->entities[i]);
            } else {
                c[i].onHover(it->entities[i]);
            }
        } else {
            c[i].hoverReset(it->entities[i]);
        }
    }
}

Texture2D genCosmicBackground() {
    ColorRamp cosmicRamp = createColorRampAuto(
        (Color[]){
            (Color){2, 2, 5, 255},    // Deep Space (Almost Black)
            (Color){8, 8, 20, 255},   // Subtle Nebula Glow (Very Dark Blue)
            (Color){15, 10, 20, 255}, // Faint Star Glow (Dark Purple)
            (Color){6, 11, 14, 255},  // Dim Nebula (Muted Violet)
            (Color){18, 10, 2, 255},  // Faded Crimson Star (Dark Crimson)
            (Color){2, 2, 9, 255} // Subdued Starlight (Soft Grayish Gold)
        },
        6, 255);
    Image colored = colorPerlin(screenWidth, cosmicRamp, 30);
    return LoadTextureFromImage(colored);
}

void PlanetModuleImport(ecs_world_t* world) {
    ECS_IMPORT(world, TransformModule);
    ECS_MODULE(world, PlanetModule);

    ECS_COMPONENT_DEFINE(world, Planet);
    ECS_COMPONENT_DEFINE(world, Clickable);
    ECS_COMPONENT_DEFINE(world, Renderable);
    ECS_SYSTEM_DEFINE(world, HandleClickables, EcsOnUpdate,
                      transform.module.Position, Clickable);
}
