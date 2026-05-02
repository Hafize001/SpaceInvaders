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

    int score = 0;      // Skor değişkeni eklendi
    bool paused = false;    
    bool game_over = false; 

    int menuSelection = 0; // 0: RESUME, 1: RESTART, 2: QUIT

    while (!WindowShouldClose()) {

        // Duraklatma ve Menü Kısayolları
        if (IsKeyPressed(KEY_P)) {
            paused = !paused;
            menuSelection = 0;
        }

        if (paused) {
            // Hızlı Tuşlar
            if (IsKeyPressed(KEY_R)) { // R tuşu ile direkt restart
                InitPlayer(&gemi);
                InitEnemies(ordumuz);
                InitBullet(&mermi);
                score = 0;
                game_over = false;
                paused = false;
            }
            if (IsKeyPressed(KEY_Q)) break; // Q tuşu ile direkt çıkış

            // Ok tuşları ile menüde gezinme
            if (IsKeyPressed(KEY_DOWN)) menuSelection = (menuSelection + 1) % 3;
            if (IsKeyPressed(KEY_UP)) menuSelection = (menuSelection - 1 + 3) % 3;

            // Enter ile seçim onaylama
            if (IsKeyPressed(KEY_ENTER)) {
                if (menuSelection == 0) paused = false;
                else if (menuSelection == 1) {
                    InitPlayer(&gemi);
                    InitEnemies(ordumuz);
                    InitBullet(&mermi);
                    score = 0;
                    game_over = false;
                    paused = false;
                }
                else if (menuSelection == 2) break;
            }
        }

        if (!paused && !game_over) {
            float dt = GetFrameTime(); 
            
            UpdatePlayer(&gemi); 
            UpdateEnemies(ordumuz, dt, SCREEN_WIDTH);

            // --- ATEŞ ETME KONTROLÜ ---
            if (IsKeyPressed(KEY_SPACE) && !mermi.active) {
                mermi.active = true;
                mermi.position = (Vector2){ gemi.position.x + 17, gemi.position.y };
            }

            // Merminin hareketini güncelle
            UpdateBullet(&mermi, dt);

            // --- ÇARPIŞMA KONTROLÜ (MERMİ DÜŞMANI VURDU MU?) ---
            if (mermi.active) {
                for (int i = 0; i < ENEMY_ROWS; i++) {
                    for (int j = 0; j < ENEMY_COLS; j++) {
                        if (ordumuz[i][j].active && CheckCollisionRecs(
                            (Rectangle){ mermi.position.x, mermi.position.y, 5, 15 }, 
                            (Rectangle){ ordumuz[i][j].position.x, ordumuz[i][j].position.y, ordumuz[i][j].size.x, ordumuz[i][j].size.y })) 
                        {
                            ordumuz[i][j].active = false; 
                            mermi.active = false;        
                            score += 100;                
                            break; 
                        }
                    }
                }
            }

            // --- OYUN BİTİŞ KONTROLÜ ---
            for (int i = 0; i < ENEMY_ROWS; i++) {
                for (int j = 0; j < ENEMY_COLS; j++) {
                    if (ordumuz[i][j].active && ordumuz[i][j].position.y + ordumuz[i][j].size.y >= gemi.position.y) {
                        game_over = true;
                    }
                }
            }
        }

        BeginDrawing();
            ClearBackground(BLACK); 
            
            // Çizimler (Duraklatılsa bile görünürler)
            DrawPlayer(&gemi); 
            DrawEnemies(ordumuz);
            DrawBullet(&mermi); 

            // Skoru ekrana yazdır
            DrawText(TextFormat("SCORE: %05d", score), 20, 20, 20, RAYWHITE);

            if (paused) {
                // Ekranı daha çok kararttık (Alpha 200/255)
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 200 });

                // Menü Kutusu
                Rectangle menuBox = { SCREEN_WIDTH/2 - 125, SCREEN_HEIGHT/2 - 100, 250, 200 };
                DrawRectangleRec(menuBox, (Color){ 30, 30, 30, 255 });
                DrawRectangleLinesEx(menuBox, 3, MAGENTA); // Kenarlık senin renginde olsun

                DrawText("PAUSE MENU", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 80, 25, RAYWHITE);

                // Seçenekler
                DrawText(menuSelection == 0 ? "> RESUME" : "  RESUME", SCREEN_WIDTH/2 - 60, SCREEN_HEIGHT/2 - 20, 20, menuSelection == 0 ? LIME : GRAY);
                DrawText(menuSelection == 1 ? "> RESTART (R)" : "  RESTART (R)", SCREEN_WIDTH/2 - 60, SCREEN_HEIGHT/2 + 20, 20, menuSelection == 1 ? LIME : GRAY);
                DrawText(menuSelection == 2 ? "> QUIT (Q)" : "  QUIT (Q)", SCREEN_WIDTH/2 - 60, SCREEN_HEIGHT/2 + 60, 20, menuSelection == 2 ? LIME : GRAY);
            } 
            else if (game_over) {
                const char* over_text = "GAME OVER";
                DrawText(over_text, (SCREEN_WIDTH - MeasureText(over_text, 40)) / 2, SCREEN_HEIGHT / 2, 40, RED);
            } 
                
        EndDrawing(); 
    } // While döngüsünün sonu

    CloseWindow();
    return 0;
} // Main fonksiyonunun sonu