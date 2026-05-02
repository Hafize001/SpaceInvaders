#include "raylib.h"
#include <stdio.h>
#include "player.h" 
#include "common.h"
#include "enemy.h"
#include "bullet.h" // Mermi modülünü dahil ettik
#include "gamestate.h" // Oyun durum modülünü dahil ettik

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
    bool victory = false;   // Zafer durumu eklendi

    int menuSelection = 0; 
    GameScreen currentScreen = SCREEN_MENU; 

    while (!WindowShouldClose()) {

        // --- EKRAN YÖNETİMİ (UPDATE) ---
        switch (currentScreen) {
            case SCREEN_MENU:
                if (IsKeyPressed(KEY_ENTER)) {
                    currentScreen = SCREEN_GAMEPLAY;
                }
                if (IsKeyPressed(KEY_Q)) return 0; 
                break;

            case SCREEN_GAMEPLAY:
                // Duraklatma ve Menü Kısayolları
                if (IsKeyPressed(KEY_P)) {
                    paused = !paused;
                    menuSelection = 0;
                }

                // İç oyunda M tuşuna basılırsa direkt ana menüye dön
                if (IsKeyPressed(KEY_M)) {
                    currentScreen = SCREEN_MENU;
                    paused = false; // Menüye dönerken pause durumunu temizle
                }

                if (paused || game_over || victory) {
                    // Hızlı Tuşlar
                    if (IsKeyPressed(KEY_R)) { // R tuşu ile direkt restart
                        InitPlayer(&gemi);
                        InitEnemies(ordumuz);
                        InitBullet(&mermi);
                        score = 0;
                        game_over = false;
                        victory = false;
                        paused = false;
                    }
                    if (IsKeyPressed(KEY_Q)) return 0; // Q tuşu ile direkt çıkış
                }

                if (paused) {
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
                            victory = false;
                            paused = false;
                        }
                        else if (menuSelection == 2) {
                            currentScreen = SCREEN_MENU; // Ana menüye dön
                            paused = false;
                        }
                    }
                }

                if (!paused && !game_over && !victory) {
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

                    // --- ZAFER KONTROLÜ ---
                    int activeEnemies = 0;
                    for (int i = 0; i < ENEMY_ROWS; i++) {
                        for (int j = 0; j < ENEMY_COLS; j++) {
                            if (ordumuz[i][j].active) activeEnemies++;
                        }
                    }
                    if (activeEnemies == 0) victory = true;

                    // --- OYUN BİTİŞ KONTROLÜ ---
                    for (int i = 0; i < ENEMY_ROWS; i++) {
                        for (int j = 0; j < ENEMY_COLS; j++) {
                            if (ordumuz[i][j].active && ordumuz[i][j].position.y + ordumuz[i][j].size.y >= gemi.position.y) {
                                game_over = true;
                            }
                        }
                    }
                }
                break;
        }

        BeginDrawing();
            ClearBackground(BLACK); 
            
            if (currentScreen == SCREEN_MENU) {
                // --- ANA MENÜ ÇİZİMİ ---
                DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){10, 10, 25, 255}, BLACK);
                DrawLineEx((Vector2){0, 150}, (Vector2){SCREEN_WIDTH, 150}, 2, MAGENTA);
                DrawLineEx((Vector2){0, SCREEN_HEIGHT - 100}, (Vector2){SCREEN_WIDTH, SCREEN_HEIGHT - 100}, 2, MAGENTA);

                DrawText("SPACE INVADERS", SCREEN_WIDTH/2 - 205, 205, 45, DARKPURPLE);
                DrawText("SPACE INVADERS", SCREEN_WIDTH/2 - 200, 200, 45, MAGENTA);

                Rectangle startBtn = { SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 - 40, 300, 80 };
                DrawRectangleLinesEx(startBtn, 3, LIME);
                DrawText("PRESS ENTER", SCREEN_WIDTH/2 - 95, SCREEN_HEIGHT/2 - 25, 30, LIME);
                DrawText("TO START MISSION", SCREEN_WIDTH/2 - 65, SCREEN_HEIGHT/2 + 10, 15, RAYWHITE);

                DrawText("DEVELOPED BY THE TEAM", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT - 60, 15, GRAY);
                DrawText("PRESS [Q] TO EXIT", 20, SCREEN_HEIGHT - 30, 15, DARKGRAY);
            }
            else if (currentScreen == SCREEN_GAMEPLAY) {
                // Çizimler (Duraklatılsa bile görünürler)
                DrawPlayer(&gemi); 
                DrawEnemies(ordumuz);
                DrawBullet(&mermi); 

                // Skoru ekrana yazdır
                DrawText(TextFormat("SCORE: %05d", score), 20, 20, 20, RAYWHITE);

                if (paused) {
                    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 200 });
                    Rectangle menuBox = { SCREEN_WIDTH/2 - 125, SCREEN_HEIGHT/2 - 100, 250, 200 };
                    DrawRectangleRec(menuBox, (Color){ 30, 30, 30, 255 });
                    DrawRectangleLinesEx(menuBox, 3, MAGENTA); 

                    DrawText("PAUSE MENU", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 80, 25, RAYWHITE);

                    // Seçenekler (M tuşu ibaresi eklendi)
                    DrawText(menuSelection == 0 ? "> RESUME" : "  RESUME", SCREEN_WIDTH/2 - 60, SCREEN_HEIGHT/2 - 20, 20, menuSelection == 0 ? LIME : GRAY);
                    DrawText(menuSelection == 1 ? "> RESTART (R)" : "  RESTART (R)", SCREEN_WIDTH/2 - 60, SCREEN_HEIGHT/2 + 20, 20, menuSelection == 1 ? LIME : GRAY);
                    DrawText(menuSelection == 2 ? "> MAIN MENU (M)" : "  MAIN MENU (M)", SCREEN_WIDTH/2 - 60, SCREEN_HEIGHT/2 + 60, 20, menuSelection == 2 ? LIME : GRAY);
                } 
                else if (game_over) {
                    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 150 });
                    const char* over_text = "GAME OVER";
                    DrawText(over_text, (SCREEN_WIDTH - MeasureText(over_text, 40)) / 2, SCREEN_HEIGHT / 2 - 20, 40, RED);
                    DrawText("Press R to Restart or M for Menu", SCREEN_WIDTH/2 - 130, SCREEN_HEIGHT/2 + 40, 20, RAYWHITE);
                } 
                else if (victory) {
                    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 150 });
                    const char* win_text = "VICTORY!";
                    DrawText(win_text, (SCREEN_WIDTH - MeasureText(win_text, 40)) / 2, SCREEN_HEIGHT / 2 - 20, 40, LIME);
                    DrawText("Press R to Play Again or M for Menu", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 40, 20, RAYWHITE);
                }
            }
                
        EndDrawing(); 
    } // While döngüsünün sonu

    CloseWindow();
    return 0;
} // Main fonksiyonunun sonu