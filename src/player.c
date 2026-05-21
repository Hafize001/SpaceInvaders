#include "player.h"
#include "common.h"
#include <math.h> 

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

    // Klavye Kontrolleri
    if (IsKeyDown(KEY_RIGHT)) player->position.x += player->speed.x * GetFrameTime();
    if (IsKeyDown(KEY_LEFT)) player->position.x -= player->speed.x * GetFrameTime();

    // Gamepad D-PAD Kontrolleri
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) player->position.x += player->speed.x * GetFrameTime();
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) player->position.x -= player->speed.x * GetFrameTime();

    // Gamepad Analog Stick (Sol Stick)
    float gamepadX = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
    if (fabs(gamepadX) > 0.3f) { // Dead zone kontrol
        player->position.x += gamepadX * player->speed.x * GetFrameTime() * 1.2f;
    }

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