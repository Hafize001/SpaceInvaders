#include "enemy.h"
#include "common.h"

// 1: Sağa doğru, -1: Sola doğru
static int armadaDir = 1; 
// Saniyede kaç piksel gidecekler?
static float armadaSpeedX = 80.0f;

void InitEnemies(Enemy enemies[ENEMY_ROWS][ENEMY_COLS], int currentLevel) {
    // Ordunun ekrandaki başlangıç noktası ve aralarındaki boşluklar
    // (Bu sayıları oyun ekranının ortasına tam oturacak şekilde kendin değiştirebilirsin)
    float startX = LEFT_BOUND + 80.0f; 
    float startY = SCREEN_HEIGHT * 0.15F; 
    float spacingX = 120.0f; 
    float spacingY = 100.0f; 

    for (int i = 0; i < ENEMY_ROWS; i++) {
        for (int j = 0; j < ENEMY_COLS; j++) {
            enemies[i][j].position.x = startX + (j * spacingX);
            enemies[i][j].position.y = startY + (i * spacingY);
            enemies[i][j].size = (Vector2){ 64.0f, 64.0f }; 
            enemies[i][j].active = true;

            // --- LEVEL VE SATIR MANTIĞI ---
            // İleride 'if (currentLevel == 2)' gibi şartlar ekleyerek ordunun şeklini tamamen değiştirebilirsin!
            // Şimdilik klasik Space Invaders gibi dizelim:
            if (i == 0) {
                enemies[i][j].type = 4; // En üst satır (Örn: Sprite sheet'teki 4. sıradaki uzaylı)
            } else if (i == 1) {
                enemies[i][j].type = 5; // Orta satırlar (Örn: 2. sıradaki uzaylı)
            } else if (i == 2){
                enemies[i][j].type = 4; // En alt satırlar
            }
        }
    }
    armadaDir = 1;
    armadaSpeedX = 100.0f;
}

void UpdateEnemies(Enemy enemies[ENEMY_ROWS][ENEMY_COLS], float *animTimer, int *currentFrame) {
    float dt = GetFrameTime(); // Oyunun çalışma hızından bağımsız sabit hareket için

    // --- 1. ANİMASYON (Kolları Açıp Kapatma) ---
    *animTimer += dt;
    if (*animTimer >= 0.5f) {
        *animTimer = 0.0f;
        *currentFrame = (*currentFrame == 0) ? 1 : 0; 
    }
    // --- 2. DUVARA ÇARPMA KONTROLÜ ---
    bool hitWall = false;

    for (int i = 0; i < ENEMY_ROWS; i++) {
        for (int j = 0; j < ENEMY_COLS; j++) {
            if (enemies[i][j].active) { // Sadece yaşayan düşmanların çarpmasını umursuyoruz
                // Sağa çarpma
                if (armadaDir == 1 && enemies[i][j].position.x + enemies[i][j].size.x >= RIGHT_BOUND) {
                    hitWall = true;
                }
                // Sola çarpma
                else if (armadaDir == -1 && enemies[i][j].position.x <= LEFT_BOUND) {
                    hitWall = true;
                }
            }
        }
    }

    // --- 3. DUVARA ÇARPTIYSAN AŞAĞI İN VE TERS YÖNE GİT ---
    if (hitWall) {
        armadaDir *= -1;         // Yönü tam tersine çevir
        armadaSpeedX += 3.0f;   // KLASİK HİLE: Her duvara çarptıklarında biraz hızlanırlar!

        for (int i = 0; i < ENEMY_ROWS; i++) {
            for (int j = 0; j < ENEMY_COLS; j++) {
                enemies[i][j].position.y += 10.0f; 
            }
        }
    }

    for (int i = 0; i < ENEMY_ROWS; i++) {
        for (int j = 0; j < ENEMY_COLS; j++) {
            enemies[i][j].position.x += armadaSpeedX * armadaDir * dt;
        }
    }
}

void DrawEnemies(Enemy enemies[ENEMY_ROWS][ENEMY_COLS], Texture2D spriteSheet, int currentFrame) {
   
    float cutWidth = 8.0f;  
    float cutHeight = 8.0f; 
    float scale = 8.0f; 
    float baslangicX = 3 * cutWidth; 
    float baslangicY = 0.0f; 

    for (int i = 0; i < ENEMY_ROWS; i++) {
        for (int j = 0; j < ENEMY_COLS; j++) {
            
            if (enemies[i][j].active) {
                
                float cutX = baslangicX + (enemies[i][j].type * cutWidth);
                float cutY = baslangicY + (currentFrame * cutHeight);

                Rectangle makas = { cutX, cutY, cutWidth, cutHeight };
                Rectangle hedef = { enemies[i][j].position.x, enemies[i][j].position.y, cutWidth * scale, cutHeight * scale };

                DrawTexturePro(spriteSheet, makas, hedef, (Vector2){0, 0}, 0.0f, WHITE);
            }
        }
    }
}

void InitUfo(Ufo *ufo) {
    ufo->active = false; 
    ufo->speed = 250.0f; 
    ufo->size = (Vector2){ 90.0f, 40.0f }; 
    ufo->timer = 0.0f;
    ufo->direction = 1;  
}

void UpdateUfo(Ufo *ufo, float dt) {
    if (!ufo->active) {

        ufo->timer += dt;
        
        if (ufo->timer >= 6.0f) {
            ufo->active = true;
            ufo->timer = 0.0f; 
            
            ufo->direction *= -1; 
            
            ufo->position.y = 80.0f; 

            if (ufo->direction == 1) { // Sağa gidecekse
                ufo->position.x = LEFT_BOUND - ufo->size.x; 
            } else { // Sola gidecekse
                ufo->position.x = RIGHT_BOUND; 
            }
        }
    } else {
        // 2. UFO AKTİFSE: Hızlıca hareket ettir
        ufo->position.x += ufo->speed * ufo->direction * dt;

        // 3. EKRANDAN TAMAMEN ÇIKTIYSA: Yok et ve sayacı yeniden başlat
        if (ufo->direction == 1 && ufo->position.x > RIGHT_BOUND) {
            ufo->active = false;
        } else if (ufo->direction == -1 && ufo->position.x + ufo->size.x < LEFT_BOUND) {
            ufo->active = false;
        }
    }
}

void DrawUfo(Ufo *ufo, Texture2D spriteSheet) {
    if (ufo->active) {
        
        Rectangle makas = { 72, 32, 16, 8 }; 
        Rectangle hedef = { ufo->position.x, ufo->position.y, ufo->size.x, ufo->size.y };
        
        DrawTexturePro(spriteSheet, makas, hedef, (Vector2){0,0}, 0.0f, WHITE);
    }
}