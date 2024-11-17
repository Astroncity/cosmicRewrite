#pragma once
#include "raylib.h"
#include <stdint.h>
#include <stdlib.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef float f32;
typedef double f64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef size_t usize;
typedef Vector2 v2;
typedef Rectangle Rect;



#define GRUV_DARK0_HARD   (Color){29, 32, 33, 255}
#define GRUV_DARK0        (Color){40, 40, 40, 255}
#define GRUV_DARK0_SOFT   (Color){50, 48, 47, 255}
#define GRUV_DARK1        (Color){60, 56, 54, 255}
#define GRUV_DARK2        (Color){80, 73, 69, 255}
#define GRUV_DARK3        (Color){102, 92, 84, 255}
#define GRUV_DARK4        (Color){124, 111, 100, 255}

#define GRUV_GRAY         (Color){146, 131, 116, 255}

#define GRUV_LIGHT0_HARD  (Color){249, 245, 215, 255}
#define GRUV_LIGHT0       (Color){235, 219, 178, 255}
#define GRUV_LIGHT0_SOFT  (Color){213, 196, 161, 255}
#define GRUV_LIGHT1       (Color){189, 174, 147, 255}
#define GRUV_LIGHT2       (Color){168, 153, 132, 255}
#define GRUV_LIGHT3       (Color){146, 131, 116, 255}
#define GRUV_LIGHT4       (Color){124, 111, 100, 255}

#define GRUV_RED          (Color){251, 73, 52, 255}
#define GRUV_GREEN        (Color){184, 187, 38, 255}
#define GRUV_YELLOW       (Color){250, 189, 47, 255}
#define GRUV_BLUE         (Color){131, 165, 152, 255}
#define GRUV_PURPLE       (Color){211, 134, 155, 255}
#define GRUV_AQUA         (Color){142, 192, 124, 255}
#define GRUV_ORANGE       (Color){254, 128, 25, 255}
