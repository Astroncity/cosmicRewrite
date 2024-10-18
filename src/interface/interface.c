// Include necessary libraries
#include "defs.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const u32 screenWidth = 1920;
const u32 screenHeight = 1080;

#define MAX_COMPONENTS 100
#define MAX_FIELDS_PER_COMPONENT 10
#define MAX_FIELD_NAME_LENGTH 100

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MAG(v) sqrt(v.x* v.x + v.y * v.y)

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

// Structure to store component data
typedef struct {
    char* name;
    char* fieldTypes[MAX_FIELDS_PER_COMPONENT];
    char* fieldNames[MAX_FIELDS_PER_COMPONENT];
    int fieldCount;
} Component;

// Function to run Python script and generate output file
void runPythonScript(const char* srcDirectory, const char* outputFile) {
    char command[512];
    snprintf(command, sizeof(command), "python3 getComponents.py %s %s",
             srcDirectory, outputFile);
    system(command);
}

// Function to load components from the output file
int loadComponents(const char* outputFile, Component* components) {
    FILE* file = fopen(outputFile, "r");
    if (!file) {
        printf("Failed to open output file: %s\n", outputFile);
        return 0;
    }

    char line[256];
    int componentCount = 0;
    Component* currentComponent = NULL;

    while (fgets(line, sizeof(line), file)) {
        // Parse component name
        if (strncmp(line, "Component:", 10) == 0) {
            currentComponent = &components[componentCount++];
            currentComponent->name = strdup(line + 11);
            currentComponent->fieldCount = 0;
        }
        // Parse fields
        else if (currentComponent && strncmp(line, "Fields:", 7) != 0) {
            char* type = strtok(line, " ");
            char* name = strtok(NULL, ";\n");

            if (type && name) {
                currentComponent->fieldTypes[currentComponent->fieldCount] =
                    strdup(type);
                currentComponent->fieldNames[currentComponent->fieldCount] =
                    strdup(name);
                currentComponent->fieldCount++;
            }
        }
    }

    fclose(file);
    return componentCount;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <src directory>\n", argv[0]);
        return 1;
    }

    const char* srcDirectory = argv[1];
    const char* outputFile = "components_output.txt";

    // Run Python script to extract component data
    runPythonScript(srcDirectory, outputFile);

    // Load components from the generated output file
    Component components[MAX_COMPONENTS];
    int componentCount = loadComponents(outputFile, components);

    // Initialize Raylib
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(screenWidth, screenHeight, "Component Viewer");
    InitAudioDevice();
    SetMasterVolume(1);
    SetTargetFPS(60);
    SetWindowSize(screenWidth / 2, screenHeight / 2);
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    // Scroll control
    float scrollOffset = 0.0f;
    const float scrollSpeed = 20.0f;

    while (!WindowShouldClose()) {
        // Update mouse position and handle scrolling
        f32 scale = MIN((f32)GetScreenWidth() / screenWidth,
                        (float)GetScreenHeight() / screenHeight);
        mouse = getScreenMousePos(&mouse, scale, screenWidth, screenHeight);

        if (IsKeyDown(KEY_UP)) scrollOffset -= scrollSpeed;
        if (IsKeyDown(KEY_DOWN)) scrollOffset += scrollSpeed;

        scrollOffset =
            MAX(scrollOffset, 0.0f); // Ensure scrollOffset is non-negative

        BeginTextureMode(target);
        ClearBackground(BLACK);

        // Draw components and their fields with rectangles and scrolling
        int yPosition = 10 - (int)scrollOffset;
        for (int i = 0; i < componentCount; i++) {
            // Draw rectangle for component name
            DrawRectangle(5, yPosition - 5, 400, 30, DARKGRAY);
            DrawText(components[i].name, 10, yPosition, 80, WHITE);
            yPosition += 30;

            for (int j = 0; j < components[i].fieldCount; j++) {
                char field[256];
                snprintf(field, sizeof(field), "    %s %s",
                         components[i].fieldTypes[j],
                         components[i].fieldNames[j]);

                // Draw rectangle for field
                DrawRectangle(10, yPosition - 5, 400, 40, GRAY);
                DrawText(field, 20, yPosition, 70, WHITE);
                yPosition += 50;
            }
            yPosition += 50;
        }

        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        drawScaledWindow(target, screenWidth, screenHeight, scale);
        EndDrawing();
    }

    // Clean up
    for (int i = 0; i < componentCount; i++) {
        free(components[i].name);
        for (int j = 0; j < components[i].fieldCount; j++) {
            free(components[i].fieldTypes[j]);
            free(components[i].fieldNames[j]);
        }
    }

    CloseWindow();

    return 0;
}
