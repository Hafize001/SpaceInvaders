#include "raylib.h"
#include <stdio.h>
#include "player.h" // Gemi fonksiyonlarını (Init, Update, Draw) kullanabilmek için ekledik

const int screen_width = 800;  // Ekran genişliği sabitimiz
const int screen_height = 600; // Ekran yüksekliği sabitimiz

int main() {
    // Oyun penceresini oluşturur
    InitWindow(screen_width, screen_height, "Space Invaders Game");
    
    // Oyunun hızını saniyede 60 kareye sabitler
    SetTargetFPS(60);

    // --- GEMİ HAZIRLIĞI ---
    Player gemi;       // Bellekte 'gemi' adında bir oyuncu alanı açar
    InitPlayer(&gemi);  // Gemiyi başlangıç pozisyonuna (alt orta) yerleştirir
    // ----------------------

    bool paused = false;    // Oyun duraklatıldı mı kontrolü
    bool game_over = false; // Oyun bitti mi kontrolü

    // Oyun penceresi kapanana kadar dönen ana döngü
    while (!WindowShouldClose()) {

        // 'P' tuşuna basıldığında oyunu duraklat veya devam ettir
        if (IsKeyPressed(KEY_P)) {
            paused = !paused;
        }

        // Eğer oyun duraklatılmamışsa ve bitmemişse hesaplamaları yap
        if (!paused && !game_over) {
            
            float dt = GetFrameTime(); // İki kare arasındaki geçen süreyi hesaplar
            
            // --- HAREKET KONTROLÜ ---
            UpdatePlayer(&gemi); // Klavyeden gelen sağ-sol komutlarını işler
            // ------------------------
        }

        // Çizim işlemlerini başlatır
        BeginDrawing();
            ClearBackground(BLACK); // Ekranı her karede siyaha boyar (temizler)

            // Duraklatma ekranı
            if (paused) {
                const char* pause_text = "PAUSED";
                int text_width = MeasureText(pause_text, 20);
                DrawText(pause_text, (screen_width - text_width) / 2, screen_height / 2, 20, YELLOW);
            } 
            // Oyun bitti ekranı
            else if (game_over) {
                const char* over_text = "GAME OVER";
                int text_width = MeasureText(over_text, 40);
                DrawText(over_text, (screen_width - text_width) / 2, screen_height / 2, 40, RED);
            } 
            // Normal oyun akışı (Çizimler burada yapılır)
            else {
                // --- GEMİ ÇİZİMİ ---
                DrawPlayer(&gemi); // Gemiyi o anki koordinatlarına çizer
                // -------------------
                
                // Bilgilendirme yazıları
                DrawText("Raylib Calisiyor!", 300, 20, 20, RAYWHITE);
                DrawText("Oyun Motoru Aktif! Kodlar Buraya Gelecek.", 10, 10, 20, GREEN);
            }
        EndDrawing(); // Çizimi bitirip ekrana yansıtır
    }

    // Pencereyi kapatır ve kaynakları serbest bırakır
    CloseWindow();
    return 0;
}