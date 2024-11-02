#include "uiFramework.h"
#include "defs.h"
#include "planet.h"
#include "raylib.h"
#include "state.h"
#include "transform.h"

ECS_COMPONENT_DECLARE(label_c);
ECS_TAG_DECLARE(textbox_tg);

void label_s(ecs_iter_t* it) {
    label_c* l = ecs_field(it, label_c, 0);
    const position_c* pos = ecs_field(it, position_c, 1);
    i32 iconOffset = 0;

    if (l->icon.width != 0) {
        DrawTexture(l->icon, pos->x, pos->y - l->icon.height / 2.0, WHITE);
        iconOffset = l->icon.width;
    }

    i32 yoff = MeasureTextEx(globalFont, l->text, 20, 1).y / 2;

    for (int i = 0; i < it->count; i++) {
        DrawTextEx(globalFont, l[i].text,
                   (v2){pos->x + l[i].offset.x + iconOffset,
                        pos->y + l[i].offset.y - yoff},
                   20, 1, WHITE);
    }
}

void renderLabel(ecs_entity_t e) {
    const label_c* l = ecs_get(world, e, label_c);
    const position_c* pos = ecs_get(world, e, position_c);
    i32 iconOffset = 0;

    if ((l->icon.width != 0)) {
        DrawTexture(l->icon, pos->x + l->offset.x,
                    pos->y + l->offset.y - l->icon.height / 2.0, WHITE);
        iconOffset = l->icon.width * 1.2f;
    }

    i32 yoff = MeasureTextEx(globalFont, l->text, 20, 1).y / 2;

    DrawTextEx(globalFont, l->text,
               (v2){pos->x + l->offset.x + iconOffset,
                    pos->y + l->offset.y - yoff},
               20, 1, WHITE);
}
void renderTextbox(ecs_entity_t e) {
    const position_c* pos = ecs_get(world, e, position_c);
    DrawRectangle(pos->x, pos->y, 100, 20, GRUV_DARK2);
}
textbox_e createTextbox(v2 pos) {
    textbox_e e = ecs_new(world);
    ecs_set(world, e, position_c, {pos.x, pos.y});
    ecs_set(world, e, Renderable, {1, renderTextbox});
    ecs_add_id(world, e, textbox_tg);
    return e;
}

ecs_entity_t TextboxPush(textbox_e e, const char* text, Texture2D icon) {
    const position_c* boxPos = ecs_get(world, e, position_c);
    u32 priority = ecs_get(world, e, Renderable)->renderLayer + 1;

    const u16 paddingx = 10;
    const i32 paddingy = MeasureTextEx(globalFont, text, 20, 1).y / 2;

    ecs_entity_t label = ecs_entity(world, {.parent = e});
    ecs_set(world, label, label_c, {text, {16 + paddingx, paddingy}, icon});
    ecs_set(world, label, position_c, {boxPos->x, boxPos->y});
    ecs_set(world, label, Renderable, {priority, renderLabel});

    return label;
}

void UIModuleImport(ecs_world_t* world) {
    ECS_MODULE(world, UIModule);
    ECS_COMPONENT_DEFINE(world, label_c);
    ECS_TAG_DEFINE(world, textbox_tg);
}
