#include "ui.h"
#include "common.h"


#include "ui.h"

void DrawGameplayUI(int score, int currentLevel, int highestScore, int lives, Texture2D heartIcon) {
    
    // sütun Çizgileri
    DrawLine(LEFT_BOUND, 0, LEFT_BOUND, SCREEN_HEIGHT, DARKGRAY);
    DrawLine(RIGHT_BOUND, 0, RIGHT_BOUND, SCREEN_HEIGHT, DARKGRAY);

    // can ve skor
    int font_size = 0.05 * SCREEN_HEIGHT;
    const char* livesText = "LIVES";
    int livesText_width = MeasureText(livesText, font_size);
    
    DrawText("LIVES", (LEFT_BOUND - livesText_width)/2, 0.02f * SCREEN_HEIGHT, font_size, RAYWHITE);
    
    float heartScale = 3.0f; 


    float cutX = 0.0f; 
    float cutY = 0.0f; 
    float cutWidth = 16.0f; 
    float cutHeight = 16.0f; 
    
    float startX = (LEFT_BOUND - livesText_width)/2; // Ortalamak için matematik
    float startY = 0.08f * SCREEN_HEIGHT;

    for (int i = 0; i < lives; i++) {

        Rectangle sourceRec = { cutX, cutY, cutWidth, cutHeight };
        float currentHeartX = startX + (i * (cutWidth * heartScale + 10)); 
        float currentHeartY = startY;
        Rectangle destRec = { currentHeartX, currentHeartY, cutWidth * heartScale, cutHeight * heartScale };

        DrawTexturePro(heartIcon, sourceRec, destRec, (Vector2){ 0, 0 }, 0.0f, WHITE);
    }

    DrawText("SCORE", (LEFT_BOUND - livesText_width)/2, 0.20f * SCREEN_HEIGHT, font_size, RAYWHITE);
    DrawText(TextFormat("%05d", score), (LEFT_BOUND - livesText_width)/2 + 10, 0.25f * SCREEN_HEIGHT, font_size, RAYWHITE);

    const char* highScoreText = "HIGH-SCORE";
    int HighScoreText_width = MeasureText(highScoreText, font_size);

    DrawText("HIGH-SCORE", ((SCREEN_WIDTH - RIGHT_BOUND) - HighScoreText_width)/2 + RIGHT_BOUND, 0.02f * SCREEN_HEIGHT, font_size, RAYWHITE);
    DrawText(TextFormat("%05d", highestScore), ((SCREEN_WIDTH - RIGHT_BOUND) - HighScoreText_width)/2 + RIGHT_BOUND, 0.08f * SCREEN_HEIGHT, font_size, RAYWHITE);
    DrawText(TextFormat("LEVEL %02d", currentLevel), ((SCREEN_WIDTH - RIGHT_BOUND) - HighScoreText_width)/2 + RIGHT_BOUND, 0.85f * SCREEN_HEIGHT, font_size, RAYWHITE);
}


   
void DrawBackground(Texture2D background) {

    // Sırasıyla: Başlangıç X, Başlangıç Y, Kesilecek Genişlik, Kesilecek Yükseklik
    Rectangle makas1 = { 0, 0, 500, 500 }; 
    // Sırasıyla: Ekrandaki X Konumu, Ekrandaki Y Konumu, Ekranda Kaplayacağı Genişlik, Ekranda Kaplayacağı Yükseklik
    Rectangle hedef1 = { LEFT_BOUND, 0, RIGHT_BOUND - LEFT_BOUND, SCREEN_HEIGHT }; 
    DrawTexturePro(background, makas1, hedef1, (Vector2){ 0, 0 }, 0.0f, WHITE);

    Rectangle makas2 = { 0, 500, 500, 500 }; 
    Rectangle hedef2 = { LEFT_BOUND, 0, RIGHT_BOUND - LEFT_BOUND, SCREEN_HEIGHT }; 
    DrawTexturePro(background, makas2, hedef2, (Vector2){ 0, 0 }, 0.0f, WHITE);

    Rectangle makas3 = { 0, 1500, 500, 500 }; 
    Rectangle hedef3 = { LEFT_BOUND, 0, RIGHT_BOUND - LEFT_BOUND, SCREEN_HEIGHT }; 
    DrawTexturePro(background, makas3, hedef3, (Vector2){ 0, 0 }, 0.0f, WHITE);
    
    Rectangle makas4 = { 0, 2000, 500, 500 }; 
    Rectangle hedef4 = { LEFT_BOUND, 0, RIGHT_BOUND - LEFT_BOUND, SCREEN_HEIGHT }; 
    DrawTexturePro(background, makas4, hedef4, (Vector2){ 0, 0 }, 0.0f, WHITE);
}
