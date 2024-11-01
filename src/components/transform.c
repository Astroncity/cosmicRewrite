#include "transform.h"
#include "raylib.h"

ECS_COMPONENT_DECLARE(position_c);
ECS_COMPONENT_DECLARE(velocity_c);

ECS_SYSTEM_DECLARE(Move);
ECS_SYSTEM_DECLARE(Controller);
ECS_TAG_DECLARE(_controllable);

void Move(ecs_iter_t* it) {
    position_c* p = ecs_field(it, position_c, 0);
    const velocity_c* v = ecs_field(it, velocity_c, 1);

    for (int i = 0; i < it->count; i++) {
        p[i].x += v[i].x * GetFrameTime();
        p[i].y += v[i].y * GetFrameTime();
    }
}

void Controller(ecs_iter_t* it) {
    velocity_c* v = ecs_field(it, velocity_c, 1);
    f32 speed = 100;

    for (int i = 0; i < it->count; i++) {
        v[i].x = 0;
        v[i].y = 0;
        f32 delta = 1;

        if (IsKeyDown(KEY_W)) {
            v[i].y -= speed * delta;
        }
        if (IsKeyDown(KEY_S)) {
            v[i].y += speed * delta;
        }
        if (IsKeyDown(KEY_A)) {
            v[i].x -= speed * delta;
        }
        if (IsKeyDown(KEY_D)) {
            v[i].x += speed * delta;
        }
    }
}

void TransformModuleImport(ecs_world_t* world) {
    ECS_MODULE(world, TransformModule);
    ECS_COMPONENT_DEFINE(world, position_c);
    ECS_COMPONENT_DEFINE(world, velocity_c);
    ECS_SYSTEM_DEFINE(world, Move, EcsOnUpdate, position_c, velocity_c);
    ECS_TAG_DEFINE(world, _controllable);
    ECS_SYSTEM_DEFINE(world, Controller, EcsOnUpdate, _controllable, velocity_c);
}
