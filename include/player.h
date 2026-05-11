#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

// Geminin tüm bilgilerini tutan yapı (struct)
typedef struct Player {
    Vector2 position;
    Vector2 speed;
    Vector2 size;
    int lives;
    Texture2D gameShip;
} Player;

// Fonksiyon tanımları (Prototipler)
void InitPlayer(Player *player);
void UpdatePlayer(Player *player);
void DrawPlayer(Player *player);

#endif