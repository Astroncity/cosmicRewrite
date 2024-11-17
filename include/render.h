#pragma once
#include "defs.h"
#include "flecs.h"

extern ECS_COMPONENT_DECLARE(Renderable);
extern ECS_SYSTEM_DECLARE(render_s);

typedef struct {
    u32 renderLayer;
    void (*render)(ecs_entity_t e);
} Renderable;

void RendererModuleImport(ecs_world_t* world);
