#include "raylib.h"

int main() {
    InitWindow(800, 600, "Space Invaders - Altyapi Hazir");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Raylib Calisiyor!", 300, 280, 20, RAYWHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}