

#include <iostream>
#include <windows.h>
#include <conio.h>
#include "menu.h"
#include "game.h"

using namespace std;

enum Color : short { // no globals
    BLACK, DARKBLUE, DARKGREEN, TURQUOISE, DARKRED,
    PURPLE, DARKYELLOW, GREY, DARKGREY, BLUE, GREEN,
    CYAN, RED, PINK, YELLOW, WHITE
};

enum Key : short {
    LEFT = 75, RIGHT = 77, UP = 72, DOWN = 80,
    ENTER = 13, SPACE = 32, ESCAPE = 27, BACKSPACE = 8
};

void displayHighScores() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    FILE* file;
    int error_code = fopen_s(&file, "C:/Users/Denis/Desktop/highscores.txt", "r");

    if (error_code == 0 && file != nullptr) {
        char* line = new char[200];  // dynamic array
        cout << "===== HIGH SCORES =====\n";
        while (fgets(line, 199, file)) { // read line from the file and print it to the console
            cout << line;
        }
        delete[] line;
        fclose(file);
    }
    else {
        cout << "Failed to load high scores. Error code: " << error_code << "\n";
    }
}

void showLoadingScreen() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    const int BAR_WIDTH = 50;

    system("cls");
    system("title Loading Pac-Man");

    const int colorGradient[] = { 15, 15, 14, 14, 14, 6, 6 }; //  color gradient 
    const char shadeGradient[] = { 176, 177, 178, 219 }; //  a set of characters  for visual effect

    int barColors[BAR_WIDTH];
    char barChars[BAR_WIDTH];

    for (int i = 0; i < BAR_WIDTH; ++i) {
        int colorIndex = (i * sizeof(colorGradient) / sizeof(colorGradient[0])) / BAR_WIDTH;
        int shadeIndex = (i * sizeof(shadeGradient)) / BAR_WIDTH;
        barColors[i] = colorGradient[colorIndex]; // assign the corresponding color and character for this position
        barChars[i] = shadeGradient[shadeIndex];
    }

    cout << "Loading: [";
    for (int i = 0; i < BAR_WIDTH; ++i) 
        cout << " ";
    cout << "]\r";
    cout.flush();

    for (int i = 0; i < BAR_WIDTH; ++i) {
        SetConsoleTextAttribute(h, barColors[i]);
        COORD pos = { static_cast<SHORT>(9 + i), 0 };   
        // set the cursor position to the ith character inside the brackets
    // the offset 9 is because Loading is 9 characters wide
        SetConsoleCursorPosition(h, pos);
        cout << barChars[i];
        cout.flush();  // make sure character is displayed immediately
        Sleep(70);
    }

    SetConsoleTextAttribute(h, 7);
    cout << "\nLoading complete...\n";
    Sleep(1000);
    system("cls");
}

void drawPacMan() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    const char pacMan[7][49] = {
        "CCCCC   CCCCC   CCCCC   C    C   CCCCC   C   C",
        "C   C   C   C   C   C   CC  CC   C   C   CC  C",
        "C   C   C   C   C   C   C  C C   C   C   C C C",
        "CCcCC   CCCCC   C       C  C C   CCCCC   C  CC",
        "C       C   C   C   C   C    C   C   C   C   C",
        "C       C   C   C   C   C    C   C   C   C   C",
        "C       C   C   CCCCC   C    C   C   C   C   C"
    };

    COORD pos = { 10, 5 };
    for (int row = 0; row < 7; ++row) {
        SetConsoleCursorPosition(h, { pos.X, static_cast<SHORT>(pos.Y + row) });
        SetConsoleTextAttribute(h, 14);
        cout << pacMan[row];
    }

    cout << "\n-----------------------------------------------------\n";
}

void showMenu() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    cout << "\n===== PAC-MAN MENU =====\n";

    SetConsoleTextAttribute(h, 10);
    cout << "1. Start Game\n";

    SetConsoleTextAttribute(h, 11);
    cout << "2. View High Scores\n";

    SetConsoleTextAttribute(h, 12);
    cout << "3. Exit\n";

    SetConsoleTextAttribute(h, 14);
    cout << "\nChoose an option: ";
}

void movePacman(Map& map, COORD& pacman, int direction) {
    switch (direction) {
    case Key::LEFT:
        if (map.grid[pacman.Y][pacman.X - 1] != GameObject::WALL) pacman.X--;
        break;
    case Key::RIGHT:
        if (map.grid[pacman.Y][pacman.X + 1] != GameObject::WALL) pacman.X++;
        break;
    case Key::UP:
        if (map.grid[pacman.Y - 1][pacman.X] != GameObject::WALL) pacman.Y--;
        break;
    case Key::DOWN:
        if (map.grid[pacman.Y + 1][pacman.X] != GameObject::WALL) pacman.Y++;
        break;
    }
}

void updateGame(Map& map, HANDLE h, COORD pacman, int& score) {
    if (map.grid[pacman.Y][pacman.X] == GameObject::COIN) {
        score++;
        map.grid[pacman.Y][pacman.X] = GameObject::HALL;
    }
    else if (map.grid[pacman.Y][pacman.X] == GameObject::BIG_COIN) {
        score += 3;
        map.grid[pacman.Y][pacman.X] = GameObject::HALL;
    }
    else if (map.grid[pacman.Y][pacman.X] == GameObject::ENEMY) {
        SetConsoleCursorPosition(h, { 0, Map::HEIGHT }); // move the cursor below the game area
        SetConsoleTextAttribute(h, Color::RED);
        cout << "GAME OVER! Final Score: " << score << "\n";

        FILE* fileOut;
        int error_code = fopen_s(&fileOut, "C:/Users/Denis/Desktop/highscores.txt", "a");
        if (error_code == 0 && fileOut != nullptr) {
            fprintf(fileOut, "%d\n", score);
            fclose(fileOut);
        }
        else {
            cout << "Failed to save high score.\n";
        }
        exit(0);
    }
}

void startGame() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    srand(static_cast<unsigned int>(time(0))); // random generator

    CONSOLE_CURSOR_INFO cursor = { 100, FALSE }; // hide the blinking console cursor 
    SetConsoleCursorInfo(h, &cursor);

    Map map;
    generateMap(map);

    COORD pacman = { 2, 2 }; //  pac-mans starting position
    int score = 0;

    COORD scorePos = { Map::WIDTH + 2, 0 };
    SetConsoleCursorPosition(h, scorePos);
    SetConsoleTextAttribute(h, Color::GREEN);
    cout << "SCORE: ";
    SetConsoleTextAttribute(h, Color::YELLOW);
    cout << score;

    while (true) {
        SetConsoleCursorPosition(h, { 0, 0 });
        drawMap(map);

        SetConsoleCursorPosition(h, pacman);
        SetConsoleTextAttribute(h, Color::YELLOW);
        cout << "C";

        int key = _getch();
        if (key == 224) key = _getch();

        movePacman(map, pacman, key); // move pac'man based on input
        updateGame(map, h, pacman, score);

        SetConsoleCursorPosition(h, scorePos); // draw initial score display
        SetConsoleTextAttribute(h, Color::GREEN);
        cout << "SCORE: ";
        SetConsoleTextAttribute(h, Color::YELLOW);
        cout << score;

        Sleep(100);  // delay to control the game speed
    }
}

int main() {
    showLoadingScreen();

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTitle(TEXT("Pac-Man"));

    CONSOLE_CURSOR_INFO cursor = { 100, FALSE };
    SetConsoleCursorInfo(h, &cursor);

    while (true) {
        system("cls");
        drawPacMan();
        showMenu();

        int choice;
       cin >> choice;

        switch (choice) {
        case 1:
            system("cls");
            startGame();

            break;


        case 2:
            system("cls");
            displayHighScores();
            cout << "\nPress any key to return to the menu...";
            _getch();             // wait for input before returning
            break;

        case 3:
            cout << "Exiting game.\n";
            return 0;       // exit 

        default:
            cout << "Invalid choice. Please try again.\n";
            Sleep(1500);
            break;
        }
    }
}
