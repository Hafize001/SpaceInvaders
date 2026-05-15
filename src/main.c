#include "raylib.h"
#include <stdio.h>
#include "player.h" 
#include "common.h"
#include "enemy.h" 
#include "game.h"  
#include "bullet.h"  
#include "gamestate.h" 
#include "ui.h" 

int main() {
    
    // Ekran boyutunu 1920x1080 olarak sabitledim(monitorde yansitmak icin)
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Invaders Game");

    ToggleFullscreen(); 

    SetTargetFPS(60);

    // --- GÖRSELLERİ HAFIZAYA YÜKLE ---
    Image spriteSheet = LoadImage("../assets/pico8_invaders_sprites_LARGE.png"); 
    ImageColorReplace(&spriteSheet, BLACK, BLANK); 
    Texture2D enemySpriteSheet = LoadTextureFromImage(spriteSheet); 
    UnloadImage(spriteSheet); 
    Texture2D heartIcon = LoadTexture("../assets/Heart_Pump.png");
    Texture2D background_1 = LoadTexture("../assets/Space_01-Sheet.png");
    Texture2D background_2 = LoadTexture("../assets/Space_02-Sheet.png");
    
    int score = 0;  
    int highest_score = 0;    
    int currentLevel = 1; // Level sistemi 
    int lives = 3;        // Can sistemi 

    bool paused = false;    
    bool game_over = false; 
    bool victory = false;   

    // Animasyon için zamanlayıcı ve anlık kare (0 veya 1)
    float animTimer = 0.0f;
    int currentFrame = 0;

    int menuSelection = 0; 
    GameScreen currentScreen = SCREEN_MENU; 
    
    // --- HAZIRLIKLAR ---
    Player gemi;       
    InitPlayer(&gemi);  
    
    Enemy ordumuz[ENEMY_ROWS][ENEMY_COLS];
    InitEnemies(ordumuz,currentLevel);  
    
    Ufo ufo;
    InitUfo(&ufo);

    Bullet bullets[MAX_BULLETS]; // Yeni çoklu mermi dizimiz
    InitBullets(bullets);

    EnemyBullet eBullets[MAX_ENEMY_BULLETS]; 
    InitEnemyBullets(eBullets);

   // Merminin ekranda kaplamasını istediğin gerçek boyut (16x16 piksel asset'ten kestim, 3 kat büyüteyim)
    // Başlangıç boyutunu küçük tutuyoruz. Örneğin 16x16 lık asset'i 1.5 kat büyüterek yollayalım.
    Vector2 mermiBoyutu = { 16.0f * 1.5f, 16.0f * 1.5f };
    float mermiHizi = 500.0f; // Saniyede 500 piksel git

    

    while (!WindowShouldClose()) {

         // --- EKRAN YÖNETİMİ ---
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

                if (game_over) {
                    if (IsKeyPressed(KEY_R)) { // R: Tamamen Sıfırla
                        currentLevel = 1;
                        lives = 3;
                        score = 0;
                        ResetArena(&gemi, ordumuz, bullets, eBullets, &ufo, currentLevel);
                        game_over = false;
                    }
                    if (IsKeyPressed(KEY_Q)) return 0; 
                } 
                else if (victory) {
                    if (IsKeyPressed(KEY_ENTER)) { // ENTER: Sonraki Level'a geç!
                        currentLevel++; // Leveli 1 artır
                        ResetArena(&gemi, ordumuz, bullets, eBullets, &ufo, currentLevel);
                        victory = false;
                    }
                    if (IsKeyPressed(KEY_Q)) return 0; 
                } 
                else if (paused) {
                    // Ok tuşları ile menüde gezinme
                    if (IsKeyPressed(KEY_DOWN)) menuSelection = (menuSelection + 1) % 3;
                    if (IsKeyPressed(KEY_UP)) menuSelection = (menuSelection - 1 + 3) % 3;

                    // Enter ile seçim onaylama
                    if (IsKeyPressed(KEY_ENTER)) {
                        if (menuSelection == 0) paused = false;
                        else if (menuSelection == 1) {
                            currentLevel = 1;
                            lives = 3;
                            score = 0;
                            ResetArena(&gemi, ordumuz, bullets, eBullets, &ufo, currentLevel);
                            paused = false;
                        }
                        else if (menuSelection == 2) {
                            currentScreen = SCREEN_MENU;
                            paused = false;
                        }
                    }
                }

                if (!paused && !game_over && !victory) {
                    UpdatePlayer(&gemi); 
                    UpdateEnemies(ordumuz, &animTimer, &currentFrame);
                    UpdateUfo(&ufo, GetFrameTime());

                    if (IsKeyPressed(KEY_SPACE)) { 
                         ShootBullet(bullets, gemi.position, gemi.size, mermiBoyutu, mermiHizi);
                    }

                    UpdateBullets(bullets, ordumuz, &score, &lives, &ufo);
                    if (score > highest_score) highest_score = score;

                    // --- YENİ: DÜŞMAN MERMİLERİ VE HASAR KONTROLÜ ---
                    bool playerHitByBullet = UpdateEnemyBullets(eBullets, ordumuz, &gemi);
                    bool enemyReachedUs = CheckEnemyReachedPlayer(ordumuz, &gemi);

                    if (playerHitByBullet || enemyReachedUs) {
                        lives--; // 1 Can Gitti
                        
                        if (lives <= 0) {
                            game_over = true; // Can bitti, game over
                        } else {
                            // Can varsa sadece sahneyi sıfırla (Level ve Skor korunur)
                            ResetArena(&gemi, ordumuz, bullets, eBullets, &ufo, currentLevel);
                        }
                    }

                    // --- YENİ: ZAFER KONTROLÜ ---
                    if (CheckVictory(ordumuz)) {
                        victory = true;
                    }
                }
                break;
        }

        BeginDrawing();
            ClearBackground((Color){ 10, 10, 25, 255 }); 
            
            if (currentScreen == SCREEN_MENU) {
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
                
                DrawBackground(background_1);

                DrawPlayer(&gemi); 
                DrawUfo(&ufo,enemySpriteSheet);
                DrawEnemies(ordumuz, enemySpriteSheet, currentFrame);
                DrawEnemyBullets(eBullets, enemySpriteSheet);        

                DrawRectangle(0, 0, LEFT_BOUND, SCREEN_HEIGHT, (Color){ 10, 10, 25, 255 }); 
                DrawRectangle(RIGHT_BOUND, 0, SCREEN_WIDTH - RIGHT_BOUND, SCREEN_HEIGHT, (Color){ 10, 10, 25, 255 });
                DrawGameplayUI(score, currentLevel, highest_score, lives, heartIcon);

                // Artık çizim yaparken resmi gönderiyoruz
                DrawBullets(bullets, enemySpriteSheet);

                // Pause ve Game Over ekranları...
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
                }else if (game_over) {
                    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 150 });
                    const char* over_text = "GAME OVER";
                    DrawText(over_text, (SCREEN_WIDTH - MeasureText(over_text, 40)) / 2, SCREEN_HEIGHT / 2 - 20, 40, RED);
                    DrawText("Press R to Restart or M for Menu", SCREEN_WIDTH/2 - 130, SCREEN_HEIGHT/2 + 40, 20, RAYWHITE);
                } 
                else if (victory) {
                    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 150 });
                    const char* win_text = "LEVEL CLEARED!"; // Yazıyı değiştirdik
                    DrawText(win_text, (SCREEN_WIDTH - MeasureText(win_text, 40)) / 2, SCREEN_HEIGHT / 2 - 40, 40, LIME);
                    
                    // R yerine ENTER ile geçiş yapıyoruz
                    DrawText("Press ENTER for Next Level", SCREEN_WIDTH/2 - 140, SCREEN_HEIGHT/2 + 20, 20, RAYWHITE);
                    DrawText("Press M for Main Menu", SCREEN_WIDTH/2 - 110, SCREEN_HEIGHT/2 + 60, 20, GRAY);
                }
            }
                
        EndDrawing(); 
    } 

    // --- TEMİZLİK (Ayrılmadan Önce Hafızayı Boşalt) ---
    UnloadTexture(gemi.gameShip);
    UnloadTexture(heartIcon);
    UnloadTexture(background_1);
    UnloadTexture(background_2);
    UnloadTexture(enemySpriteSheet);

    CloseWindow();
    return 0;
}