#include "transform.h"
#include "state.h"
#include <raylib.h>

ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Velocity);

ECS_SYSTEM_DECLARE(Move);
ECS_SYSTEM_DECLARE(Controller);
ECS_TAG_DECLARE(_controllable);

void Move(ecs_iter_t* it) {
    Position* p = ecs_field(it, Position, 0);
    const Velocity* v = ecs_field(it, Velocity, 1);

    for (int i = 0; i < it->count; i++) {
        p[i].x += v[i].x * GetFrameTime();
        p[i].y += v[i].y * GetFrameTime();
    }
}

void Controller(ecs_iter_t* it) {
    Velocity* v = ecs_field(it, Velocity, 1);
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

void TransformImport() {
    ECS_MODULE(world, Transform);

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);
    ECS_SYSTEM_DEFINE(world, Move, EcsOnUpdate, Position, Velocity);
    ECS_TAG_DEFINE(world, _controllable);
    ECS_SYSTEM_DEFINE(world, Controller, EcsOnUpdate, _controllable,
                      Velocity);
}
