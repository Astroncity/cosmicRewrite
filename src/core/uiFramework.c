#include "uiFramework.h"
#include "defs.h"
#include "raylib.h"
#include "render.h"
#include "state.h"
#include "transform.h"

ECS_COMPONENT_DECLARE(label_c);

typedef struct {
    usize size;
    i32 maxLen;
    i32 minLen;
    v2 endCon;
} textbox_c;
ECS_COMPONENT_DECLARE(textbox_c);

void drawConnectiveLine(const v2 start, const v2 end) {
    const v2 dist = (v2){end.x - start.x, end.y - start.y};
    const f32 width = 1;
    const Color cl = WHITE;

    DrawLineEx(start, (v2){start.x + dist.x / 2, start.y}, width, cl);
    DrawLineEx((v2){start.x + dist.x / 2, start.y},
               (v2){start.x + dist.x / 2, end.y}, width, cl);
    DrawLineEx((v2){start.x + dist.x / 2, end.y}, (v2){end.x, end.y}, width, cl);
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

    i32 yoff = MeasureTextEx(globalFont, l->text, l->fontSize, 1).y / 2;

    DrawTextEx(globalFont, l->text,
               (v2){pos->x + l->offset.x + iconOffset, pos->y + l->offset.y - yoff},
               l->fontSize, 1, WHITE);
}
void renderTextbox(ecs_entity_t e) {
    const position_c* pos = ecs_get(world, e, position_c);
    const textbox_c* box = ecs_get(world, e, textbox_c);

    const f32 width = MAX(box->maxLen, box->minLen);

    DrawRectangleRounded((Rect){pos->x, pos->y, width, 20 * box->size}, 0.3, 2,
                         GRUV_DARK2);

    if (box->endCon.x != -1) {
        drawConnectiveLine((v2){pos->x + box->maxLen, pos->y + 20}, box->endCon);
    }
}
textbox_e createTextbox(const char* title, v2 pos, v2 connectionPoint) {
    textbox_e e = ecs_new(world);
    ecs_set(world, e, position_c, {pos.x, pos.y});
    ecs_set(world, e, Renderable, {5, renderTextbox});
    ecs_set(world, e, textbox_c,
            {.size = 0, .maxLen = 0, .minLen = 100, .endCon = connectionPoint});

    TextboxPush(e, title, 20, (Texture2D){});
    TextboxPush(e, "", 20, (Texture2D){});
    return e;
}

ecs_entity_t TextboxPush(textbox_e e, const char* text, f32 fontSize,
                         Texture2D icon) {
    const position_c* boxPos = ecs_get(world, e, position_c);
    u32 priority = ecs_get(world, e, Renderable)->renderLayer + 1;
    textbox_c* box = ecs_get_mut(world, e, textbox_c);

    const v2 measure = MeasureTextEx(globalFont, text, fontSize, 1);

    const i16 padx = 5;
    i32 pady = measure.y / 1.7;

    box->maxLen = MAX(box->maxLen, measure.x + padx + icon.width * (2.5f) + 10);
    printf("New maxLen: %d\n", box->maxLen);

    ecs_entity_t label = ecs_entity(world, {.parent = e});
    ecs_set(world, label, label_c,
            {.text = text,
             .offset = {padx, pady + 5},
             .icon = icon,
             .fontSize = fontSize});
    ecs_set(world, label, position_c,
            {boxPos->x, boxPos->y + (pady * 2 * box->size)});
    ecs_set(world, label, Renderable, {priority, renderLabel});
    ++box->size;

    return label;
}

void UIModuleImport(ecs_world_t* world) {
    ECS_MODULE(world, UIModule);
    ECS_COMPONENT_DEFINE(world, label_c);
    ECS_COMPONENT_DEFINE(world, textbox_c);
}
