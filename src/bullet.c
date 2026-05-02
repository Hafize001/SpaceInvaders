#include "bullet.h"

// Mermiyi hazırlar (başlangıçta kapalı)
void InitBullet(Bullet *bullet) {
    bullet->active = false; 
    bullet->speed = 500.0f; // Mermi hızı (değiştirebilirsin)
}

// Merminin her karede ne yapacağını hesaplar
void UpdateBullet(Bullet *bullet, float dt) {
    if (bullet->active) {
        // Mermiyi yukarı (eksi y yönünde) hareket ettir
        bullet->position.y -= bullet->speed * dt;
        
        // Eğer mermi ekranın tepesinden çıkarsa pasif yap (belleği yormasın)
        if (bullet->position.y < -20) {
            bullet->active = false;
        }
    }
}

// Mermiyi ekrana çizer
void DrawBullet(Bullet *bullet) {
    if (bullet->active) {
        // Sarı renkte, ince bir mermi çizer
        DrawRectangleV(bullet->position, (Vector2){ 5, 15 }, YELLOW);
    }
}