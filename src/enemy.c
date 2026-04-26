#include "../include/enemy.h"
#include "raylib.h"

static float enemySpeedX = 100.0f;
static int enemyDirection = 1; // 1: Sağa, -1: Sola

void InitEnemies(Enemy enemies[ENEMY_ROWS][ENEMY_COLS]) {
    int startX = 50;   // Sol taraftan başlama boşluğu
    int startY = 100;   // Yukarıdan başlama boşluğu
    int spacingX = 45; // Düşmanlar arası yatay boşluk
    int spacingY = 40; // Düşmanlar arası dikey boşluk

    for (int i = 0; i < ENEMY_ROWS; i++) {
       
        Color rowColor;
        if (i == 0) rowColor = SKYBLUE;            // En üst satır
        else if (i == 1 || i == 2) rowColor = RED; // Orta 2 satır
        else rowColor = GREEN;                     // Alt 2 satır

        for (int j = 0; j < ENEMY_COLS; j++) {
            enemies[i][j].position.x = startX + j * spacingX;
            enemies[i][j].position.y = startY + i * spacingY;
            enemies[i][j].size.x = 30; 
            enemies[i][j].size.y = 20; 
            enemies[i][j].color = rowColor;
            enemies[i][j].active = true;
        }
    }
}

void UpdateEnemies(Enemy enemies[ENEMY_ROWS][ENEMY_COLS], float dt, int screenWidth) {
    bool hitWall = false;

    
    for (int i = 0; i < ENEMY_ROWS; i++) {
        for (int j = 0; j < ENEMY_COLS; j++) {
            if (enemies[i][j].active) {
                
                float nextX = enemies[i][j].position.x + (enemySpeedX * enemyDirection * dt);
                
                
                if (nextX <= 0 || nextX + enemies[i][j].size.x >= screenWidth) {
                    hitWall = true;
                    break; 
                }
            }
        }
        if (hitWall) break;
    }

   
    for (int i = 0; i < ENEMY_ROWS; i++) {
        for (int j = 0; j < ENEMY_COLS; j++) {
            if (enemies[i][j].active) {
                if (hitWall) {
                    
                    enemies[i][j].position.y += 25; 
                } else {
                    
                    enemies[i][j].position.x += enemySpeedX * enemyDirection * dt;
                }
            }
        }
    }

    if (hitWall) {
        enemyDirection *= -1; 
    }
}

void DrawEnemies(Enemy enemies[ENEMY_ROWS][ENEMY_COLS]) {
    for (int i = 0; i < ENEMY_ROWS; i++) {
        for (int j = 0; j < ENEMY_COLS; j++) {
        
            if (enemies[i][j].active) {
                DrawRectangle(
                    (int)enemies[i][j].position.x, 
                    (int)enemies[i][j].position.y, 
                    (int)enemies[i][j].size.x, 
                    (int)enemies[i][j].size.y, 
                    enemies[i][j].color
                );
            }
        }
    }
}