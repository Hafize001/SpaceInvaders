#include "player.h"

// main.c içindeki değişkenleri dışarıdan kullanabilmek için buraya ekliyoruz
extern const int screen_width;
extern const int screen_height;

void InitPlayer(Player *player) {
    // SCREEN_WIDTH yerine screen_width yazıyoruz
    player->position = (Vector2){ screen_width / 2, screen_height - 50 };
    player->speed = (Vector2){ 300, 0 }; // Şimdilik hızı direkt sayı yazdım
    player->lives = 3;
}

void UpdatePlayer(Player *player) {
    if (IsKeyDown(KEY_RIGHT)) player->position.x += player->speed.x * GetFrameTime();
    if (IsKeyDown(KEY_LEFT)) player->position.x -= player->speed.x * GetFrameTime();

    if (player->position.x <= 0) player->position.x = 0;
    // Burada da küçük harf kullanıyoruz
    if (player->position.x >= screen_width - 40) player->position.x = screen_width - 40;
}

void DrawPlayer(Player *player) {
    DrawRectangleV(player->position, (Vector2){ 40, 20 }, GREEN);
}