#include "defs.hpp"
#include <algorithm>
#include <any>
#include <cstdio>
#include <math.h>
#include <optional>
#include <raylib.h>
#include <typeindex>
#include <unordered_map>

const u32 screenWidth = 480;
const u32 screenHeight = 270;

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define vec(x) ComponentVec<x>&
#define MAG(v) sqrt(v.x* v.x + v.y * v.y)
#define _generic template <typename T>

v2 mouse;

v2 v2Clamp(v2 vec, v2 min, v2 max) {
    return (v2){MIN(MAX(vec.x, min.x), max.x),
                MIN(MAX(vec.y, min.y), max.y)};
}

v2 getScreenMousePos(v2* mouse, f32 scale, i32 sw, i32 sh) {
    v2 mouseOLD = GetMousePosition();
    mouse->x =
        (mouseOLD.x - (GetScreenWidth() - (sw * scale)) * 0.5f) / scale;
    mouse->y =
        (mouseOLD.y - (GetScreenHeight() - (sh * scale)) * 0.5f) / scale;
    *mouse = v2Clamp(*mouse, (v2){0, 0}, (v2){(f32)sw, (f32)sh});

    return *mouse;
}

void drawScaledWindow(RenderTexture2D target, f32 sw, f32 sh, f32 scale) {
    f32 tw = (f32)target.texture.width;
    f32 th = (f32)target.texture.height;
    Rect rect1 = {0.0f, 0.0f, tw, -th};
    f32 x = (GetScreenWidth() - (sw * scale)) * 0.5f;
    f32 y = (GetScreenHeight() - (sh * scale)) * 0.5f;

    Rect rect2 = {x, y, sw * scale, sh * scale};

    DrawTexturePro(target.texture, rect1, rect2, (v2){0, 0}, 0.0f, WHITE);
}

using Entity = u32;

_generic class ComponentVec {
  public:
    void addComponent(Entity entity, T component) {
        entityToIndex[entity] = comp.size();
        comp.push_back(std::move(component));
        indexToEntity.push_back(entity);
    }

    T* getComponent(Entity entity) {
        auto it = entityToIndex.find(entity);
        if (it != entityToIndex.end()) {
            return &comp[it->second];
        }
        return nullptr;
    }

    void removeComponent(Entity entity) {
        auto it = entityToIndex.find(entity);
        if (it == entityToIndex.end()) {
            return; // Entity doesn't have this component
        }

        usize indexToRemove = it->second;
        usize lastIndex = comp.size() - 1;

        // Swap with the last element and pop
        if (indexToRemove < lastIndex) {
            comp[indexToRemove] = std::move(comp[lastIndex]);
            Entity movedEntity = indexToEntity[lastIndex];
            entityToIndex[movedEntity] = indexToRemove;
            indexToEntity[indexToRemove] = movedEntity;
        }

        comp.pop_back();
        indexToEntity.pop_back();
        entityToIndex.erase(entity);
    }

    Entity getEntity(usize index) { return indexToEntity[index]; }

    usize size() const { return comp.size(); }

    bool empty() const { return comp.empty(); }

  private:
    std::vector<T> comp;
    std::unordered_map<Entity, usize> entityToIndex;
    std::vector<Entity> indexToEntity;
};

typedef struct Position {
    f32 x;
    f32 y;
} Position;

typedef struct Velocity {
    f32 x;
    f32 y;
} Velocity;

class World {
  private:
    u32 entityCounter = 0;
    std::unordered_map<std::type_index, std::any> componentVecs;

  public:
    Entity createEntity() { return entityCounter++; }

    _generic void registerComponent() {
        componentVecs.insert(
            {std::type_index(typeid(T)), ComponentVec<T>()});
    }

    _generic ComponentVec<T>& getComponentVec() {
        return std::any_cast<ComponentVec<T>&>(componentVecs[typeid(T)]);
    }
};

class MovementSystem {
  private:
    vec(Position) positions;
    vec(Velocity) velocities;

  public:
    MovementSystem(vec(Position) positions, vec(Velocity) velocities)
        : positions(positions), velocities(velocities) {}

    void update() {

        i32 maxSpeed = 40;
        i32 speed = 4;
        const f32 friction = 0.95;

        for (usize i = 0; i < positions.size(); ++i) {
            const Entity entity = positions.getEntity(i);
            Position* pos = positions.getComponent(entity);
            Velocity* vel = velocities.getComponent(entity);
            const f32 deltaTime = GetFrameTime();

            if (vel && pos) {
                bool inMotion = false;

                if (IsKeyDown(KEY_LEFT_SHIFT)) {
                    speed *= 2;
                    maxSpeed *= 2;
                }

                if (IsKeyDown(KEY_W)) {
                    vel->y -= speed;
                    inMotion = true;
                }
                if (IsKeyDown(KEY_S)) {
                    vel->y += speed;
                    inMotion = true;
                }
                if (IsKeyDown(KEY_A)) {
                    vel->x -= speed;
                    inMotion = true;
                }
                if (IsKeyDown(KEY_D)) {
                    vel->x += speed;
                    inMotion = true;
                }
                const f32 mag = MAG((*vel));

                if (mag > maxSpeed) {
                    vel->x = vel->x / mag * maxSpeed;
                    vel->y = vel->y / mag * maxSpeed;
                } else if (!inMotion) {
                    vel->x *= friction;
                    vel->y *= friction;
                }
                printf("Speed: %f\n", mag);

                pos->x += vel->x * deltaTime;
                pos->y += vel->y * deltaTime;
            }
        }
    }
};

class BouncySystem {
  private:
    vec(Position) positions;
    vec(Velocity) velocities;

  public:
    BouncySystem(vec(Position) positions, vec(Velocity) velocities)
        : positions(positions), velocities(velocities) {}

    void update() {
        for (usize i = 0; i < positions.size(); ++i) {
            const Entity entity = positions.getEntity(i);
            Position* pos = positions.getComponent(entity);
            Velocity* vel = velocities.getComponent(entity);
            const f32 deltaTime = GetFrameTime();

            if (vel && pos) {
                if (pos->x < 0 || pos->x > screenWidth) {
                    vel->x = -vel->x;
                }
                if (pos->y < 0 || pos->y > screenHeight) {
                    vel->y = -vel->y;
                }

                pos->x += vel->x * deltaTime;
                pos->y += vel->y * deltaTime;
            }
        }
    }
};

class RenderSystem {
  public:
    void update(ComponentVec<Position>& positions) {
        for (usize i = 0; i < positions.size(); ++i) {
            Entity entity = positions.getEntity(i);
            if (auto* position = positions.getComponent(entity)) {
                DrawCircle(position->x, position->y, 10, RED);
            }
        }
    }
};

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(screenWidth, screenHeight, "Planet Generation Test");
    InitAudioDevice();
    SetMasterVolume(1);
    SetTargetFPS(144);
    SetWindowSize(screenWidth * 2, screenHeight * 2);

    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    World world;

    world.registerComponent<Position>();
    world.registerComponent<Velocity>();
    ComponentVec<Position>& positions = world.getComponentVec<Position>();
    ComponentVec<Velocity>& velocities = world.getComponentVec<Velocity>();

    BouncySystem bounce(positions, velocities);

    RenderSystem renderSystem;

    Entity entity = world.createEntity();
    positions.addComponent(entity, (Position){100, 100});
    velocities.addComponent(entity, (Velocity){30, 30});

    while (!WindowShouldClose()) {

        f32 scale = MIN((f32)GetScreenWidth() / screenWidth,
                        (float)GetScreenHeight() / screenHeight);

        mouse = getScreenMousePos(&mouse, scale, screenWidth, screenHeight);

        bounce.update();

        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            printf("pressed middle mouse button\n");
        }

        BeginTextureMode(target);

        ClearBackground(BLACK);
        renderSystem.update(positions);
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        drawScaledWindow(target, screenWidth, screenHeight, scale);
        EndDrawing();
    }
}
