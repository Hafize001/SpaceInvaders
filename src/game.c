#include "game.h"
#include "common.h"
// Eğer ekranda aktif (yaşayan) hiçbir düşman kalmadıysa 'true' (Zafer) döndürür
bool CheckVictory(Enemy enemies[ENEMY_ROWS][ENEMY_COLS]) {
    for (int i = 0; i < ENEMY_ROWS; i++) {
        for (int j = 0; j < ENEMY_COLS; j++) {
            if (enemies[i][j].active) {
                return false; // Bir tane bile yaşıyorsa zafer yok, false dön.
            }
        }
    }
    return true; // Bütün döngü bitti ve kimse bulunamadıysa zafer!
}

// Eğer herhangi bir düşman, bizim gemimizin hizasına indiyse 'true' döndürür
bool CheckEnemyReachedPlayer(Enemy enemies[ENEMY_ROWS][ENEMY_COLS], Player *gemi) {
    for (int i = 0; i < ENEMY_ROWS; i++) {
        for (int j = 0; j < ENEMY_COLS; j++) {
            if (enemies[i][j].active && enemies[i][j].position.y + enemies[i][j].size.y >= gemi->position.y) {
                return true; // Düşman sınırı aştı!
            }
        }
    }
    return false;
}

// Oyunu, leveli veya can kaybı sonrasını sıfırlamak için tek bir komut
void ResetArena(Player *gemi, Enemy enemies[ENEMY_ROWS][ENEMY_COLS], Bullet bullets[MAX_BULLETS], EnemyBullet eBullets[MAX_ENEMY_BULLETS], Ufo *ufo, int level) {
    InitPlayer(gemi);
    InitEnemies(enemies, level);
    InitBullets(bullets);
    InitEnemyBullets(eBullets);
    InitUfo(ufo);
}