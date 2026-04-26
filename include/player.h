#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

// Geminin tüm bilgilerini tutan yapı (struct)
typedef struct Player {
    Vector2 position;
    Vector2 speed;
    int lives;
    Texture2D texture; // İleride resim eklemek için hazır tutuyoruz
} Player;

// Fonksiyon tanımları (Prototipler)
void InitPlayer(Player *player);
void UpdatePlayer(Player *player);
void DrawPlayer(Player *player);

#endif