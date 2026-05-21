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

// En iyi oyuncunun adını döndüren fonksiyon
void GetTopPlayerName(char* topName) {
    HighScoreEntry entries[5];
    LoadLeaderboard(entries);
    if (strcmp(entries[0].name, "EMPTY") != 0) {
        strcpy(topName, entries[0].name);
    } else {
        strcpy(topName, "NO RECORD");
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
    
    // --- 3. BÖLÜMÜN SONUNDAKİ ZAFER GÖRSELİNİ PNG OLARAK YÜKLE ---
    Texture2D galaxySavedScreen = LoadTexture("../assets/galaxysavedekranı.png");    
    // Texture loading kontrolü
    if (galaxySavedScreen.id == 0) {
        printf("UYARI: galaxysavedekrani.png yüklenemedi!\n");
    }
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

    int lastUfoLive = ufo.active;

    Bullet bullets[MAX_BULLETS]; 
    InitBullets(bullets);

    EnemyBullet eBullets[MAX_ENEMY_BULLETS]; 
    InitEnemyBullets(eBullets);

    Vector2 mermiBoyutu = { 16.0f * 1.5f, 16.0f * 1.5f };
    float mermiHizi = 500.0f; // Saniyede 500 piksel git

    float transShipX = 0.0f, transShipY = 0.0f, transShipRot = 0.0f;
    
    HighScoreEntry displayLeaderboard[5];
    while (!WindowShouldClose()) {

        // --- MÜZİK DÖNGÜSÜNÜ GÜNCELLE ---
        UpdateMusicStream(bgMusic);

        static float animProgress = 0.0f; 
        static float startX, startY;
        static float targetX, targetY;
        static float controlX, controlY; 

        switch (currentScreen) {
            case SCREEN_MENU:
                // ENTER veya Konsol A Butonu
                if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                    currentLevel = 1;
                    lives = 3;
                    score = 0;
                    game_over = false;
                    victory = false;
                    ResetArena(&gemi, ordumuz, bullets, eBullets, &ufo, currentLevel);
                    
                    currentScreen = SCREEN_NAME_INPUT; 
                }
                // S veya Konsol X Butonu leaderboard'a gider
                if (IsKeyPressed(KEY_S) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
                    LoadLeaderboard(displayLeaderboard); 
                    currentScreen = SCREEN_LEADERBOARD; 
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

                // ENTER veya Konsol A Butonu name_input'u kitler
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
                // M Tuşu, ESC veya Konsol B Butonu menüye döndürür
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

                // P veya Konsol Menü/Start Butonu oyunu duraklatır
                if (IsKeyPressed(KEY_P) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                    paused = !paused;
                    menuSelection = 0;
                }

                // M veya Konsol B Butonu doğrudan çıkış yapar
                if (IsKeyPressed(KEY_M) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                    if (!game_over && !victory) {
                        SaveToLeaderboard(playerName, score); 
                    }
                    currentScreen = SCREEN_MENU;
                    paused = false; 
                    game_over = false; 
                    victory = false;
                }

                if (game_over) {
                    // R veya Konsol A Butonu restart atar
                    if (IsKeyPressed(KEY_R) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) { 
                        currentLevel = 1;
                        lives = 3;
                        score = 0;
                        ResetArena(&gemi, ordumuz, bullets, eBullets, &ufo, currentLevel);
                        game_over = false;
                    }
                    if (IsKeyPressed(KEY_Q)) return 0; 
                } 
                else if (victory) {
                    // 3. Bölüm sonu ekranı (Galaxy Saved) - özel kontroller
                    if (currentLevel >= 3) {
                        // ENTER veya Konsol A Butonu baştan başlama
                        if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                            currentLevel = 1;
                            lives = 3;
                            score = 0;
                            ResetArena(&gemi, ordumuz, bullets, eBullets, &ufo, currentLevel);
                            victory = false;
                        }
                        // M veya Konsol B Butonu menüye dön
                        if (IsKeyPressed(KEY_M) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                            SaveToLeaderboard(playerName, score);
                            currentScreen = SCREEN_MENU;
                            victory = false;
                        }
                        // Q tuşu oyundan çık
                        if (IsKeyPressed(KEY_Q)) return 0;
                    } else {
                        // Diğer level sonları (1 ve 2)
                        // ENTER veya Konsol A Butonu sonraki level'a
                        if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                            currentLevel++; 
                            ResetArena(&gemi, ordumuz, bullets, eBullets, &ufo, currentLevel);
                            victory = false;
                        }
                        // M veya Konsol B Butonu menüye dön
                        if (IsKeyPressed(KEY_M) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                            SaveToLeaderboard(playerName, score);
                            currentScreen = SCREEN_MENU;
                            victory = false;
                        }
                        if (IsKeyPressed(KEY_Q)) return 0;
                    }
                }
                else if (paused) {
                    // Konsol D-PAD (Yön butonları) Pause menüsünde gezinmeyi sağlar
                    if (IsKeyPressed(KEY_DOWN) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) menuSelection = (menuSelection + 1) % 3;
                    if (IsKeyPressed(KEY_UP) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP)) menuSelection = (menuSelection - 1 + 3) % 3;

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
                            currentScreen = SCREEN_MENU;
                            paused = false;
                        }
                    }
                }

                if (!paused && !game_over && !victory) {
                    // Geliştirici Geçici Test Hilesi
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

                    // SPACE veya Konsol A Butonu mermi ateşler
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
                    // Galaxy Saved Screen - Bitiş Kutlama Ekranı
                    
                    // Dinamik arka plan - yıldız efekti ile
                    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 
                        (Color){15, 5, 35, 255}, (Color){5, 2, 15, 255});
                    
                    // Yıldız efekti
                    static float starTimer = 0.0f;
                    starTimer += GetFrameTime();
                    for (int i = 0; i < 50; i++) {
                        int starX = (i * 127 + (int)(starTimer * 50)) % SCREEN_WIDTH;
                        int starY = ((i * 89 + i * i) % SCREEN_HEIGHT);
                        float starOpacity = 0.3f + 0.2f * sinf(starTimer + i);
                        DrawCircle(starX, starY, 1.5f, (Color){255, 255, 255, (unsigned char)(starOpacity * 255)});
                    }
                    
                    // Merkez başlık - GALAXY SAVED
                    const char* mainTitle = "GALAXY SAVED!";
                    int titleWidth = MeasureText(mainTitle, 80);
                    DrawText(mainTitle, SCREEN_WIDTH/2 - titleWidth/2, 100, 80, GOLD);
                    
                    // Parlak çizgi efekti
                    DrawLineEx((Vector2){SCREEN_WIDTH/2 - 300, 200}, (Vector2){SCREEN_WIDTH/2 + 300, 200}, 4, YELLOW);
                    
                    // En iyi oyuncunun adını sol tarafta
                    char topPlayerName[16];
                    GetTopPlayerName(topPlayerName);
                    
                    DrawRectangleRounded((Rectangle){50, 300, 350, 200}, 0.1f, 10, (Color){20, 10, 40, 200});
                    DrawRectangleLinesEx((Rectangle){50, 300, 350, 200}, 3, MAGENTA);
                    
                    DrawText("TOP PILOT:", 80, 330, 28, GOLD);
                    DrawText(topPlayerName, 80, 390, 40, MAGENTA);
                    
                    // Oyuncu skoru sağ tarafta
                    DrawRectangleRounded((Rectangle){SCREEN_WIDTH - 400, 300, 350, 200}, 0.1f, 10, (Color){10, 30, 20, 200});
                    DrawRectangleLinesEx((Rectangle){SCREEN_WIDTH - 400, 300, 350, 200}, 3, LIME);
                    
                    DrawText("FINAL SCORE:", SCREEN_WIDTH - 370, 330, 28, LIME);
                    DrawText(TextFormat("%05d PTS", score), SCREEN_WIDTH - 370, 390, 40, YELLOW);
                    
                    // Ortada tebrik metni
                    DrawText("CONGRATULATIONS!", SCREEN_WIDTH/2 - 180, 550, 36, (Color){255, 200, 100, 255});
                    DrawText("You have saved the galaxy!", SCREEN_WIDTH/2 - 170, 610, 28, SKYBLUE);
                    
                    // Alt kısımda menü kontrollerini göster
                    DrawRectangle(0, SCREEN_HEIGHT - 220, SCREEN_WIDTH, 220, (Color){0, 0, 0, 220});
                    DrawLineEx((Vector2){0, SCREEN_HEIGHT - 220}, (Vector2){SCREEN_WIDTH, SCREEN_HEIGHT - 220}, 3, MAGENTA);
                    
                    // Kontrol seçenekleri
                    DrawText("GAME COMPLETE!", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT - 180, 32, GOLD);
                    
                    DrawText("[ENTER] or [A] Button", SCREEN_WIDTH/2 - 300, SCREEN_HEIGHT - 120, 20, LIME);
                    DrawText("Restart from Level 1", SCREEN_WIDTH/2 - 220, SCREEN_HEIGHT - 85, 18, GRAY);
                    
                    DrawText("[M] or [B] Button", SCREEN_WIDTH/2 + 50, SCREEN_HEIGHT - 120, 20, SKYBLUE);
                    DrawText("Return to Main Menu", SCREEN_WIDTH/2 + 80, SCREEN_HEIGHT - 85, 18, GRAY);
                    
                    DrawText("[Q] to Exit", SCREEN_WIDTH - 300, SCREEN_HEIGHT - 100, 20, RED);
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
                        
                        // Gamepad kontrol bilgisi
                        DrawText("Use UP/DOWN or Gamepad D-PAD to navigate", SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT/2 + 130, 14, GRAY);
                        DrawText("Press ENTER or [A] Gamepad Button to select", SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT/2 + 155, 14, GRAY);
                    } else if (game_over) {
                        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 150 });
                        const char* over_text = "GAME OVER";
                        DrawText(over_text, (SCREEN_WIDTH - MeasureText(over_text, 40)) / 2, SCREEN_HEIGHT / 2 - 20, 40, RED);
                        DrawText("Press R or [A] Gamepad Button to Restart", SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 + 40, 20, RAYWHITE);
                        DrawText("Press M or [B] Gamepad Button for Menu", SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 + 75, 20, RAYWHITE);
                    } 
                    else if (victory) {
                        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 150 });
                        const char* win_text = "LEVEL CLEARED!"; 
                        DrawText(win_text, (SCREEN_WIDTH - MeasureText(win_text, 40)) / 2, SCREEN_HEIGHT / 2 - 40, 40, LIME);
                        DrawText("Press ENTER or [A] Gamepad Button for Next Level", SCREEN_WIDTH/2 - 240, SCREEN_HEIGHT/2 + 20, 20, RAYWHITE);
                        DrawText("Press M or [B] Gamepad Button for Main Menu", SCREEN_WIDTH/2 - 230, SCREEN_HEIGHT/2 + 60, 20, GRAY);
                    }
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
    UnloadTexture(galaxySavedScreen); 
    UnloadTexture(gemi.gameShip);
    UnloadTexture(heartIcon);
    UnloadTexture(background_1);
    UnloadTexture(background_2);
    UnloadTexture(enemySpriteSheet);
    UnloadTexture(cleanTitle);
    UnloadTexture(gameShip);
    CloseWindow();
    return 0;
}