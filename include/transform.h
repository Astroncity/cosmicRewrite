#pragma once
#include "defs.h"
#include "flecs.h"

typedef struct {
    f32 x;
    f32 y;
} Position, Velocity;

extern ECS_TAG_DECLARE(_controllable);
extern ECS_COMPONENT_DECLARE(Velocity);
extern ECS_COMPONENT_DECLARE(Position);
extern ECS_SYSTEM_DECLARE(Move);
extern ECS_SYSTEM_DECLARE(Controller);

void TransformImport();
