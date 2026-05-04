#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "common.h"


#define ENEMY_ROWS 3
#define ENEMY_COLS 8

// Tek bir düşmanın sahip olacağı özellikler
typedef struct Enemy {
    Vector2 position;
    Vector2 size;
    bool active;   
    int type;      
} Enemy;

// Fonksiyon Prototipleri
void InitEnemies(Enemy enemies[ENEMY_ROWS][ENEMY_COLS], int currentLevel);
void UpdateEnemies(Enemy enemies[ENEMY_ROWS][ENEMY_COLS], float *animTimer, int *currentFrame);
void DrawEnemies(Enemy enemies[ENEMY_ROWS][ENEMY_COLS], Texture2D spriteSheet, int currentFrame);

// ufo gemisi
typedef struct Ufo {
    Vector2 position;
    Vector2 size;
    bool active;     // Ekranda mı?
    int direction;   // 1 (Sağa) veya -1 (Sola)
    float speed;     // Ordudan daha hızlı olacak
    float timer;     // Tekrar çıkması için geri sayım sayacı
} Ufo;

// UFO Fonksiyonları
void InitUfo(Ufo *ufo);
void UpdateUfo(Ufo *ufo, float dt);
void DrawUfo(Ufo *ufo, Texture2D spriteSheet);
#endif