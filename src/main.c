#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <string.h> 
#include "player.h" 
#include "common.h"
#include "enemy.h" 
#include "game.h"  
#include "bullet.h"  
#include "gamestate.h" 
#include "menu.h"
#include "ui.h"

// --- SKOR TABLOSU VERI YAPISI VE FONKSIYONLARI ---
typedef struct {
    char name[16];
    int score;
} HighScoreEntry;

// En yüksek 5 skoru dosyadan yükleyen fonksiyon
void LoadLeaderboard(HighScoreEntry entries[]) {
    for (int i = 0; i < 5; i++) {
        strcpy(entries[i].name, "EMPTY");
        entries[i].score = 0;
    }
    FILE *file = fopen("../assets/highscore.txt", "r");
    if (file != NULL) {
        for (int i = 0; i < 5; i++) {
            if (fscanf(file, "%s %d", entries[i].name, &entries[i].score) == EOF) break;
        }
        fclose(file);
    }
}

// Yeni skoru kontrol edip sıralayarak kaydeden fonksiyon
void SaveToLeaderboard(const char* name, int newScore) {
    HighScoreEntry entries[5];
    LoadLeaderboard(entries);

    char finalName[16];
    if (strlen(name) == 0) strcpy(finalName, "ANON");
    else strcpy(finalName, name);

    if (newScore > entries[4].score) {
        strcpy(entries[4].name, finalName);
        entries[4].score = newScore;

        // Bubble Sort: Büyükten küçüğe sıralama
        for (int i = 0; i < 5; i++) {
            for (int j = i + 1; j < 5; j++) {
                if (entries[j].score > entries[i].score) {
                    HighScoreEntry temp = entries[i];
                    entries[i] = entries[j];
                    entries[j] = temp;
                }
            }
        }

        // Dosyaya güncel ilk 5'i yazdır
        FILE *file = fopen("../assets/highscore.txt", "w");
        if (file != NULL) {
            for (int i = 0; i < 5; i++) {
                fprintf(file, "%s %d\n", entries[i].name, entries[i].score);
            }
            fclose(file);
        }
    }
}

int main() {
    
    // Ekran boyutunu sabitle ve başlat
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Invaders Game");
    ToggleFullscreen(); 
    SetTargetFPS(60);

    // --- SES MOTORUNU BAŞLAT ---
    InitAudioDevice();

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
    
    // Zafer ekranı arka planı
    Texture2D galaxySavedScreen = LoadTexture("../assets/galaxysaved.png");    

    // --- SES DOSYALARINI YÜKLE ---
    Music bgMusic = LoadMusicStream("../assets/bg_music.wav"); 
    Sound laserSound = LoadSound("../assets/laser.wav");
    Sound ufoExplosionSound = LoadSound("../assets/ufo_explosion.wav");
    Sound shipHitSound = LoadSound("../assets/ship_hit.wav");

    bgMusic.looping = true;
    SetMusicVolume(bgMusic, 0.4f);
    PlayMusicStream(bgMusic);

    Menu_Assets menuVisuals;
    menuVisuals.background_2 = background_2; 
    menuVisuals.spriteSheet = enemySpriteSheet; 
    menuVisuals.heartIcon = heartIcon;
    menuVisuals.gameShip = gameShip;
    menuVisuals.title = cleanTitle;
    
    int score = 0;  
    int highest_score = 0;    
    int currentLevel = 1; 
    int lives = 3;        

    bool paused = false;    
    bool game_over = false; 
    bool victory = false;   

    float animTimer = 0.0f;
    int currentFrame = 0;

    int menuSelection = 0; 
    GameScreen currentScreen = SCREEN_MENU; 
    
    char playerName[16] = "\0";
    int letterCount = 0;

    HighScoreEntry tempEntries[5];
    LoadLeaderboard(tempEntries);
    highest_score = tempEntries[0].score;

    Player gemi;       
    InitPlayer(&gemi);  
    
    Enemy ordumuz[ENEMY_ROWS][ENEMY_COLS];
    InitEnemies(ordumuz,currentLevel);  
    
    Ufo ufo;
    InitUfo(&ufo);

    int lastUfoLive = ufo.active;

    Bullet bullets[MAX_BULLETS]; 
    InitBullets(bullets);

    EnemyBullet eBullets[MAX_ENEMY_BULLETS]; 
    InitEnemyBullets(eBullets);

    Vector2 mermiBoyutu = { 16.0f * 1.5f, 16.0f * 1.5f };
    float mermiHizi = 500.0f; 

    float transShipX = 0.0f, transShipY = 0.0f, transShipRot = 0.0f;
    
    HighScoreEntry displayLeaderboard[5];
    while (!WindowShouldClose()) {

        UpdateMusicStream(bgMusic);

        static float animProgress = 0.0f; 
        static float startX, startY;
        static float targetX, targetY;
        static float controlX, controlY; 

        switch (currentScreen) {
            case SCREEN_MENU:
                if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                    currentLevel = 1;
                    lives = 3;
                    score = 0;
                    game_over = false;
                    victory = false;
                    ResetArena(&gemi, ordumuz, bullets, eBullets, &ufo, currentLevel);
                    
                    currentScreen = SCREEN_NAME_INPUT; 
                }
                // S veya X tuşu veya Gamepad X (Sağ yüz sol) butonu ile Leaderboard açma
                if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_X) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)) {
                    LoadLeaderboard(displayLeaderboard); 
                    currentScreen = SCREEN_LEADERBOARD; 
                }
                // Q tuşu veya Gamepad Y butonu ile çıkış
                if (IsKeyPressed(KEY_Q) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP)) return 0; 
                break;

            case SCREEN_NAME_INPUT: {
                int key = GetCharPressed();
                while (key > 0) {
                    if ((key >= 32) && (key <= 125) && (letterCount < 15)) {
                        playerName[letterCount] = (char)key;
                        playerName[letterCount + 1] = '\0';
                        letterCount++;
                    }
                    key = GetCharPressed();
                }

                if (IsKeyPressed(KEY_BACKSPACE)) {
                    letterCount--;
                    if (letterCount < 0) letterCount = 0;
                    playerName[letterCount] = '\0';
                }

                if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                    if (letterCount == 0) {
                        strcpy(playerName, "PILOT-X");
                    }

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
                break;
            }

            case SCREEN_LEADERBOARD:
                if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                    currentScreen = SCREEN_MENU; 
                }
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

                if (IsKeyPressed(KEY_P) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                    paused = !paused;
                    menuSelection = 0;
                }

                if (IsKeyPressed(KEY_M) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                    if (!game_over && !victory) {
                        SaveToLeaderboard(playerName, score); 
                        LoadLeaderboard(displayLeaderboard);
                        highest_score = displayLeaderboard[0].score;
                    }
                    currentScreen = SCREEN_MENU;
                    paused = false; 
                    game_over = false; 
                    victory = false;
                }

                if (game_over) {
                    if (IsKeyPressed(KEY_R) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) { 
                        currentLevel = 1;
                        lives = 3;
                        score = 0;
                        game_over = false;
                        ResetArena(&gemi, ordumuz, bullets, eBullets, &ufo, currentLevel);
                    }
                    if (IsKeyPressed(KEY_M) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                        currentScreen = SCREEN_MENU;
                        game_over = false;
                        paused = false;
                    }
                    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_X) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)) {
                        LoadLeaderboard(displayLeaderboard);
                        currentScreen = SCREEN_LEADERBOARD;
                    }
                    if (IsKeyPressed(KEY_Q) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP)) return 0; 
                } 
                else if (victory) {
                    if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) { 
                        if (currentLevel < 3) {
                            currentLevel++; 
                            ResetArena(&gemi, ordumuz, bullets, eBullets, &ufo, currentLevel);
                            victory = false;
                        } else {
                            currentLevel = 1;
                            lives = 3;
                            score = 0;
                            ResetArena(&gemi, ordumuz, bullets, eBullets, &ufo, currentLevel);
                            victory = false;
                        }
                    }
                    if (IsKeyPressed(KEY_M) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) { 
                        SaveToLeaderboard(playerName, score); 
                        LoadLeaderboard(displayLeaderboard);
                        highest_score = displayLeaderboard[0].score;
                        currentScreen = SCREEN_MENU;          
                        victory = false;                      
                    }
                    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_X) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)) { 
                        SaveToLeaderboard(playerName, score);
                        LoadLeaderboard(displayLeaderboard);
                        highest_score = displayLeaderboard[0].score;
                        currentScreen = SCREEN_LEADERBOARD;
                        victory = false;
                    }
                    if (IsKeyPressed(KEY_Q) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP)) {
                        return 0; 
                    }
                }
                else if (paused) {
                    if (IsKeyPressed(KEY_DOWN) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) menuSelection = (menuSelection + 1) % 3;
                    if (IsKeyPressed(KEY_UP) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP)) menuSelection = (menuSelection - 1 + 3) % 3;

                    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_X) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)) {
                        LoadLeaderboard(displayLeaderboard);
                        currentScreen = SCREEN_LEADERBOARD;
                    }

                    if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                        if (menuSelection == 0) paused = false;
                        else if (menuSelection == 1) {
                            currentLevel = 1;
                            lives = 3;
                            score = 0;
                            ResetArena(&gemi, ordumuz, bullets, eBullets, &ufo, currentLevel);
                            paused = false;
                        }
                        else if (menuSelection == 2) {
                            SaveToLeaderboard(playerName, score); 
                            LoadLeaderboard(displayLeaderboard);
                            highest_score = displayLeaderboard[0].score;
                            currentScreen = SCREEN_MENU;
                            paused = false;
                        }
                    }
                }

                if (!paused && !game_over && !victory) {
                    if (IsKeyPressed(KEY_U)) { 
                        victory = true;
                    }

                    UpdatePlayer(&gemi); 
                    UpdateEnemies(ordumuz, &animTimer, &currentFrame);
                    
                    lastUfoLive = ufo.active; 
                    UpdateUfo(&ufo, GetFrameTime());

                    if (lastUfoLive && !ufo.active) {
                        PlaySound(ufoExplosionSound);
                    }

                    if (IsKeyPressed(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) { 
                         ShootBullet(bullets, gemi.position, gemi.size, mermiBoyutu, mermiHizi);
                         PlaySound(laserSound);
                    }

                    UpdateBullets(bullets, ordumuz, &score, &lives, &ufo);
                    if (score > highest_score) highest_score = score;

                    bool playerHitByBullet = UpdateEnemyBullets(eBullets, ordumuz, &gemi);
                    bool enemyReachedUs = CheckEnemyReachedPlayer(ordumuz, &gemi);

                    if ((playerHitByBullet || enemyReachedUs) && gemi.blinkTimer <= 0.0f) {
                        lives--;
                        PlaySound(shipHitSound);
                        
                        if (lives <= 0) {
                            game_over = true; 
                            SaveToLeaderboard(playerName, score);
                            LoadLeaderboard(displayLeaderboard);
                            highest_score = displayLeaderboard[0].score;
                        } else {
                            gemi.position.x = SCREEN_WIDTH / 2.0f; 
                            InitBullets(bullets);
                            InitEnemyBullets(eBullets);
                            gemi.blinkTimer = 2.0f; 
                        }
                    }

                    if (CheckVictory(ordumuz)) {
                        victory = true;
                    }
                }
                break;
            default:
                break;
        }

        BeginDrawing();
            ClearBackground((Color){ 10, 10, 25, 255 }); 
            
            if (currentScreen == SCREEN_MENU) {
                DrawFlippedMenuScreen(SCREEN_WIDTH, SCREEN_HEIGHT, &menuVisuals, true);
                DrawText("PRESS [S] OR [X] GAMEPAD BUTTON FOR LEADERBOARD", SCREEN_WIDTH - 480, SCREEN_HEIGHT - 60, 20, GOLD);
            }
            else if (currentScreen == SCREEN_NAME_INPUT) {
                DrawFlippedMenuScreen(SCREEN_WIDTH, SCREEN_HEIGHT, &menuVisuals, false);
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 160 });

                Rectangle inputBox = { SCREEN_WIDTH/2 - 250, SCREEN_HEIGHT/2 - 80, 500, 140 };
                DrawRectangleRec(inputBox, (Color){20, 20, 35, 255});
                DrawRectangleLinesEx(inputBox, 4, LIME);

                DrawText("ENTER YOUR PILOT NAME:", SCREEN_WIDTH/2 - 190, SCREEN_HEIGHT/2 - 50, 24, RAYWHITE);
                DrawText(playerName, SCREEN_WIDTH/2 - 220, SCREEN_HEIGHT/2, 32, MAGENTA); 
                DrawText("Press ENTER or [A] Gamepad Button to Lock In", SCREEN_WIDTH/2 - 190, SCREEN_HEIGHT/2 + 90, 18, GRAY);
            }
            else if (currentScreen == SCREEN_LEADERBOARD) {
                DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){15, 10, 35, 255}, BLACK);

                DrawText("GALACTIC LEADERBOARD", SCREEN_WIDTH/2 - 260, 120, 40, MAGENTA);
                DrawLineEx((Vector2){SCREEN_WIDTH/2 - 300, 180}, (Vector2){SCREEN_WIDTH/2 + 300, 180}, 3, LIME);

                for (int i = 0; i < 5; i++) {
                    Color rowColor = (i == 0) ? GOLD : (i == 1) ? SKYBLUE : RAYWHITE;
                    DrawText(TextFormat("#%d  %-15s", i + 1, displayLeaderboard[i].name), SCREEN_WIDTH/2 - 200, 280 + (i * 75), 30, rowColor);
                    DrawText(TextFormat("%05d PTS", displayLeaderboard[i].score), SCREEN_WIDTH/2 + 100, 280 + (i * 75), 30, rowColor);
                }
                DrawText("PRESS [M] OR [B] GAMEPAD BUTTON TO RETURN TO MAIN MENU", SCREEN_WIDTH/2 - 340, SCREEN_HEIGHT - 120, 18, GRAY);
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
                if (victory && currentLevel >= 3) {
                    // Zafer ekranı arka planı çiziliyor
                    DrawTexture(galaxySavedScreen, 0, 0, WHITE);
                    
                    // Okunabilirliği artırmak için arka plana hafif karartma katmanı çekiyoruz
                    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 110 });

                    // Sol Üst: "BEST SCORE" (Top Pilot yerine parıltılı ve revize alan)
                    // Dinamik bir renk dalgalanması efekti ekleyelim (Parıltı hissi için)
                    float pulse = sinf(GetTime() * 4.0f) * 0.5f + 0.5f;
                    Color glowColor = (Color){ 255, (unsigned char)(150 + pulse * 105), (unsigned char)(50 + pulse * 100), 255 };

                    Rectangle scoreRecBox = { 50, 300, 380, 200 };
                    DrawRectangleRec(scoreRecBox, (Color){ 15, 10, 30, 220 }); // Kutu içi koyulaştırıldı
                    DrawRectangleLinesEx(scoreRecBox, 4, glowColor);
                    
                    // Yazı Gölgeleri (Shadow Effect) ve Kalınlaştırılmış Metinler
                    DrawText("BEST SCORE:", scoreRecBox.x + 27, scoreRecBox.y + 32, 26, BLACK);
                    DrawText("BEST SCORE:", scoreRecBox.x + 25, scoreRecBox.y + 30, 26, glowColor);
                    
                    DrawText(TextFormat("%05d PTS", highest_score), scoreRecBox.x + 27, scoreRecBox.y + 92, 34, BLACK);
                    DrawText(TextFormat("%05d PTS", highest_score), scoreRecBox.x + 25, scoreRecBox.y + 90, 34, GOLD);
                    
                    DrawText(TextFormat("PILOT: %s", playerName), scoreRecBox.x + 27, scoreRecBox.y + 152, 20, BLACK);
                    DrawText(TextFormat("PILOT: %s", playerName), scoreRecBox.x + 25, scoreRecBox.y + 150, 20, RAYWHITE);

                    // Sağ Üst: Skor Kutusu
                    Rectangle scoreBox = { SCREEN_WIDTH - 430, 300, 380, 200 };
                    DrawRectangleRec(scoreBox, (Color){ 15, 10, 30, 220 });
                    DrawRectangleLinesEx(scoreBox, 4, LIME);
                    
                    DrawText("FINAL SCORE:", scoreBox.x + 27, scoreBox.y + 32, 26, BLACK);
                    DrawText("FINAL SCORE:", scoreBox.x + 25, scoreBox.y + 30, 26, LIME);
                    
                    DrawText(TextFormat("%05d PTS", score), scoreBox.x + 27, scoreBox.y + 102, 34, BLACK);
                    DrawText(TextFormat("%05d PTS", score), scoreBox.x + 25, scoreBox.y + 100, 34, GOLD);

                    // Alt Pembe Ayraç Çizgisi
                    DrawLineEx((Vector2){ 0, SCREEN_HEIGHT - 220 }, (Vector2){ SCREEN_WIDTH, SCREEN_HEIGHT - 220 }, 5, MAGENTA);

                    // Alt Kontrol Butonları Konumlandırması
                    float btnY = SCREEN_HEIGHT - 150;
                    float btnW = 380;
                    float btnH = 90;
                    float spacing = 60;
                    float startX = (SCREEN_WIDTH - (4 * btnW + 3 * spacing)) / 2.0f;

                    // 1. Kutu: ENTER / [A] - RESTART
                    Rectangle r1 = { startX, btnY, btnW, btnH };
                    DrawRectangleRec(r1, (Color){ 20, 20, 35, 240 });
                    DrawRectangleLinesEx(r1, 3, LIME);
                    DrawRectangleRec((Rectangle){r1.x+5, r1.y+5, 110, btnH-10}, (Color){15, 15, 25, 255});
                    DrawRectangleLinesEx((Rectangle){r1.x+10, r1.y+10, 100, btnH-20}, 2, LIME);
                    DrawText("ENTER/[A]", r1.x + 15, r1.y + 33, 16, LIME);
                    DrawText("RESTART\nFROM LEVEL 1", r1.x + 135, r1.y + 22, 18, LIME);

                    // 2. Kutu: M / [B] - MAIN MENU
                    Rectangle r2 = { startX + btnW + spacing, btnY, btnW, btnH };
                    DrawRectangleRec(r2, (Color){ 20, 20, 35, 240 });
                    DrawRectangleLinesEx(r2, 3, SKYBLUE);
                    DrawRectangleRec((Rectangle){r2.x+5, r2.y+5, 100, btnH-10}, (Color){15, 15, 25, 255});
                    DrawRectangleLinesEx((Rectangle){r2.x+10, r2.y+10, 80, btnH-20}, 2, SKYBLUE);
                    DrawText("M / [B]", r2.x + 20, r2.y + 33, 18, SKYBLUE);
                    DrawText("RETURN TO\nMAIN MENU", r2.x + 125, r2.y + 22, 18, SKYBLUE);

                    // 3. Kutu: S / [X] - LEADERBOARD
                    Rectangle r3 = { startX + 2 * (btnW + spacing), btnY, btnW, btnH };
                    DrawRectangleRec(r3, (Color){ 20, 20, 35, 240 });
                    DrawRectangleLinesEx(r3, 3, GOLD);
                    DrawRectangleRec((Rectangle){r3.x+5, r3.y+5, 100, btnH-10}, (Color){15, 15, 25, 255});
                    DrawRectangleLinesEx((Rectangle){r3.x+10, r3.y+10, 80, btnH-20}, 2, GOLD);
                    DrawText("S / [X]", r3.x + 22, r3.y + 33, 18, GOLD);
                    DrawText("VIEW\nLEADERBOARD", r3.x + 125, r3.y + 22, 18, GOLD);

                    // 4. Kutu: Q / [Y] - EXIT TO DESKTOP
                    Rectangle r4 = { startX + 3 * (btnW + spacing), btnY, btnW, btnH };
                    DrawRectangleRec(r4, (Color){ 20, 20, 35, 240 });
                    DrawRectangleLinesEx(r4, 3, RED);
                    DrawRectangleRec((Rectangle){r4.x+5, r4.y+5, 100, btnH-10}, (Color){15, 15, 25, 255});
                    DrawRectangleLinesEx((Rectangle){r4.x+10, r4.y+10, 80, btnH-20}, 2, RED);
                    DrawText("Q / [Y]", r4.x + 18, r4.y + 33, 18, RED);
                    DrawText("EXIT TO\nDESKTOP", r4.x + 125, r4.y + 22, 18, RED);

                } else {
                    DrawBackground(background_1);

                    DrawPlayer(&gemi); 
                    DrawUfo(&ufo, enemySpriteSheet);
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
                        DrawText(menuSelection == 1 ? "> RESTART" : "  RESTART", SCREEN_WIDTH/2 - 60, SCREEN_HEIGHT/2 + 20, 20, menuSelection == 1 ? LIME : GRAY);
                        DrawText(menuSelection == 2 ? "> MAIN MENU" : "  MAIN MENU", SCREEN_WIDTH/2 - 60, SCREEN_HEIGHT/2 + 60, 20, menuSelection == 2 ? LIME : GRAY);
                    } else if (game_over) {
                        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 150 });
                        const char* over_text = "GAME OVER";
                        DrawText(over_text, (SCREEN_WIDTH - MeasureText(over_text, 40)) / 2, SCREEN_HEIGHT / 2 - 20, 40, RED);
                        DrawText("Press R or [A] Gamepad Button to Restart", SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 + 40, 20, RAYWHITE);
                        DrawText("Press M or [B] Gamepad Button for Menu", SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 + 75, 20, RAYWHITE);
                        DrawText("Press S or [X] Gamepad Button for Leaderboard", SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 + 110, 20, RAYWHITE);
                        DrawText("Press Q or [Y] Gamepad Button to Exit", SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 + 145, 20, RAYWHITE);
                    } 
                    else if (victory) {
                        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 150 });
                        const char* win_text = "LEVEL CLEARED!"; 
                        DrawText(win_text, (SCREEN_WIDTH - MeasureText(win_text, 40)) / 2, SCREEN_HEIGHT / 2 - 40, 40, LIME);
                        DrawText("Press ENTER or [A] Gamepad Button for Next Level", SCREEN_WIDTH/2 - 240, SCREEN_HEIGHT/2 + 20, 20, RAYWHITE);
                        DrawText("Press M or [B] Gamepad Button for Main Menu", SCREEN_WIDTH/2 - 230, SCREEN_HEIGHT/2 + 60, 20, GRAY);
                        DrawText("Press S or [X] Gamepad Button for Leaderboard", SCREEN_WIDTH/2 - 230, SCREEN_HEIGHT/2 + 100, 20, GRAY);
                        DrawText("Press Q or [Y] Gamepad Button to Exit", SCREEN_WIDTH/2 - 230, SCREEN_HEIGHT/2 + 140, 20, GRAY);
                    }
                }
            }
                
        EndDrawing(); 
    } 

    // --- TEMİZLİK ---
    UnloadMusicStream(bgMusic);
    UnloadSound(laserSound);
    UnloadSound(ufoExplosionSound);
    UnloadSound(shipHitSound);
    CloseAudioDevice();

    UnloadTexture(galaxySavedScreen); 
    UnloadTexture(heartIcon);
    UnloadTexture(background_1);
    UnloadTexture(background_2);
    UnloadTexture(enemySpriteSheet);
    UnloadTexture(cleanTitle);
    UnloadTexture(gameShip);

    CloseWindow();
    return 0;
}