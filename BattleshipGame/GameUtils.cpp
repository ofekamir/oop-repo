#include <fstream>
#include <iostream>
#include "GameUtils.h"

#define ADJ_SHIPS_ERR printf("Adjacent Ships on Board")
#define TOO_FEW_SHIPS_ERR(j) printf("Too few ships for player %c\n",j)
#define TOO_MANY_SHIPS_ERR(j) printf("Too many ships for player %c\n",j)
#define BAD_SHIP_ERR(s, j) printf("Wrong size or shape for ship %c for player %c\n",s,j)
#define WRONG_PATH_ERR(p) printf("Wrong path: %s\n",p)
#define MISSING_BOARD_FILE_ERR(p) printf("Missing board file (*.sboard) looking in path: %s",p)
#define MISSING_ATTACK_FILE_ERR(p, j) printf("Missing attack file for player %s (*.attack-a) looking in path: %s",j,p)
#define BUF_SIZE 1024

bool isShip(char i);

void printChar2DArr(char **arr, int rows, int cols);

void fillChar2DArr(char **&arr, int rows, int cols, char c);

void free2DArr(char **arr, int rows);

void GameUtils::cleanBoard(char **&board) { fillChar2DArr(board, ACTUAL(BOARD_LEN), ACTUAL(BOARD_LEN), EMPTY_CELL); }

void GameUtils::printBoard(char **board) { printChar2DArr(board, ACTUAL(BOARD_LEN), ACTUAL(BOARD_LEN)); }

void GameUtils::destroyBoard(char **board) { free2DArr(board, ACTUAL(BOARD_LEN)); }

int GameUtils::initBoard(int len, char **(&dest)) {
    dest = (char **) malloc(len * sizeof(char *));
    if (dest == NULL) {
        ALLOCATION_ERR;
        return ERROR;
    }
    for (int i = 0; i < len; i++) {
        dest[i] = (char *) malloc(len * sizeof(char));
        if (dest[i] == NULL) {
            ALLOCATION_ERR;
            free2DArr(dest, i);
            return ERROR;
        }
    }
    return SUCCESS;
}

int GameUtils::fillBoardFromFile(char path[], char **&board) {
    string line;
    int numLines = 1, err = 0;
    ifstream file(path);
    if (file.is_open()) {
        while (getline(file, line)) {
            for (int i = 1; i < ACTUAL(BOARD_LEN) - 1; i++) {
                if (isShip(line[i]))
                    board[numLines][i] = line[i];
            }
            numLines++;
            if (numLines > BOARD_LEN) { break; }
        }
        //err = validateBoard(board);
        if (err) { return ERROR; }
        return SUCCESS;
    } else {
        WRONG_PATH_ERR(path);
        return ERROR;
    }
}

int GameUtils::validateBoard(char **board) {
    int err;
    char currShip;
    char visited = 'x';
    for (int i = 1; i < ACTUAL(BOARD_LEN) - 1; i++) {
        for (int j = 1; j < ACTUAL(BOARD_LEN) - 1; j++) {
            if (board[i][j] == visited) { continue; }
            currShip = board[i][j];
            switch (tolower(currShip)) {
                case 'b' :
                    err = checkShip(board, i, j, 'b', 1);
                    break;
                case 'p' :
                    err = checkShip(board, i, j, 'p', 2);
                    break;
                case 'm' :
                    err = checkShip(board, i, j, 'm', 3);
                    break;
                case 'd' :
                    err = checkShip(board, i, j, 'm', 4);
                    break;
                default : //'0'
                    continue;
            }

        }
    }

    //player A



    return SUCCESS;
}

int GameUtils::checkShip(char **(&board), int i, int j, char ship, int shipNum) {
    int err = 0;
    char visited = 'x', curr;
    if (shipNum == 1) { //check that radius of 1 is clear
        for (int k = -1; k <= 1; k++) {
            for (int l = -1; l <= 1; l++) {
                if (l == 0 && k == 0) continue; //the ship itself
                err |= (board[i + k][j + l] == EMPTY_CELL);
            }
        }
/*        err |= board[i][j - 1] == EMPTY_CELL;
        err |= board[i][j + 1] == EMPTY_CELL;
        err |= board[i - 1][j] == EMPTY_CELL;
        err |= board[i + 1][j] == EMPTY_CELL;
        err |= board[i - 1][j + 1] == EMPTY_CELL;
        err |= board[i + 1][j - 1] == EMPTY_CELL;
        err |= board[i + 1][j + 1] == EMPTY_CELL;
        err |= board[i - 1][j - 1] == EMPTY_CELL;*/
        if (err) { ADJ_SHIPS_ERR; }
        return err;
    } else {

        if (board[i][j + 1] == ship) { //check to the right
            board[i][j + 1] = visited;
            int k;
            for (k = 1; k < shipNum; k++) {
                curr = board[i][j + k];
                if (curr == ship) {
                    curr = visited;
                } else if (curr == EMPTY_CELL) {
                    BAD_SHIP_ERR(ship, 'A' /* todo */ );
                    return ERROR;
                } else {
                    BAD_SHIP_ERR(ship, 'A' /* todo */ );
                    ADJ_SHIPS_ERR;
                    return ERROR;
                }
            }
            if (board[i][j + k] != EMPTY_CELL) { //ship is not adjacent to empty cell
                ADJ_SHIPS_ERR;
                return ERROR;
            }
        } else if (board[i][j] == ship) { //check to the right
            board[i][j + 1] = visited;
            int k;
            for (k = 1; k < shipNum; k++) {
                curr = board[i][j + k];
                if (curr == ship) {
                    curr = visited;
                } else if (curr == EMPTY_CELL) {
                    BAD_SHIP_ERR(ship, 'A' /* todo */ );
                    return ERROR;
                } else {
                    BAD_SHIP_ERR(ship, 'A' /* todo */ );
                    ADJ_SHIPS_ERR;
                    return ERROR;
                }
            }
            if (board[i][j + k] != EMPTY_CELL) { //ship is not adjacent to empty cell
                ADJ_SHIPS_ERR;
                return ERROR;
            }
        }


    }
    if (board[i][j + 1] == ship) { //check to the right
        while (1){};
    }
    //boolean
    return 0;
}


void free2DArr(char **arr, int rows) {
    int i;
    for (i = 0; i < rows; i++) {
        free(arr[i]);
    }
    free(arr);
}

void fillChar2DArr(char **&arr, int rows, int cols, char c) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            arr[i][j] = c;
        }
        printf("\n");
    }

}

void printChar2DArr(char **arr, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c ", arr[i][j]);
        }
        printf("\n");
    }
}

bool isShip(char c) {

    char shipType[4] = {'b', 'd', 'm', 'p'}; //types of ships
    for (int i = 0; i < 4; i++) {
        if (tolower(c) == shipType[i]) {
            return true;
        }
    }
    return false;

}

