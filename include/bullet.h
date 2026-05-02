#ifndef BULLET_H
#define BULLET_H

#include "raylib.h"

// Mermi yapısı
typedef struct {
    Vector2 position;
    float speed;
    bool active;
} Bullet;

// Fonksiyon tanımları
void InitBullet(Bullet *bullet);
void UpdateBullet(Bullet *bullet, float dt);
void DrawBullet(Bullet *bullet);

#endif