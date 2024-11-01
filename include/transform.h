#pragma once
#include "defs.h"
#include "flecs.h"

typedef struct {
    f32 x;
    f32 y;
} position_c, velocity_c;

extern ECS_TAG_DECLARE(_controllable);
extern ECS_COMPONENT_DECLARE(velocity_c);
extern ECS_COMPONENT_DECLARE(position_c);
extern ECS_SYSTEM_DECLARE(Move);
extern ECS_SYSTEM_DECLARE(Controller);

void TransformModuleImport(ecs_world_t* world);
