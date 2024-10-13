#include <raylib.h>

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "CRT Effect Example");
    SetTargetFPS(60);

    // Load the CRT shader
    Shader crtShader = LoadShader(0, "crt.frag.glsl");

    // Create a render texture for the main scene
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);

    while (!WindowShouldClose()) {
        // Step 1: Render the scene to an off-screen texture
        BeginTextureMode(target);
        ClearBackground(RAYWHITE); // Render the scene (replace this with
                                   // your game logic)

        // Draw your game objects here
        DrawCircle(GetMouseX(), GetMouseY(), 50, RED); // Example object

        EndTextureMode();

        // Step 2: Apply the CRT effect shader
        BeginDrawing();
        ClearBackground(BLACK);

        // Set shader parameters
        float time = GetTime();
        SetShaderValue(crtShader, GetShaderLocation(crtShader, "time"),
                       &time, SHADER_UNIFORM_FLOAT);
        Vector2 resolution = {screenWidth, screenHeight};
        SetShaderValue(crtShader,
                       GetShaderLocation(crtShader, "resolution"),
                       &resolution, SHADER_UNIFORM_VEC2);

        // Draw the texture with the CRT shader
        BeginShaderMode(crtShader);
        DrawTexture(target.texture, 0, 0, WHITE);
        EndShaderMode();

        EndDrawing();
    }

    // Cleanup
    UnloadShader(crtShader);
    UnloadRenderTexture(target);
    CloseWindow();

    return 0;
}
