
#include <iostream>
#include <windows.h> // COORD HANDLE SetConsoleTextAttribute SetConsoleCursorPosition
#include "menu.h"
#include "game.h"
#include <conio.h>

using namespace std;

//enum GameObject : short {
//    HALL, WALL, COIN, BIG_COIN, ENEMY, PACMAN
//};
enum Color : short {
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
        char* line = new char[200];
        cout << "===== HIGH SCORES =====\n";
        while (true) {
            fgets(line, 199, file);
            if (feof(file) == 0) {
                cout << line;
            }
            else {
                break;
            }
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

        
  

    const int colorGradient[] = {
        15, 15, 14, 14, 14, 6, 6  
    };
    const int GRADIENT_SIZE = sizeof(colorGradient) / sizeof(colorGradient[0]);

    
    const char shadeGradient[] = {
        176, 177, 178, 219  // ░ → ▒ → ▓ → █
    };
    const int SHADE_SIZE = sizeof(shadeGradient) / sizeof(shadeGradient[0]);

    
    int barColors[BAR_WIDTH];
    char barChars[BAR_WIDTH];
    for (int i = 0; i < BAR_WIDTH; ++i) {
        int colorIndex = (i * GRADIENT_SIZE) / BAR_WIDTH;
        int shadeIndex = (i * SHADE_SIZE) / BAR_WIDTH;
        barColors[i] = colorGradient[colorIndex];
        barChars[i] = shadeGradient[shadeIndex]; 
    }

   
    cout << "Loading: [";
    for (int i = 0; i < BAR_WIDTH; ++i) std::cout << " ";
    cout << "]\r";
    cout.flush();

    for (int i = 0; i < BAR_WIDTH; ++i) {
        SetConsoleTextAttribute(h, barColors[i]);

       
        COORD pos;
        pos.X = 9 + i;  
        pos.Y = 0;
        SetConsoleCursorPosition(h, pos);

        
       cout << barChars[i];

        cout.flush();
        Sleep(70); 
    }


    SetConsoleTextAttribute(h, 7); 
    cout << "\nLoading complete...\n";
    Sleep(1000);
    system("cls");
}



void drawPacMan() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    const int pacManWidth = 48;
    const int pacManHeight = 7;

    const char pacMan[pacManHeight][pacManWidth + 1] = {
        "CCCCC   CCCCC   CCCCC   C    C   CCCCC   C   C",
        "C   C   C   C   C   C   CC  CC   C   C   CC  C",
        "C   C   C   C   C   C   C  C C   C   C   C C C",
        "CCcCC   CCCCC   C       C  C C   CCCCC   C  CC",
        "C       C   C   C   C   C    C   C   C   C   C",
        "C       C   C   C   C   C    C   C   C   C   C",
        "C       C   C   CCCCC   C    C   C   C   C   C"
    };

    COORD pos;
    pos.X = 10;
    pos.Y = 5;
    SetConsoleCursorPosition(h, pos);

    for (int row = 0; row < pacManHeight; ++row) {
        SetConsoleCursorPosition(h, { pos.X, static_cast<SHORT>(pos.Y + row) });
        SetConsoleTextAttribute(h, 14); 
        cout << pacMan[row];
    }

    cout << "\n-----------------------------------------------------\n";
}


// Function to show the main menu
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



void generateMap(Map& map) {
    for (int y = 0; y < Map::HEIGHT; ++y) {
        for (int x = 0; x < Map::WIDTH; ++x) {
            if (x == 0 || y == 0 || x == Map::WIDTH - 1 || y == Map::HEIGHT - 1) {
                map.grid[y][x] = GameObject::WALL;  
            }
            else if (rand() % 7 == 0) {  
                map.grid[y][x] = GameObject::WALL;
            }
            else {
                int randObj = rand() % 100;  

                if (randObj < 50)
                    map.grid[y][x] = GameObject::HALL;       // 50% 
                else if (randObj < 80)
                    map.grid[y][x] = GameObject::COIN;       // 30% 
                else if (randObj < 82)
                    map.grid[y][x] = GameObject::BIG_COIN;   // 2% 
                else if (randObj < 90)
                    map.grid[y][x] = GameObject::ENEMY;      // 8% 
                else
                    map.grid[y][x] = GameObject::HALL;       // 10% extra 
            }
        }
    }
}

void drawMap(const Map& map) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    for (int y = 0; y < Map::HEIGHT; ++y) {
        for (int x = 0; x < Map::WIDTH; ++x) {
            switch (map.grid[y][x]) {
            case GameObject::HALL:
                SetConsoleTextAttribute(h, Color::BLACK);
                cout << " ";
                break;
            case GameObject::WALL:
                SetConsoleTextAttribute(h, Color::BLUE);
                cout << (char)178; 
                break;
            case GameObject::COIN:
                SetConsoleTextAttribute(h, Color::YELLOW);
                cout << ".";  
                break;
            case GameObject::BIG_COIN:
                SetConsoleTextAttribute(h, Color::YELLOW); 
                cout << "*";
                break;
            case GameObject::ENEMY:
                SetConsoleTextAttribute(h, Color::RED);
                cout << "O";  
                break;
            case GameObject::PACMAN:
                SetConsoleTextAttribute(h, Color::YELLOW);
                cout << "C";  
                break;
            }
        }
        cout << "\n";
    }
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

    if (map.grid[pacman.Y][pacman.X] == GameObject::BIG_COIN) {
        score += 3;  // Big coins give 3 points
        map.grid[pacman.Y][pacman.X] = GameObject::HALL;
    }

    if (map.grid[pacman.Y][pacman.X] == GameObject::ENEMY) {
        SetConsoleCursorPosition(h, { 0, Map::HEIGHT });
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

// Main game loop
void startGame() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    srand(time(0));

    CONSOLE_CURSOR_INFO cursor;
    cursor.bVisible = false;
    cursor.dwSize = 100;
    SetConsoleCursorInfo(h, &cursor);

    Map map;
    generateMap(map);
    COORD pacman = { 2, 2 };
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

        movePacman(map, pacman, key);
        updateGame(map, h, pacman, score);

        SetConsoleCursorPosition(h, scorePos);
        SetConsoleTextAttribute(h, Color::GREEN);
        cout << "SCORE: ";
        SetConsoleTextAttribute(h, Color::YELLOW);
        cout << score;

        Sleep(100);
    }
}


int main() {
    showLoadingScreen();

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    system("title Pac-Man");

    // Hide the cursor
    CONSOLE_CURSOR_INFO cursor;
    cursor.bVisible = false;
    cursor.dwSize = 100;
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
            _getch();             // Wait for input before returning
            break;

        case 3:
            cout << "Exiting game.\n";
            return 0;       // Exit the program

        default:
            cout << "Invalid choice. Please try again.\n";
            Sleep(1500);
            break;
        }
    }
}