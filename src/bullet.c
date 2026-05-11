#include "bullet.h"

void InitBullets(Bullet bullets[MAX_BULLETS]) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = false;
    }
}

void ShootBullet(Bullet bullets[MAX_BULLETS], Vector2 playerPos, Vector2 playerSize, Vector2 bulletSize, float bulletSpeed) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].originalSize = bulletSize;
            bullets[i].size = bulletSize;
            
            // Tam ortadan ateşleme matematiği
            bullets[i].pos.x = playerPos.x + (playerSize.x / 2.0f) - (bulletSize.x / 2.0f);
            bullets[i].pos.y = playerPos.y - bulletSize.y;
            
            bullets[i].speed = bulletSpeed;
            bullets[i].state = BULLET_FLYING; // Doğar doğmaz uçuş modunda başlar
            bullets[i].explodeTimer = 0.0f;
            bullets[i].active = true;
            break;
        }
    }
}

void UpdateBullets(Bullet bullets[MAX_BULLETS], Enemy enemies[ENEMY_ROWS][ENEMY_COLS], int *score, int *lives, Ufo *ufo) {
    float dt = GetFrameTime();

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            
            // --- DURUM 1: MERMİ UÇUYORSA ---
            if (bullets[i].state == BULLET_FLYING) {
                bullets[i].pos.y -= bullets[i].speed * dt;

                if (bullets[i].pos.y < 0) {
                    bullets[i].active = false;
                    continue; 
                }

                Rectangle bulletRec = { bullets[i].pos.x, bullets[i].pos.y, bullets[i].size.x, bullets[i].size.y };

                // 1. UFO Çarpışması
                if (ufo->active) {
                    Rectangle ufoRec = { ufo->position.x, ufo->position.y, ufo->size.x, ufo->size.y };
                    if (CheckCollisionRecs(bulletRec, ufoRec)) {
                        ufo->active = false;
                        int ufoPuan = 100 * (GetRandomValue(1, 3));
                        *score += ufoPuan;
                        // --- PATLAMA TETİKLEYİCİSİ ---
                        bullets[i].state = BULLET_EXPLODING; 
                        bullets[i].explodeTimer = 0.0f;
                        bullets[i].earnedScore = ufoPuan;
                        float scaleUp = 3.0f; // Patlama anında resim 2 kat büyür!
                        
                        // Patlamayı merminin ortasından dışa doğru genişletmek için pozisyonu geri çek
                        bullets[i].pos.x -= (bullets[i].size.x * scaleUp - bullets[i].size.x) / 2.0f;
                        bullets[i].pos.y -= (bullets[i].size.y * scaleUp - bullets[i].size.y) / 2.0f;
                        
                        bullets[i].size.x *= scaleUp;
                        bullets[i].size.y *= scaleUp;
                        continue; 
                    }
                }

                // 2. Uzaylı Ordusu Çarpışması
                bool hitEnemy = false;
                for (int r = 0; r < ENEMY_ROWS; r++) {
                    for (int c = 0; c < ENEMY_COLS; c++) {
                        if (enemies[r][c].active) {
                            Rectangle enemyRec = { enemies[r][c].position.x, enemies[r][c].position.y, enemies[r][c].size.x, enemies[r][c].size.y };
                            if (CheckCollisionRecs(bulletRec, enemyRec)) {
                                enemies[r][c].active = false;
                                *score += 10;
                                
                                // --- PATLAMA TETİKLEYİCİSİ ---
                                bullets[i].state = BULLET_EXPLODING;
                                bullets[i].explodeTimer = 0.0f;
                                bullets[i].earnedScore = 10;
                                float scaleUp = 3.0f; // 2 kat büyüme
                                
                                bullets[i].pos.x -= (bullets[i].size.x * scaleUp - bullets[i].size.x) / 2.0f;
                                bullets[i].pos.y -= (bullets[i].size.y * scaleUp - bullets[i].size.y) / 2.0f;
                                
                                bullets[i].size.x *= scaleUp;
                                bullets[i].size.y *= scaleUp;
                                
                                hitEnemy = true;
                                break; 
                            }
                        }
                    }
                    if (hitEnemy) break; 
                }
            } 
            // --- DURUM 2: MERMİ PATLIYORSA ---
            else if (bullets[i].state == BULLET_EXPLODING) {
                bullets[i].explodeTimer += dt;
                // Ekranda büyük patlama resmi sadece 0.15 saniye kalır, sonra tamamen silinir
                if (bullets[i].explodeTimer > 0.4f) { 
                    bullets[i].active = false;
                }
            }
        }
    }
}

void DrawBullets(Bullet bullets[MAX_BULLETS], Texture2D bulletTexture) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            
            // Makas kısmı: Sprite sheet'teki sarı daireyi buradan kendi resmine göre ayarla.
            // Uçarken de patlarken de AYNI resmi kesecek, sadece destRec'teki 'size' 
            // değiştiği için ekranda dev gibi görünecek.
            Rectangle sourceRec = { 8 * 2, 8 * 3, 8, 8 }; // Kendi resmine göre burayı güncelle!
            
            Rectangle destRec = { bullets[i].pos.x, bullets[i].pos.y, bullets[i].size.x, bullets[i].size.y };
            
            DrawTexturePro(bulletTexture, sourceRec, destRec, (Vector2){ 0, 0 }, 0.0f, WHITE);

            if (bullets[i].state == BULLET_EXPLODING) {
                // Zaman geçtikçe yazıyı Y ekseninde yukarı doğru kaydırır (Havalanma efekti)
                float floatUp = bullets[i].explodeTimer * 50.0f; 
                
                // Raylib'in TextFormat komutu, string içine sayı gömmemizi sağlar
                DrawText(
                    TextFormat("+%d", bullets[i].earnedScore), 
                    bullets[i].pos.x, 
                    bullets[i].pos.y - 10 - floatUp, // Resmin hemen üstünde başlar ve yukarı uçar
                    30, // Yazı boyutu
                    WHITE // Yazı rengi
                );
            }
        }
    }
}

// --- DÜŞMAN MERMİSİ SİSTEMİ ---

void InitEnemyBullets(EnemyBullet enemyBullets[MAX_ENEMY_BULLETS]) {
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        enemyBullets[i].active = false;
    }
}

bool UpdateEnemyBullets(EnemyBullet enemyBullets[MAX_ENEMY_BULLETS], Enemy enemies[ENEMY_ROWS][ENEMY_COLS], Player *gemi) {
    float dt = GetFrameTime();
    bool playerHit = false;

    // 1. DÜŞMAN YAZAY ZEKASI: Rastgele ateş etme
    // GetRandomValue(1, 60) == 1 demek, saniyede ortalama 1 kere ateş edilmesi demektir. 
    // Level 2'de bunu hızlandırabilirsin!
    if (GetRandomValue(1, 60) == 1) { 
        int rRow = GetRandomValue(0, ENEMY_ROWS - 1);
        int rCol = GetRandomValue(0, ENEMY_COLS - 1);

        if (enemies[rRow][rCol].active) {
            // Boş bir düşman mermisi bul ve ateşle
            for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
                if (!enemyBullets[i].active) {
                    enemyBullets[i].size = (Vector2){ 64.0f, 64.0f }; // Mermi boyutu
                    enemyBullets[i].pos.x = enemies[rRow][rCol].position.x + (enemies[rRow][rCol].size.x / 2.0f) - (enemyBullets[i].size.x / 2.0f);
                    enemyBullets[i].pos.y = enemies[rRow][rCol].position.y + enemies[rRow][rCol].size.y;
                    enemyBullets[i].speed = 300.0f; // Aşağı iniş hızı
                    enemyBullets[i].active = true;
                    break;
                }
            }
        }
    }

    // 2. MERMİLERİ HAREKET ETTİR VE ÇARPIŞMA KONTROLÜ
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (enemyBullets[i].active) {
            enemyBullets[i].pos.y += enemyBullets[i].speed * dt; // Aşağı gidiyor (+)

            // Ekrandan çıktıysa yuvaya dön
            if (enemyBullets[i].pos.y > SCREEN_HEIGHT) {
                enemyBullets[i].active = false;
            } else {
                // Bizim gemimize çarptı mı?
                Rectangle eBulletRec = { enemyBullets[i].pos.x, enemyBullets[i].pos.y, enemyBullets[i].size.x, enemyBullets[i].size.y };
                Rectangle playerRec = { gemi->position.x, gemi->position.y, gemi->size.x, gemi->size.y };

                if (CheckCollisionRecs(eBulletRec, playerRec)) {
                    enemyBullets[i].active = false; 
                    playerHit = true; // GEMİMİZ VURULDU! main.c'ye haber veriyoruz.
                }
            }
        }
    }
    
    return playerHit; 
}

void DrawEnemyBullets(EnemyBullet enemyBullets[MAX_ENEMY_BULLETS], Texture2D bulletTexture) {
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (enemyBullets[i].active) {
            // Pico8 Sprite Sheet'indeki düşman mermisini buradan kesiyoruz
            Rectangle sourceRec = { 8 *1, 8 * 5, 8, 8 }; // Kendi resmine göre burayı güncelleyebilirsin
            Rectangle destRec = { enemyBullets[i].pos.x, enemyBullets[i].pos.y, enemyBullets[i].size.x, enemyBullets[i].size.y };
            
            // Eğer istersen kendi mermin gibi buna da DrawTexturePro yapabilirsin, şimdilik kırmızı bir lazer çizelim:
            DrawTexturePro(bulletTexture, sourceRec, destRec, (Vector2){ 0, 0 }, 0.0f, WHITE);
        }
    }
}