#include "raylib.h"
#include <stdio.h>
#include "player.h" 
#include "common.h"
#include "enemy.h"


int main() {
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Invaders Game");
    SetTargetFPS(60);

    
    Player gemi;       
    InitPlayer(&gemi);  
    
    Enemy ordumuz[ENEMY_ROWS][ENEMY_COLS];
    InitEnemies(ordumuz);

    bool paused = false;    
    bool game_over = false; 

    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_P)) {
            paused = !paused;
        }

        if (!paused && !game_over) {
            float dt = GetFrameTime(); 
            
            UpdatePlayer(&gemi); 
            UpdateEnemies(ordumuz, dt, SCREEN_WIDTH);
        }
 
        BeginDrawing();
            ClearBackground(BLACK); 
            //durdurunca gemi resmi gidiyordu yukarı aldım
            DrawPlayer(&gemi); 
            DrawEnemies(ordumuz);

            if (paused) {
                const char* pause_text = "PAUSED";
                int text_width = MeasureText(pause_text, 20);
                
                DrawText(pause_text, (SCREEN_WIDTH - text_width) / 2, SCREEN_HEIGHT / 2, 20, YELLOW);
            } 
            else if (game_over) {
                const char* over_text = "GAME OVER";
                int text_width = MeasureText(over_text, 40);
                DrawText(over_text, (SCREEN_WIDTH - text_width) / 2, SCREEN_HEIGHT / 2, 40, RED);
            } 
               
        EndDrawing(); 
    } // While döngüsünün sonu

    CloseWindow();
    return 0;
} // Main fonksiyonunun sonu 