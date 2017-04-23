#include "BoardFactory.h"
#include <cstdio>
#include <locale>
#include <iostream>
#include <fstream>
#define SUCCESS 0 ;
#define ERROR 1 ;
#define ALLOC_ERR printf("Error: memory allocation error\n")
#define ADJ_SHIPS_ERR printf("Adjacent Ships on Board\n")
#define TOO_FEW_SHIPS_ERR(j) printf("Too few ships for player %c\n",j)
#define TOO_MANY_SHIPS_ERR(j) printf("Too many ships for player %c\n",j)
#define ILLEGAL_SHIP_ERR(s, j) printf("Wrong size or shape for ship %c for player %c\n",s,j)
#define WRONG_PATH_ERR(p) printf("Wrong path: %s\n",p)
#define MIN(a, b) ((a < b) ? (a) : (b))
/* helper methods declerations */
bool isShip(char c);
/* class methods implementation */
BoardFactory::BoardFactory(int rows, int cols, string path)
{
	//add rows and columns around board
	boardRows = rows + 2;
	boardCols = cols + 2;
	boardPath = path;
}

void BoardFactory::destroyBoard(char** board, int rows, int cols)
{
	if (board == nullptr)
	{
		return;
	}
	for (int i = 0; i < cols; i++)
	{
		free(board[i]);
		board[i] = nullptr;
	}
	free(board);
	// ReSharper disable once CppAssignedValueIsNeverUsed
	board = nullptr;
}

// ReSharper disable once CppParameterValueIsReassigned
// ReSharper disable once CppParameterValueIsReassigned
int BoardFactory::getPlayerBoards(char** boardA, char** boardB)
{
	int err;
	// allocate memory
	boardA = initBoard();
	if (boardA == nullptr)
	{
		return ERROR;
	}
	// allocate memory
	boardB = initBoard();
	if (boardB == nullptr)
	{
		destroyBoard(boardA, boardRows, boardCols);
		return ERROR;
	}
	// allocate memory
	fullBoard = initBoard();
	if (fullBoard == nullptr)
	{
		destroyBoard(boardA, boardRows, boardCols);
		destroyBoard(boardB, boardRows, boardCols);
		return ERROR;
	}
	// fill fullBoard with empty cells
	cleanBoard();
	// fill fullBoard member from path
	err = fillBoardFromFile();
	if (err)
	{
		destroyBoard(boardA, boardRows, boardCols);
		destroyBoard(boardB, boardRows, boardCols);
		destroyBoard(fullBoard, boardRows, boardCols);
		return ERROR;
	}
	// fill players' boards with their relevant ships
	fillPlayerBoards(boardA, boardB);
	destroyBoard(fullBoard, boardRows, boardCols);
	return SUCCESS;
}

BoardFactory::~BoardFactory()
{

	destroyBoard(fullBoard, boardCols, boardRows);

}

void BoardFactory::fillPlayerBoards(char** boardA, char** boardB) const
{
	char c;
	for (int i = 0; i < boardRows; i++)
	{
		for (int j = 0; j < boardCols; j++)
		{
			boardA[i][j] = EMPTY_CELL;
			boardB[i][j] = EMPTY_CELL;
			c = fullBoard[i][j];
			if (c == tolower(c))
			{
				boardB[i][j] = c;
			}
			else
			{
				boardA[i][j] = c;
			}
		}
	}
}

char** BoardFactory::initBoard() const
{
	char** board = static_cast<char**>(malloc(boardRows * sizeof(char*)));
	if (board == nullptr)
	{
		ALLOC_ERR;
		return nullptr;
	}

	for (int i = 0; i < boardRows; i++)
	{
		board[i] = static_cast<char*>(malloc(boardCols * sizeof(char)));
		if (board[i] == nullptr)
		{
			ALLOC_ERR;
			destroyBoard(board, i, boardCols);
			return nullptr;
		}
	}
	return board;

}

void BoardFactory::cleanBoard() const
{
	for (int i = 0; i < boardRows; i++) {
		for (int j = 0; j < boardCols; j++) {
			fullBoard[i][j] = EMPTY_CELL;
		}
	}
}

int BoardFactory::fillBoardFromFile() const
{
	string line;
	int row = 1, err;
	size_t m;
	ifstream file(boardPath);
	if (!file.is_open()) {
		cout << "Error: failed to open file " << boardPath << endl;
		return ERROR;
	}

	while (getline(file, line) && row <= boardRows) {
		m = MIN(boardCols, line.length());
		//cout << line << endl;
		for (int i = 1; i <= m; i++) { //1,2,3,4,5,6,7,8,9,10
			if (isShip(line[i - 1])) //check if valid ship char
				fullBoard[row][i] = line[i - 1];
		}
		row++;
	}
	err = validateBoard();
	if (err) {
		return ERROR;
	}
	return SUCCESS;
}

int BoardFactory::validateBoard() const
{
	bool err;
	char visited = 'x';
	char illgeal = 'i';
	bool isAdjShips = false;
	char ship;
	int direction, k, shipLen;
	int shipCountA = 0, shipCountB = 0;
	char** boardCpy = initBoard();
	if (boardCpy == nullptr)
	{
		return ERROR;
	}
	for (int i = 0; i < boardRows; i++) { //copy board content
		for (int j = 0; j < boardCols; j++) {
			boardCpy[i][j] = fullBoard[i][j];
		}
	}

	/*
	* mark invalid ships from board to avoid counting them in other errors
	* and if encountered illegal ships, print proper errors
	*/
	err = markInvalidShips(boardCpy);

	for (int i = 1; i < boardRows - 1; i++) {
		for (int j = 1; j < boardCols - 1; j++) {
			shipLen = 0;
			if (boardCpy[i][j] == EMPTY_CELL || boardCpy[i][j] == visited || boardCpy[i][j] == illgeal) { continue; }
			ship = boardCpy[i][j];
			boardCpy[i][j] = visited;
			shipLen++;
			direction = (boardCpy[i][j + 1] == ship) ? HORIZONTAL : VERTICAL;
			k = 1;

			if (direction == HORIZONTAL) {
				while (fullBoard[i][j + k] == ship) {
					boardCpy[i][j + k] = visited;
					shipLen++;
					k++;
				}
			}
			else if (direction == VERTICAL) {
				while (fullBoard[i + k][j] == ship) {
					boardCpy[i + k][j] = visited;
					shipLen++;
					k++;
				}
			}

			//check surroundings: checking no adjacent ships
			if (direction == HORIZONTAL) {
				for (int r = -1; r <= 1; r += 2) { //{-1,1}
					for (int c = 0; c < shipLen; c++) {
						if (boardCpy[i + r][j + c] != EMPTY_CELL) { //stumbled another ship
							isAdjShips = true;
						}
					}
				}
			}
			else if (direction == VERTICAL) {
				for (int r = 0; r < shipLen; r++) {
					for (int c = -1; c <= 1; c += 2) { //{-1,1}
						if (boardCpy[i + r][j + c] != EMPTY_CELL) { //stumbled another ship
							isAdjShips = true;
						}
					}
				}
			}

			//check which player
			if (ship == tolower(ship)) { //lowerCase = player B
				shipCountB++;
			}
			else { //upperCase = player A
				shipCountA++;
			}
		}
	}

	if (shipCountA > SHIPS_FOR_PLAYER) {
		TOO_MANY_SHIPS_ERR('A');
		err = true;
	}
	if (shipCountA < SHIPS_FOR_PLAYER) {
		TOO_FEW_SHIPS_ERR('A');
		err = true;
	}
	if (shipCountB > SHIPS_FOR_PLAYER) {
		TOO_MANY_SHIPS_ERR('B');
		err = true;
	}
	if (shipCountB < SHIPS_FOR_PLAYER) {
		TOO_FEW_SHIPS_ERR('B');
		err = true;
	}
	if (isAdjShips) {
		ADJ_SHIPS_ERR;
		err = true;
	}
	destroyBoard(boardCpy, boardCols, boardRows);
	if (err)
	{
		return ERROR;
	};
	return SUCCESS;
}

bool BoardFactory::markInvalidShips(char **boardCpy) const
{

	bool err = false;
	bool isInvalid = false; //stores if current ship is illegal in iteration
	char ship;
	int direction, k, shipLen;
	char shipTypes[NUM_SHIP_TYPES] = { RUBBER_BOAT, ROCKET_SHIP, SUBMARINE, DESTROYER };
	bool illegalShipsA[NUM_SHIP_TYPES] = { false }; //{RUBBER,ROCKET,SUBMARINE,DESTROYER}
	bool illegalShipsB[NUM_SHIP_TYPES] = { false };
	char visited = 'x'; //visited legal
	char illegal = 'i'; //illegal symbol

	for (int i = 1; i < boardRows - 1; i++) {
		for (int j = 1; j < boardCols - 1; j++) {
			shipLen = 0;
			if (boardCpy[i][j] == EMPTY_CELL || boardCpy[i][j] == visited || boardCpy[i][j] == illegal) { continue; }
			ship = boardCpy[i][j];
			boardCpy[i][j] = visited;
			shipLen++;
			direction = (boardCpy[i][j + 1] == ship) ? HORIZONTAL : VERTICAL;
			k = 1;

			if (direction == HORIZONTAL) {
				while (boardCpy[i][j + k] == ship) {
					boardCpy[i][j + k] = visited;
					shipLen++;
					k++;
				}

			}
			else if (direction == VERTICAL) {
				while (boardCpy[i + k][j] == ship) {
					boardCpy[i + k][j] = visited;
					shipLen++;
					k++;
				}
			}

			//check if ship size is valid (until now)
			int index = 0;
			switch (tolower(ship)) { //{RUBBER,ROCKET,SUBMARINE,DESTROYER}
			case RUBBER_BOAT:
				index = 0;
				isInvalid = shipLen != RUB_LEN;
				break;
			case ROCKET_SHIP:
				index = 1;
				isInvalid = shipLen != ROC_LEN;
				break;
			case SUBMARINE:
				index = 2;
				isInvalid = shipLen != SUB_LEN;
				break;
			case DESTROYER:
				index = 3;
				isInvalid = shipLen != DES_LEN;
				break;
			default:;
			}

			//check surroundings: checking valid shape
			if (direction == HORIZONTAL) {
				for (int r = -1; r <= 1; r += 2) { //{-1,1}
					for (int c = 0; c < shipLen; c++) {
						if (boardCpy[i + r][j + c] == ship) { //bad shape indeed
							boardCpy[i + r][j + c] = illegal; //mark illegal
							isInvalid = true;
						}
					}
				}
				if (isInvalid) { //current ship is illegal, mark entire shape
					for (int l = 0; l < shipLen; l++) {
						boardCpy[i][j + l] = illegal;
					}
				}
			}
			else if (direction == VERTICAL) {
				for (int r = -1; r < shipLen; r++) {
					for (int c = 0; c <= 1; c += 2) { //{-1,1}
						if (boardCpy[i + r][j + c] == ship) {
							boardCpy[i + r][j + c] = illegal;
							isInvalid = true;
						}
					}
				}

				if (isInvalid) { //mark entire shape
					for (int l = 0; l < shipLen; l++) {
						boardCpy[i + l][j] = illegal;
					}
				}
			}

			//check which player
			if (ship == tolower(ship)) { //lowerCase = player B
				illegalShipsB[index] |= isInvalid;
			}
			else { //upperCase = player A
				illegalShipsA[index] |= isInvalid;
			}
		}
	}

	//print possible errors:
	for (int i = 0; i < NUM_SHIP_TYPES; i++) {
		if (illegalShipsA[i]) {
			ILLEGAL_SHIP_ERR(toupper(shipTypes[i]), 'A');
			err = true;
		}
	}

	for (int i = 0; i < NUM_SHIP_TYPES; i++) {
		if (illegalShipsB[i]) {
			ILLEGAL_SHIP_ERR(shipTypes[i], 'B');
			err = true;
		}
	}

	for (int i = 0; i < boardRows; i++) { //remove visited symbol from legal ships, leave illegal symbols
		for (int j = 0; j < boardCols; j++) {
			if (boardCpy[i][j] == visited)
			{
				boardCpy[i][j] = fullBoard[i][j];
			}
		}
	}

	return err;
}

/* helper functions */

bool isShip(char c)
{
	char shipType[NUM_SHIP_TYPES] = { SUBMARINE, DESTROYER, RUBBER_BOAT, ROCKET_SHIP }; //types of ships
	for (int i = 0; i < NUM_SHIP_TYPES; i++) {
		if (tolower(c) == shipType[i]) {
			return true;
		}
	}
	return false;
}
