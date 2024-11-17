#pragma once
#include "defs.h"
#include "flecs.h"

typedef struct {
    const char* text;
    v2 offset;
    Texture2D icon;
    f32 fontSize;
} label_c;

typedef ecs_entity_t textbox_e;

extern ECS_COMPONENT_DECLARE(label_c);

void UIModuleImport(ecs_world_t* world);

textbox_e createTextbox(const char* title, v2 pos);
ecs_entity_t TextboxPush(textbox_e e, const char* text, f32 fontSize,
                         Texture2D icon);
void basicButtonRender(ecs_entity_t e);
