#include "menu.h"
#include "raylib.h"

void DrawFlippedMenuScreen(int screenWidth, int screenHeight, Menu_Assets *assets, bool showShip) {
    
    // 1. Katman
    Rectangle bg1_Source = { 0, 0, 500, 500 }; 
    Rectangle bg1_Dest   = { 0, 0, screenWidth, screenHeight }; // Tüm ekrana yay
    DrawTexturePro(assets->background_2, bg1_Source, bg1_Dest, (Vector2){0,0}, 0.0f, WHITE);

    // 2. Katman
    Rectangle bg2_Source = { 0, 500, 500, 500 }; 
    Rectangle bg2_Dest   = { 0, 0, screenWidth, screenHeight }; 
    DrawTexturePro(assets->background_2, bg2_Source, bg2_Dest, (Vector2){0,0}, 0.0f, WHITE);

    // 3. Katman
    Rectangle bg3_Source = { 0, 1000, 500, 500 }; 
    Rectangle bg3_Dest   = { 0, 0, screenWidth, screenHeight }; 
    DrawTexturePro(assets->background_2, bg3_Source, bg3_Dest, (Vector2){0,0}, 0.0f, WHITE);

    // 4. Katman
    Rectangle bg4_Source = { 0, 1500, 500, 500 }; 
    Rectangle bg4_Dest   = { 0, 0, screenWidth, screenHeight }; 
    DrawTexturePro(assets->background_2, bg4_Source, bg4_Dest, (Vector2){0,0}, 0.0f, WHITE);

    // 5. Katman
    Rectangle bg5_Source = { 0, 2000, 500, 500 }; 
    Rectangle bg5_Dest   = { 0, 0, screenWidth, screenHeight }; 
    DrawTexturePro(assets->background_2, bg5_Source, bg5_Dest, (Vector2){0,0}, 0.0f, WHITE);
    
    float titleScale = 5.0f; 
    float scaledWidth = assets->title.width * titleScale;
    float titleX = screenWidth / 2.0f - scaledWidth / 2.0f; 
    float titleY = 200.0f; 
    DrawTextureEx(assets->title, (Vector2){ titleX, titleY }, 0.0f, titleScale, WHITE);
    
    Rectangle btnSource = { 32, 104, 64, 24 }; 
    float btnScale = 5.0f; 
    Rectangle btnDest = { screenWidth / 2.0f - (btnSource.width * btnScale)/2.0f, screenHeight / 2.0f, btnSource.width * btnScale, btnSource.height * btnScale };
    DrawTexturePro(assets->spriteSheet, btnSource, btnDest, (Vector2){0,0}, 0.0f, WHITE);

    const char* startText = "START";
    int fontSize = 40;
    float textWidth = MeasureText(startText, fontSize);
    DrawText(startText, screenWidth / 2.0f - textWidth / 2.0f, btnDest.y + 40.0f, fontSize, WHITE);

    Rectangle ssUfoSource = { 72, 32, 16, 8 }; 
    
    float ssUfoScale = 13.0f;
    Rectangle ssUfoDest = { screenWidth / 2.0f - 600.0f, 200.0f, ssUfoSource.width * ssUfoScale, ssUfoSource.height * ssUfoScale };
    Vector2 ssUfoOrigin = { ssUfoDest.width / 2.0f, ssUfoDest.height / 2.0f }; 
    
    float ssUfoRot = -20.0f; 
    DrawTexturePro(assets->spriteSheet, ssUfoSource, ssUfoDest, ssUfoOrigin, ssUfoRot, WHITE);

    Rectangle heartSource = { 0, 0, 16, 16 }; 
    float heartScale = 10.0f;

    Rectangle heartDest1 = { screenWidth / 2.0f + 600.0f, 200.0f, heartSource.width * heartScale, heartSource.height * heartScale };
    Vector2 heartOrigin1 = { heartDest1.width / 2.0f, heartDest1.height / 2.0f };
    DrawTexturePro(assets->heartIcon, heartSource, heartDest1, heartOrigin1, -20.0f, WHITE);

    Rectangle heartDest2 = { screenWidth / 2.0f + 650.0f, 350.0f, heartSource.width * (heartScale * 0.8f), heartSource.height * (heartScale * 0.8f) };
    Vector2 heartOrigin2 = { heartDest2.width / 2.0f, heartDest2.height / 2.0f };
    DrawTexturePro(assets->heartIcon, heartSource, heartDest2, heartOrigin2, 15.0f, WHITE);

    if (showShip) {
        Rectangle gsSource = { 0, 0, 32, 32 }; 
        float gsScale = 4.0f;
        Rectangle gsDest = { btnDest.x + btnDest.width + 70.0f, btnDest.y + btnDest.height/2.0f, gsSource.width * gsScale, gsSource.height * gsScale };
        Vector2 gsOrigin = { gsDest.width / 2.0f, gsDest.height / 2.0f };
        
        float gsRot = 90.0f; 
        DrawTexturePro(assets->gameShip, gsSource, gsDest, gsOrigin, gsRot, WHITE);
    }

    
    // Uyarı/Bilgi Yazısı (İsteğe bağlı)
    DrawText("PRESS ENTER TO START", screenWidth / 2 - 130, screenHeight - 80, 20, GRAY);
}