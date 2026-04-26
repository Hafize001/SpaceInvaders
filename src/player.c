#include "player.h"
#include "common.h" // SCREEN_WIDTH, PLAYER_SPEED buradan gelecek

void InitPlayer(Player *player) {
    // Büyük harfli olanları kullanıyoruz çünkü common.h'de öyle tanımladık
    player->position = (Vector2){ SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50 };
    player->speed = (Vector2){ PLAYER_SPEED, 0 };
    player->lives = PLAYER_LIVES;
}

void UpdatePlayer(Player *player) {
    if (IsKeyDown(KEY_RIGHT)) player->position.x += player->speed.x * GetFrameTime();
    if (IsKeyDown(KEY_LEFT)) player->position.x -= player->speed.x * GetFrameTime();

    // Sınır Kontrolü
    if (player->position.x <= 0) player->position.x = 0;
    if (player->position.x >= SCREEN_WIDTH - 40) player->position.x = SCREEN_WIDTH - 40;
}

void DrawPlayer(Player *player) {
    DrawRectangleV(player->position, (Vector2){ 40, 20 }, GREEN);
}