#pragma once

#include <string>

#define BOARD_LEN 10
#define EMPTY_CELL '0'
#define SUCCESS 0
#define ERROR -1
#define ACTUAL(b) (b+2)
#define ALLOCATION_ERR printf("Allocation error!")
//#define B_NUM 1; //number of blocks for each ship
//#define P_NUM 2;
//#define M_NUM 3;
//#define D_NUM 4;

using namespace std;

class GameUtils {
public:
    static int fillBoardFromFile(char path[], char **(&board));

    static void printBoard(char **board);

    static void cleanBoard(char **(&board));

    static int initBoard(int len, char **(&dest));

    static void destroyBoard(char **board);

private:
    //disallow creating instance of class
    GameUtils();

    static int validateBoard(char **board);

    static int checkShip(char **(&board), int i, int j, char ship, int shipNum);
};