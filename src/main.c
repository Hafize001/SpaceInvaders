#include "raylib.h"
#include <stdio.h>
#include "player.h" 
#include "common.h"
#include "enemy.h"
#include "bullet.h" // Mermi modülünü dahil ettik

int main() {
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Invaders Game");
    SetTargetFPS(60);

    // --- HAZIRLIKLAR ---
    Player gemi;       
    InitPlayer(&gemi);  
    
    Enemy ordumuz[ENEMY_ROWS][ENEMY_COLS];
    InitEnemies(ordumuz);

    Bullet mermi;       // Mermi objesini oluşturduk
    InitBullet(&mermi); // Mermiyi başlangıç durumuna getirdik

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

            // --- ATEŞ ETME KONTROLÜ ---
            // Boşluk tuşuna basıldığında ve ekranda aktif bir mermi yoksa ateş et
            if (IsKeyPressed(KEY_SPACE) && !mermi.active) {
                mermi.active = true;
                // Mermiyi tam geminin ortasından başlatıyoruz
                mermi.position = (Vector2){ gemi.position.x + 20, gemi.position.y };
            }

            // Merminin hareketini güncelle
            UpdateBullet(&mermi, dt);
            // --- ÇARPIŞMA KONTROLÜ (MERMİ DÜŞMANI VURDU MU?) ---
            if (mermi.active) {
                for (int i = 0; i < ENEMY_ROWS; i++) {
                    for (int j = 0; j < ENEMY_COLS; j++) {
                        // Eğer düşman hayattaysa ve mermiyle temas ediyorsa
                        if (ordumuz[i][j].active && CheckCollisionRecs(
                            (Rectangle){ mermi.position.x, mermi.position.y, 5, 15 }, 
                            (Rectangle){ ordumuz[i][j].position.x, ordumuz[i][j].position.y, ordumuz[i][j].size.x, ordumuz[i][j].size.y })) 
                        {
                            ordumuz[i][j].active = false; // Düşman patladı!
                            mermi.active = false;        // Mermi yok oldu!
                            break; // Bir mermiyle sadece bir düşman vurulsun
                        }
                    }
                }
            }

             // --- OYUN BİTİŞ KONTROLÜ ---         
        }
 
        BeginDrawing();
            ClearBackground(BLACK); 
            
            // Çizimler (Duraklatılsa bile görünürler)
            DrawPlayer(&gemi); 
            DrawEnemies(ordumuz);
            DrawBullet(&mermi); // Mermiyi ekrana çiz

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