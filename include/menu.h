#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include "gamestate.h" 
#include <stdbool.h>
typedef struct Menu_Assets {
    Texture2D background_2; 
    Texture2D spriteSheet;
    Texture2D heartIcon;
    Texture2D gameShip;
    Texture2D title;
} Menu_Assets;

void DrawFlippedMenuScreen(int screenWidth, int screenHeight, Menu_Assets *assets, bool showShip);

#endif