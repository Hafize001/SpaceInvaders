#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include "player.h" 
#include "common.h"
#include "enemy.h" 
#include "game.h"  
#include "bullet.h"  
#include "gamestate.h" 
#include "menu.h"
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

    Image titleImage = LoadImage("../assets/title.png"); 
    ImageColorReplace(&titleImage, BLACK, BLANK); 
    Texture2D cleanTitle = LoadTextureFromImage(titleImage);
    UnloadImage(titleImage);

    Texture2D heartIcon = LoadTexture("../assets/Heart_Pump.png");
    Texture2D background_1 = LoadTexture("../assets/Space_01-Sheet.png");
    Texture2D background_2 = LoadTexture("../assets/Space_02-Sheet.png");
    Texture2D gameShip = LoadTexture("../assets/HeartShip_Thurst_Foward.png");

    Menu_Assets menuVisuals;
    menuVisuals.background_2 = background_2; 
    menuVisuals.spriteSheet = enemySpriteSheet; 
    menuVisuals.heartIcon = heartIcon;
    menuVisuals.gameShip = gameShip;
    menuVisuals.title = cleanTitle;
    
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

    Bullet bullets[MAX_BULLETS]; 
    InitBullets(bullets);

    EnemyBullet eBullets[MAX_ENEMY_BULLETS]; 
    InitEnemyBullets(eBullets);

    Vector2 mermiBoyutu = { 16.0f * 1.5f, 16.0f * 1.5f };
    float mermiHizi = 500.0f; // Saniyede 500 piksel git

    float transShipX = 0.0f, transShipY = 0.0f, transShipRot = 0.0f;
    float animProgress = 0.0f; 
    float startX = 0.0f, startY = 0.0f;
    float targetX = 0.0f, targetY = 0.0f;
    float controlX = 0.0f, controlY = 0.0f;
    

    while (!WindowShouldClose()) {

        static float animProgress = 0.0f; 
        static float startX, startY;
        static float targetX, targetY;
        static float controlX, controlY; 

        switch (currentScreen) {
            case SCREEN_MENU:
                if (IsKeyPressed(KEY_ENTER)) {
                    float btnWidth = 64.0f * 5.0f;  
                    float btnHeight = 24.0f * 5.0f; 
                    float btnX = SCREEN_WIDTH / 2.0f - btnWidth / 2.0f;
                    float btnY = SCREEN_HEIGHT / 2.0f;

                    startX = btnX + btnWidth + 70.0f; 
                    startY = btnY + btnHeight / 2.0f;
                    
                    float gemiGercekYukseklik = 32.0f * 4.0f; 

                    targetX = gemi.position.x; 
                    targetY = gemi.position.y + (gemiGercekYukseklik / 2.0f);
                    controlX = SCREEN_WIDTH / 2.0f + 500.0f; 
                    controlY = SCREEN_HEIGHT / 2.0f + 100.0f;
                    
                    animProgress = 0.0f; 
                    currentScreen = SCREEN_TRANSITION;
                }
                if (IsKeyPressed(KEY_Q)) return 0; 
                break;

            case SCREEN_TRANSITION: 
                float speed = 1.0f / 1.5f; 
                animProgress += GetFrameTime() * speed;

                if (animProgress >= 1.0f) {
                    animProgress = 1.0f; 
                    currentScreen = SCREEN_GAMEPLAY; 
                }

                float t = animProgress;
                float u = 1.0f - t;
                float tt = t * t;
                float uu = u * u;

                transShipX = uu * startX + 2 * u * t * controlX + tt * targetX;
                transShipY = uu * startY + 2 * u * t * controlY + tt * targetY;

                transShipRot = 90.0f + (270.0f * t); 
                break;
               
            case SCREEN_GAMEPLAY:

                if (IsKeyPressed(KEY_P)) {
                    paused = !paused;
                    menuSelection = 0;
                }

                if (IsKeyPressed(KEY_M)) {
                    currentScreen = SCREEN_MENU;
                    paused = false; 
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

                    bool playerHitByBullet = UpdateEnemyBullets(eBullets, ordumuz, &gemi);
                    bool enemyReachedUs = CheckEnemyReachedPlayer(ordumuz, &gemi);

                    if (playerHitByBullet || enemyReachedUs) {
                        lives--;
                        
                        if (lives <= 0) {
                            game_over = true; 
                        } else {
                            // Can varsa sadece sahneyi sıfırla (Level ve Skor korunur)
                            ResetArena(&gemi, ordumuz, bullets, eBullets, &ufo, currentLevel);
                        }
                    }

                    if (CheckVictory(ordumuz)) {
                        victory = true;
                    }
                }
            case SCREEN_GAMEOVER:
            case SCREEN_VICTORY:
            break;
        }

        BeginDrawing();
            ClearBackground((Color){ 10, 10, 25, 255 }); 
            
            if (currentScreen == SCREEN_MENU) {
                DrawFlippedMenuScreen(SCREEN_WIDTH, SCREEN_HEIGHT, &menuVisuals, true);
            }
            else if (currentScreen == SCREEN_TRANSITION) {
                
                DrawFlippedMenuScreen(SCREEN_WIDTH, SCREEN_HEIGHT, &menuVisuals, false);
                Rectangle gsSource = { 0, 0, 32, 32 }; 
                float gsScale = 4.0f;
                Rectangle gsDest = { transShipX, transShipY, gsSource.width * gsScale, gsSource.height * gsScale };
                Vector2 gsOrigin = { gsDest.width / 2.0f, gsDest.height / 2.0f }; 
                
                DrawTexturePro(menuVisuals.gameShip, gsSource, gsDest, gsOrigin, transShipRot, WHITE);
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

                DrawBullets(bullets, enemySpriteSheet);

                if (paused) {
                    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 200 });
                    Rectangle menuBox = { SCREEN_WIDTH/2 - 125, SCREEN_HEIGHT/2 - 100, 250, 200 };
                    DrawRectangleRec(menuBox, (Color){ 30, 30, 30, 255 });
                    DrawRectangleLinesEx(menuBox, 3, MAGENTA); 

                    DrawText("PAUSE MENU", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 80, 25, RAYWHITE);
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
    UnloadTexture(cleanTitle);
    CloseWindow();
    return 0;
}