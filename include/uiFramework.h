#pragma once
#include "defs.h"
#include "flecs.h"

typedef struct {
    const char* text;
    v2 offset;
    Texture2D icon;
} label_c;

typedef ecs_entity_t textbox_e;

extern ECS_COMPONENT_DECLARE(label_c);

void UIModuleImport(ecs_world_t* world);

textbox_e createTextbox(v2 pos);
ecs_entity_t TextboxPush(textbox_e e, const char* text, Texture2D icon);
