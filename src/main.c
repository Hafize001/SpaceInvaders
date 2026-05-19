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
    
    // Ekran boyutunu 1920x1080 olarak sabitledim
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

    // --- SES DOSYALARINI YÜKLE ---
    Music bgMusic = LoadMusicStream("../assets/bg_music.wav"); 
    Sound laserSound = LoadSound("../assets/laser.wav");
    Sound ufoExplosionSound = LoadSound("../assets/ufo_explosion.wav");
    Sound shipHitSound = LoadSound("../assets/ship_hit.wav");

    // Arka plan müziğini döngüye al ve ses seviyesini ayarla (%40 ses)
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
    
    // --- İSİM GİRİŞ DEĞİŞKENLERİ (BELLEK) ---
    char playerName[16] = "\0";
    int letterCount = 0;

    // Başlangıçta en yüksek skoru tablonun en tepesinden çekiyoruz
    HighScoreEntry tempEntries[5];
    LoadLeaderboard(tempEntries);
    highest_score = tempEntries[0].score;

    // --- HAZIRLIKLAR ---
    Player gemi;       
    InitPlayer(&gemi);  
    
    Enemy ordumuz[ENEMY_ROWS][ENEMY_COLS];
    InitEnemies(ordumuz,currentLevel);  
    
    Ufo ufo;
    InitUfo(&ufo);

    // Kırmızı hatayı önlemek için döngü dışında doğru isimlendirmeyle başlattık
    int lastUfoLive = ufo.active;

    Bullet bullets[MAX_BULLETS]; 
    InitBullets(bullets);

    EnemyBullet eBullets[MAX_ENEMY_BULLETS]; 
    InitEnemyBullets(eBullets);

    Vector2 mermiBoyutu = { 16.0f * 1.5f, 16.0f * 1.5f };
    float mermiHizi = 500.0f; // Saniyede 500 piksel git

    float transShipX = 0.0f, transShipY = 0.0f, transShipRot = 0.0f;
    

    while (!WindowShouldClose()) {

        // --- MÜZİK DÖNGÜSÜNÜ GÜNCELLE ---
        UpdateMusicStream(bgMusic);

        static float animProgress = 0.0f; 
        static float startX, startY;
        static float targetX, targetY;
        static float controlX, controlY; 

        switch (currentScreen) {
            case SCREEN_MENU:
                if (IsKeyPressed(KEY_ENTER)) {
                    currentScreen = SCREEN_NAME_INPUT; // Önce isim sorulacak
                }
                if (IsKeyPressed(KEY_S)) {
                    currentScreen = SCREEN_LEADERBOARD; // S ile tablo açılır
                }
                if (IsKeyPressed(KEY_Q)) return 0; 
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

                if (IsKeyPressed(KEY_ENTER)) {
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
                if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE)) {
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

                if (IsKeyPressed(KEY_P)) {
                    paused = !paused;
                    menuSelection = 0;
                }

                if (IsKeyPressed(KEY_M)) {
                    SaveToLeaderboard(playerName, score); 
                    currentScreen = SCREEN_MENU;
                    paused = false; 
                }

                if (game_over) {
                    if (IsKeyPressed(KEY_R)) { 
                        currentLevel = 1;
                        lives = 3;
                        score = 0;
                        ResetArena(&gemi, ordumuz, bullets, eBullets, &ufo, currentLevel);
                        game_over = false;
                    }
                    if (IsKeyPressed(KEY_Q)) return 0; 
                } 
                else if (victory) {
                    if (IsKeyPressed(KEY_ENTER)) { 
                        currentLevel++; 
                        ResetArena(&gemi, ordumuz, bullets, eBullets, &ufo, currentLevel);
                        victory = false;
                    }
                    if (IsKeyPressed(KEY_Q)) return 0; 
                } 
                else if (paused) {
                    if (IsKeyPressed(KEY_DOWN)) menuSelection = (menuSelection + 1) % 3;
                    if (IsKeyPressed(KEY_UP)) menuSelection = (menuSelection - 1 + 3) % 3;

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
                            SaveToLeaderboard(playerName, score); 
                            currentScreen = SCREEN_MENU;
                            paused = false;
                        }
                    }
                }

                if (!paused && !game_over && !victory) {
                    UpdatePlayer(&gemi); 
                    UpdateEnemies(ordumuz, &animTimer, &currentFrame);
                    
                    lastUfoLive = ufo.active; // Kırmızı yanan yer düzeltildi
                    UpdateUfo(&ufo, GetFrameTime());

                    // --- UFO VURULUNCA PATLAMA SESİ ÇAL ---
                    if (lastUfoLive && !ufo.active) {
                        PlaySound(ufoExplosionSound);
                    }

                    // --- MERMİ ATILINCA SES ÇAL ---
                    if (IsKeyPressed(KEY_SPACE)) { 
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
                DrawText("PRESS [S] FOR LEADERBOARD", SCREEN_WIDTH - 360, SCREEN_HEIGHT - 60, 20, GOLD);
            }
            else if (currentScreen == SCREEN_NAME_INPUT) {
                DrawFlippedMenuScreen(SCREEN_WIDTH, SCREEN_HEIGHT, &menuVisuals, false);
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 160 });

                Rectangle inputBox = { SCREEN_WIDTH/2 - 250, SCREEN_HEIGHT/2 - 80, 500, 140 };
                DrawRectangleRec(inputBox, (Color){20, 20, 35, 255});
                DrawRectangleLinesEx(inputBox, 4, LIME);

                DrawText("ENTER YOUR PILOT NAME:", SCREEN_WIDTH/2 - 190, SCREEN_HEIGHT/2 - 50, 24, RAYWHITE);
                DrawText(playerName, SCREEN_WIDTH/2 - 220, SCREEN_HEIGHT/2, 32, MAGENTA); 
                DrawText("Press ENTER to Lock In", SCREEN_WIDTH/2 - 110, SCREEN_HEIGHT/2 + 90, 18, GRAY);
            }
            else if (currentScreen == SCREEN_LEADERBOARD) {
                DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){15, 10, 35, 255}, BLACK);
                
                DrawText("GALACTIC LEADERBOARD", SCREEN_WIDTH/2 - 260, 120, 40, MAGENTA);
                DrawLineEx((Vector2){SCREEN_WIDTH/2 - 300, 180}, (Vector2){SCREEN_WIDTH/2 + 300, 180}, 3, LIME);

                HighScoreEntry leaderboard[5];
                LoadLeaderboard(leaderboard);
                for (int i = 0; i < 5; i++) {
                    Color rowColor = (i == 0) ? GOLD : (i == 1) ? SKYBLUE : RAYWHITE;
                    DrawText(TextFormat("#%d  %-15s", i + 1, leaderboard[i].name), SCREEN_WIDTH/2 - 200, 280 + (i * 75), 30, rowColor);
                    DrawText(TextFormat("%05d PTS", leaderboard[i].score), SCREEN_WIDTH/2 + 100, 280 + (i * 75), 30, rowColor);
                }

                DrawText("PRESS [M] TO RETURN TO MAIN MENU", SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT - 120, 18, GRAY);
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
                } else if (game_over) {
                    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 150 });
                    const char* over_text = "GAME OVER";
                    DrawText(over_text, (SCREEN_WIDTH - MeasureText(over_text, 40)) / 2, SCREEN_HEIGHT / 2 - 20, 40, RED);
                    DrawText("Press R to Restart or M for Menu", SCREEN_WIDTH/2 - 130, SCREEN_HEIGHT/2 + 40, 20, RAYWHITE);
                } 
                else if (victory) {
                    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 150 });
                    const char* win_text = "LEVEL CLEARED!"; 
                    DrawText(win_text, (SCREEN_WIDTH - MeasureText(win_text, 40)) / 2, SCREEN_HEIGHT / 2 - 40, 40, LIME);
                    DrawText("Press ENTER for Next Level", SCREEN_WIDTH/2 - 140, SCREEN_HEIGHT/2 + 20, 20, RAYWHITE);
                    DrawText("Press M for Main Menu", SCREEN_WIDTH/2 - 110, SCREEN_HEIGHT/2 + 60, 20, GRAY);
                }
            }
                
        EndDrawing(); 
    } 

    // --- SES TEMİZLİĞİ VE KAPANIŞ ---
    UnloadMusicStream(bgMusic);
    UnloadSound(laserSound);
    UnloadSound(ufoExplosionSound);
    UnloadSound(shipHitSound);
    CloseAudioDevice();

    // --- GÖRSEL TEMİZLİK ---
    UnloadTexture(gemi.gameShip);
    UnloadTexture(heartIcon);
    UnloadTexture(background_1);
    UnloadTexture(background_2);
    UnloadTexture(enemySpriteSheet);
    UnloadTexture(cleanTitle);
    CloseWindow();
    return 0;
}