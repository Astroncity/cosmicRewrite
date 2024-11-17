#include "render.h"

ECS_COMPONENT_DECLARE(Renderable);
ECS_SYSTEM_DECLARE(renderSystem);

void render(ecs_iter_t* it) {

    const Renderable* s = ecs_field(it, Renderable, 0);

    for (int i = 0; i < it->count; i++) {
        s[i].render(it->entities[i]);
    }
}

int compareRenderable(ecs_entity_t e1, const void* ptr1, ecs_entity_t e2,
                      const void* ptr2) {
    (void)e1;
    (void)e2;
    const Renderable* r1 = ptr1;
    const Renderable* r2 = ptr2;

    return r1->renderLayer - r2->renderLayer;
}

void RendererModuleImport(ecs_world_t* world) {
    ECS_MODULE(world, RendererModule);
    ECS_COMPONENT_DEFINE(world, Renderable);

    ecs_entity_t render_s = ecs_system(
        world,
        {.entity = ecs_entity(world,
                              {.name = "renderSystem", // Name of the system
                               .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
         .query.terms =
             {
                 {.id = ecs_id(Renderable)} // Filter for entities with Renderable
             },
         .callback = render,
         .query.order_by = ecs_id(Renderable),
         .query.order_by_callback = compareRenderable});
    (void)render_s;
}
