#ifndef GAMESTATE_H
#define GAMESTATE_H

typedef enum GameScreen {
    SCREEN_MENU = 0,
    SCREEN_NAME_INPUT,    // İsim yazma ekranı
    SCREEN_LEADERBOARD,   // S tuşuyla açılan sıralama tablosu
    SCREEN_TRANSITION,
    SCREEN_GAMEPLAY,
    SCREEN_GAMEOVER,
    SCREEN_VICTORY
} GameScreen;

#endif