#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"

#define ENEMY_ROWS 5
#define ENEMY_COLS 11

typedef struct {
    Vector2 position;
    Vector2 size;
    Color color;
    bool active; 
} Enemy;

void InitEnemies(Enemy enemies[ENEMY_ROWS][ENEMY_COLS]);
void UpdateEnemies(Enemy enemies[ENEMY_ROWS][ENEMY_COLS], float dt, int screenWidth);
void DrawEnemies(Enemy enemies[ENEMY_ROWS][ENEMY_COLS]);

#endif