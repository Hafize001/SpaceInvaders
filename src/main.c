#include "raylib.h"
#include <stdio.h>

const int screen_width = 800;
const int screen_height = 600;

// yapilar tanimlanicak(player, enemy gibi)


int main() {
    InitWindow(800, 600, "Space Invaders Game");
    SetTargetFPS(60);

    bool paused = false;
    bool game_over = false;

    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_P)) {
            paused = !paused;
        }
        if (!paused && !game_over) {
            
            float dt = GetFrameTime();
            
            //dosyalarimizdaki fonksiyonlari burada cagirmaliyiz
            
        }
        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Raylib Calisiyor!", 300, 280, 20, RAYWHITE);
            if (paused) {
            // oyun durduruldugunda ekrana yazilacaklar
            const char* pause_text = "PAUSED";
            int text_width = MeasureText(pause_text, 20);
            DrawText(pause_text, (screen_width - text_width) / 2, screen_height / 2, 20, YELLOW);
        } 
        else if (game_over) {
            // Oyun bittiğinde ekrana yazdırılacaklar
            const char* over_text = "GAME OVER";
            int text_width = MeasureText(over_text, 40);
            DrawText(over_text, (screen_width - text_width) / 2, screen_height / 2, 40, RED);
        }else {
            // Oyun normal akarken çizilecek nesneler
            // DrawPlayer(&gemi);
            // DrawBullets();
            // DrawEnemies();
            
            // Şimdilik sistemin çalıştığını görmek için geçici bir yazı
            DrawText("Oyun Motoru Aktif! Kodlar Buraya Gelecek.", 10, 10, 20, GREEN);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}