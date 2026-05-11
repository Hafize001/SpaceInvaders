#ifndef BULLET_H
#define BULLET_H

#include "raylib.h"
#include "common.h"
#include "player.h"
#include "enemy.h"

#define MAX_BULLETS 5
#define MAX_ENEMY_BULLETS 10

// --- YENİ: MERMİ DURUMLARI ---
typedef enum {
    BULLET_FLYING = 0,   // Havada ilerlerken
    BULLET_EXPLODING     // Çarptığında patlarken
} BulletState;

typedef struct Bullet {
    Vector2 pos;
    Vector2 size;
    Vector2 originalSize; // Patlamadan sonra boyutları karıştırmamak için uçuş boyutunu hafızada tutar
    float speed;
    bool active;
    BulletState state;    // Mermi şu an uçuyor mu, patlıyor mu?
    float explodeTimer;   // Patlama animasyonunun ekranda kalma süresi
    int earnedScore; 
} Bullet;

typedef struct EnemyBullet {
    Vector2 pos;
    Vector2 size;
    float speed;
    bool active;
} EnemyBullet;

void InitBullets(Bullet bullets[MAX_BULLETS]);
void ShootBullet(Bullet bullets[MAX_BULLETS], Vector2 playerPos, Vector2 playerSize, Vector2 bulletSize, float bulletSpeed);
void UpdateBullets(Bullet bullets[MAX_BULLETS], Enemy enemies[ENEMY_ROWS][ENEMY_COLS], int *score, int *lives, Ufo *ufo);
void DrawBullets(Bullet bullets[MAX_BULLETS], Texture2D bulletTexture);

void InitEnemyBullets(EnemyBullet enemyBullets[MAX_ENEMY_BULLETS]);
bool UpdateEnemyBullets(EnemyBullet enemyBullets[MAX_ENEMY_BULLETS], Enemy enemies[ENEMY_ROWS][ENEMY_COLS], Player *gemi);
void DrawEnemyBullets(EnemyBullet enemyBullets[MAX_ENEMY_BULLETS], Texture2D spriteSheet);

#endif