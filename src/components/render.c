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
    // ECS_SYSTEM_DEFINE(world, render_s, EcsOnUpdate, Renderable);

    ecs_entity_t render_s = ecs_system_init(
        world,
        &(ecs_system_desc_t){
            .entity = ecs_entity(world,
                                 {.name = "render_s", // Name of the system
                                  .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
            .query.terms =
                {
                    {.id = ecs_id(Renderable)} // Filter for entities with Renderable
                },
            .callback = render_s, // Your system function
            .quer.order_by
        });
}
