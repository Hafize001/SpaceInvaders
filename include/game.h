#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"

// 1. Düşmanların hepsi öldü mü kontrolü
bool CheckVictory(Enemy enemies[ENEMY_ROWS][ENEMY_COLS]);

// 2. Düşmanlar gemimize ulaştı mı kontrolü
bool CheckEnemyReachedPlayer(Enemy enemies[ENEMY_ROWS][ENEMY_COLS], Player *gemi);

// 3. Bölüm atladığımızda veya canımız yandığında sahneyi sıfırlama
void ResetArena(Player *gemi, Enemy enemies[ENEMY_ROWS][ENEMY_COLS], Bullet bullets[MAX_BULLETS], EnemyBullet eBullets[MAX_ENEMY_BULLETS], Ufo *ufo, int level);

#endif