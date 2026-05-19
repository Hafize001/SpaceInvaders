#ifndef GAMESTATE_H
#define GAMESTATE_H

typedef enum GameScreen {
    SCREEN_MENU = 0,
    SCREEN_NAME_INPUT,    // Yeni: İsim girme ekranı
    SCREEN_LEADERBOARD,   // Yeni: Sıralama tablosu ekranı
    SCREEN_TRANSITION,
    SCREEN_GAMEPLAY,
    SCREEN_GAMEOVER,
    SCREEN_VICTORY
} GameScreen;

#endif