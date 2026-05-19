#include "player.h"
#include "common.h" 

void InitPlayer(Player *player) {
    player->position = (Vector2){ SCREEN_WIDTH / 2, 0.9f * SCREEN_HEIGHT};
    player->speed = (Vector2){ PLAYER_SPEED, 0 };
    player->size = (Vector2){ 128.0f, 128.0f };
    player->lives = PLAYER_LIVES;
    player->gameShip = LoadTexture("../assets/HeartShip_Thurst_Foward.png");
    
    player->blinkTimer = 0.0f; 
}

void UpdatePlayer(Player *player) {

    if (player->blinkTimer > 0.0f) {
        player->blinkTimer -= GetFrameTime(); 
    }

    if (IsKeyDown(KEY_RIGHT)) player->position.x += player->speed.x * GetFrameTime();
    if (IsKeyDown(KEY_LEFT)) player->position.x -= player->speed.x * GetFrameTime();

    if (player->position.x < LEFT_BOUND) player->position.x = LEFT_BOUND;
    if (player->position.x > RIGHT_BOUND - player->gameShip.width) player->position.x = RIGHT_BOUND - player->gameShip.width;
}

void DrawPlayer(Player *player) {
    float tekGemiGenisligi = (float)(player->gameShip.width) / 4;
    float tekGemiYuksekligi = (float)(player->gameShip.height);
    Rectangle makas = { 0, 0, tekGemiGenisligi, tekGemiYuksekligi };
    float buyutmeKati = 4.0f;
    float gercekGenislik = tekGemiGenisligi * buyutmeKati;
    float gercekYukseklik = tekGemiYuksekligi * buyutmeKati;
    
    Rectangle hedefKutu = { player->position.x, player->position.y, gercekGenislik, gercekYukseklik };
    Vector2 merkez = { gercekGenislik / 2.0f, 0.0f };

    if (player->blinkTimer <= 0.0f || ((int)(player->blinkTimer * 15) % 2 == 0)) {
        DrawTexturePro(player->gameShip, makas, hedefKutu, merkez, 0.0f, WHITE);
    }
}