#include <cstdlib>
#include <iostream>
#include "GameUtils.h"

using namespace std;


int main(int argc, char *argv[]) {
    char path[] = "board.txt";
    char **board;
    int err;
    err = GameUtils::initBoard(ACTUAL(BOARD_LEN), board);
    if (err) { return EXIT_FAILURE; }
    GameUtils::cleanBoard(board);
    err = GameUtils::fillBoardFromFile(path, board);
    if (err) { return EXIT_FAILURE; }
    GameUtils::printBoard(board);
    GameUtils::destroyBoard(board);
    //std::cout << "Start bafttle!" << std::endl;
    return EXIT_SUCCESS;
}
