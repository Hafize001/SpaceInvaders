#include "player.h"
#include "common.h" // PLAYER_SPEED buradan gelecek

void InitPlayer(Player *player) {
    // Büyük harfli olanları kullanıyoruz çünkü common.h'de öyle tanımladık
    player->position = (Vector2){ SCREEN_WIDTH / 2, 0.9f * SCREEN_HEIGHT};
    player->speed = (Vector2){ PLAYER_SPEED, 0 };
    player->size = (Vector2){ 128.0f, 128.0f };
    player->lives = PLAYER_LIVES;
    player->gameShip = LoadTexture("./assets/HeartShip_Thurst_Foward.png");
}

void UpdatePlayer(Player *player) {

    
    
    if (IsKeyDown(KEY_RIGHT)) player->position.x += player->speed.x * GetFrameTime();
    if (IsKeyDown(KEY_LEFT)) player->position.x -= player->speed.x * GetFrameTime();

    // Sınır Kontrolü
    if (player->position.x < LEFT_BOUND) player->position.x = LEFT_BOUND;
    if (player->position.x > RIGHT_BOUND - player->gameShip.width) player->position.x = RIGHT_BOUND - player->gameShip.width;
                   
}

void DrawPlayer(Player *player) {
    
    float tekGemiGenisligi = (float)(player->gameShip.width) / 4; 
    float tekGemiYuksekligi = (float)(player->gameShip.height);

    Rectangle makas = { 0, 0, tekGemiGenisligi, tekGemiYuksekligi };

    float buyutmeKati = 4.0f; 
    Rectangle hedefKutu = { 
        player->position.x, 
        player->position.y, 
        tekGemiGenisligi * buyutmeKati, 
        tekGemiYuksekligi * buyutmeKati 
    };

    DrawTexturePro(player->gameShip, makas, hedefKutu, (Vector2){0, 0}, 0.0f, WHITE);
}