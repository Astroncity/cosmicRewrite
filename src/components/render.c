#include "render.h"

ECS_COMPONENT_DECLARE(Renderable);
ECS_SYSTEM_DECLARE(render_s);

void render_s(ecs_iter_t* it) {

    const Renderable* s = ecs_field(it, Renderable, 0);

    for (int i = 0; i < it->count; i++) {
        s[i].render(it->entities[i]);
    }
}

void RendererModuleImport(ecs_world_t* world) {
    ECS_MODULE(world, RendererModule);
    ECS_COMPONENT_DEFINE(world, Renderable);
    ECS_SYSTEM_DEFINE(world, render_s, EcsOnUpdate, Renderable);
}
