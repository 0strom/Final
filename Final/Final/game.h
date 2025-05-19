#pragma once
#include <windows.h>

enum GameObject : short { HALL, WALL, COIN, ENEMY, BIG_COIN, PACMAN };

// user-defined structurestruct Map 
    static const int HEIGHT = 20;
    static const int WIDTH = 40;
    GameObject grid[HEIGHT][WIDTH];

    void clear();
};

void generateMap(Map& map);
void drawMap(const Map& map);
void movePacman(Map& map, COORD& pacman, int direction);
void updateGame(Map& map, HANDLE h, COORD pacman, int& score);
void startGame();