#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "common.h"

void DrawGameplayUI(int score, int currentLevel, int highestScore, int lives, Texture2D heartIcon);
void DrawBackground(Texture2D background);

#endif