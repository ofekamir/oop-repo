#include <fstream>
#include <iostream>
#include "GameUtils.h"
#include <string>
#include <sstream>
#include <vector>

#define ADJ_SHIPS_ERR printf("Adjacent Ships on Board\n")
#define TOO_FEW_SHIPS_ERR(j) printf("Too few ships for player %c\n",j)
#define TOO_MANY_SHIPS_ERR(j) printf("Too many ships for player %c\n",j)
#define ILLEGAL_SHIP_ERR(s, j) printf("Wrong size or shape for ship %c for player %c\n",s,j)
#define WRONG_PATH_ERR(p) printf("Wrong path: %s\n",p)
#define MIN(a, b) ((a < b) ? (a) : (b))
#define MAX_PATH 1024
#define SEARCH_DEFAULT_CMD "dir /b /a-d > files.txt 2> nul"
#define WRONG_PATH "Wrong path: "
#define MISSING_BOARD_FILE "Missing board file (*.sboard) looking in path: "
#define MISSING_ATTACK_A_FILE "Missing attack file for player A (*.attack-a) looking in path: "
#define MISSING_ATTACK_B_FILE "Missing attack file for player B (*.attack-b) looking in path: "
#define FILES_FILE_NAME "files.txt"
#define BOARD_EXTENSION "sboard"
#define ATTACK_A_EXTENSION "attack-a"
#define ATTACK_B_EXTENSION "attack-b"
#define WORKING_DIR "Working Directory"

void GameUtils::cleanBoard(char board[FULL_BOARD_LEN][FULL_BOARD_LEN]) {
	for (int i = 0; i < FULL_BOARD_LEN; i++) {
		for (int j = 0; j < FULL_BOARD_LEN; j++) {
			board[i][j] = EMPTY_CELL;
		}
	}
}

void GameUtils::printBoard(char board[FULL_BOARD_LEN][FULL_BOARD_LEN], bool fullPrint) {
	int start, end;
	start = fullPrint ? 0 : 1;
	end = fullPrint ? FULL_BOARD_LEN : BOARD_LEN + 1;
	for (int i = start; i < end; i++) {
		for (int j = start; j < end; j++) {
			if (board[i][j] == ' ')
			{
				printf("*");
			}
			else { printf("%c", board[i][j]); }
		}
		printf("\n");
	}
}


int GameUtils::fillBoardFromFile(string path, char board[FULL_BOARD_LEN][FULL_BOARD_LEN]) {
	string line;
	int row = 1, err = 0;
	size_t m = 0;
	ifstream file(path);
	if (!file.is_open()) {
		cout << "Error: failed to open file " << path << endl;
		return ERROR;
	}

	while (getline(file, line) && row <= BOARD_LEN) {
		m = MIN(BOARD_LEN, line.length());
		//cout << line << endl;
		for (int i = 1; i <= m; i++) { //1,2,3,4,5,6,7,8,9,10
			if (isShip(line[i - 1])) //check if valid ship char
				board[row][i] = line[i - 1];
		}
		row++;
	}
	err = validateBoard(board);
	if (err) {
		return ERROR;
	}
	return SUCCESS;
}

int GameUtils::validateBoard(char board[FULL_BOARD_LEN][FULL_BOARD_LEN]) {
	bool err = false;
	char visited = 'x';
	char illgeal = 'i';
	bool isAdjShips = false;
	char ship;
	int direction, k, shipLen;
	int shipCountA = 0, shipCountB = 0;
	char boardCpy[FULL_BOARD_LEN][FULL_BOARD_LEN];

	for (int i = 0; i < FULL_BOARD_LEN; i++) { //copy board content
		for (int j = 0; j < FULL_BOARD_LEN; j++) {
			boardCpy[i][j] = board[i][j];
		}
	}

	/*
	* mark invalid ships from board to avoid counting them in other errors
	* and if encountered illegal ships, print proper errors
	*/
	err = markInvalidShips(boardCpy, board);

	for (int i = 1; i < FULL_BOARD_LEN - 1; i++) {
		for (int j = 1; j < FULL_BOARD_LEN - 1; j++) {
			shipLen = 0;
			if (boardCpy[i][j] == EMPTY_CELL || boardCpy[i][j] == visited || boardCpy[i][j] == illgeal) { continue; }
			ship = boardCpy[i][j];
			boardCpy[i][j] = visited;
			shipLen++;
			direction = (boardCpy[i][j + 1] == ship) ? HORIZONTAL : VERTICAL;
			k = 1;

			if (direction == HORIZONTAL) {
				while (board[i][j + k] == ship) {
					boardCpy[i][j + k] = visited;
					shipLen++;
					k++;
				}
			}
			else if (direction == VERTICAL) {
				while (board[i + k][j] == ship) {
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

	return err ? ERROR : SUCCESS;
}

bool GameUtils::markInvalidShips(char boardCpy[FULL_BOARD_LEN][FULL_BOARD_LEN],
	char board[FULL_BOARD_LEN][FULL_BOARD_LEN]) {

	bool err = false;
	bool isInvalid = false; //stores if current ship is illegal in iteration
	char ship;
	int direction, k, shipLen;
	char shipTypes[NUM_SHIP_TYPES] = { RUBBER_BOAT, ROCKET_SHIP, SUBMARINE, DESTROYER };
	bool illegalShipsA[NUM_SHIP_TYPES] = { false }; //{RUBBER,ROCKET,SUBMARINE,DESTROYER}
	bool illegalShipsB[NUM_SHIP_TYPES] = { false };
	char visited = 'x'; //visited legal
	char illegal = 'i'; //illegal symbol

	for (int i = 1; i < FULL_BOARD_LEN - 1; i++) {
		for (int j = 1; j < FULL_BOARD_LEN - 1; j++) {
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

	for (int i = 0; i < FULL_BOARD_LEN; i++) { //remove visited symbol from legal ships, leave illegal symbols
		for (int j = 0; j < FULL_BOARD_LEN; j++) {
			if (boardCpy[i][j] == visited)
			{
				boardCpy[i][j] = board[i][j];
			}
		}
	}

	return err;
}


bool GameUtils::isShip(char c)
{
	char shipType[NUM_SHIP_TYPES] = { SUBMARINE, DESTROYER, RUBBER_BOAT, ROCKET_SHIP }; //types of ships
	for (int i = 0; i < NUM_SHIP_TYPES; i++) {
		if (tolower(c) == shipType[i]) {
			return true;
		}
	}
	return false;
}


void GameUtils::fillPlayerBoard(int player, char playerBoard[FULL_BOARD_LEN][FULL_BOARD_LEN], char fullBoard[FULL_BOARD_LEN][FULL_BOARD_LEN])
{
	char c;
	for (int i = 0; i < FULL_BOARD_LEN; i++)
	{
		for (int j = 0; j < FULL_BOARD_LEN; j++)
		{
			c = fullBoard[i][j];
			if (!isShip(c)) { continue; }
			if ((player == 0 && c != tolower(c)) || (player == 1 && c == tolower(c)))
			{
				playerBoard[i][j] = c;
			}
		}
	}
}



int GameUtils::getInputFiles(vector<string> & inputFiles, vector<string> & messages, int argc, char *argv[])
{
	string searchDir = argc == 1 ? "" : argv[1];
	int op_res = fetchInputFiles(inputFiles, messages, searchDir);
	if (op_res == 1)
	{
		cout << "Error: failed to fetch input files from super-file container" << endl;
		return ERROR;
	}
	if (messages.size() != 0)
	{
		showMessages(messages);
		return ERROR;
	}
	return SUCCESS;
}

void GameUtils::showMessages(vector<string> messages)
{
	for (vector<string>::iterator it = messages.begin(); it != messages.end(); ++it)
	{
		cout << *it << endl;
	}
}

int GameUtils::fetchInputFiles(vector<string> & inputFiles, vector<string> & messages, const string path)
{
	int op_res = 0, hasBoardFile = 0, hasAAttackFile = 0, hasBAttackFile = 0;
	size_t delimIndex = 0;
	const char * searchCmd;
	string s_search_cmd, pathToFiles, line, fileExtension;
	ostringstream tmpSearchCmd, tmpPathToFiles, wrongPath, missingBoardFile, missingAAtackFile, missingBAttckFile, fullPath;
	ifstream inputFilesContainer;
	vector<string> tmpInputFiles;

	if (path.empty() == true)
	{
		//prepare messsages
		wrongPath << WRONG_PATH << WORKING_DIR << endl;
		missingBoardFile << MISSING_BOARD_FILE << WORKING_DIR << endl;
		missingAAtackFile << MISSING_ATTACK_A_FILE << WORKING_DIR << endl;
		missingBAttckFile << MISSING_ATTACK_B_FILE << WORKING_DIR << endl;
		op_res = system(SEARCH_DEFAULT_CMD);
		if (1 == op_res)
		{
			messages.push_back(wrongPath.str());
			return SUCCESS;
		}
	}
	else
	{
		fullPath << path << "\\";
		//prepare messsages
		wrongPath << WRONG_PATH << path << endl;
		missingBoardFile << MISSING_BOARD_FILE << path << endl;
		missingAAtackFile << MISSING_ATTACK_A_FILE << path << endl;
		missingBAttckFile << MISSING_ATTACK_B_FILE << path << endl;
		/* build the dir command: with arguments /b /a-d. if this command fail(op res == 1) there are 2 options:
		1. wrong or non-exist path - in this case present wrong path messasge and return.
		2. the dir exist but it is empty  - in this case we need to collect the "missing files warnings" and return,
		therfore we need to continue and open the output file.
		*/
		tmpSearchCmd << "dir \"" << path << "\" /b /a-d > files.txt 2> nul";
		s_search_cmd = tmpSearchCmd.str();
		searchCmd = s_search_cmd.c_str();
		op_res = system(searchCmd);
		if (1 == op_res)
		{
			// build the new dir command: with arguments /b /a. if also this command fail(op res == 1) than it's a wrong path
			tmpSearchCmd.str("");
			tmpSearchCmd << "dir" << path << " /b /a > files.txt 2> nul";
			s_search_cmd = tmpSearchCmd.str();
			searchCmd = s_search_cmd.c_str();
			op_res = system(searchCmd);
			if (1 == op_res)
			{
				messages.push_back(wrongPath.str());
				return SUCCESS;
			}
		}
	}
	//in this point we act in the same logic for the two cases
	inputFilesContainer.open("files.txt");
	if (false == inputFilesContainer.is_open())
	{
		cout << "Error: failed to open super-file which holds the path to the different input files" << endl;
		return ERROR;
	}
	while (getline(inputFilesContainer, line))
	{
		delimIndex = line.find('.');
		if (delimIndex == string::npos) {	//string::npos returns when '.' was not found
			continue;
		}
		if (line.substr(delimIndex + 1).compare(BOARD_EXTENSION) == 0)
		{
			// if we read from the working directory
			if ((hasBoardFile != 1) && (path.empty() == true))
			{
				tmpInputFiles.push_back(line);
				hasBoardFile = 1;
				continue;
			}
			// if we read from a given path
			if ((hasBoardFile != 1) && (path.empty() == false))
			{
				fullPath << line;
				tmpInputFiles.push_back(fullPath.str());
				hasBoardFile = 1;
				fullPath.str("");
				fullPath << path << "\\";
				continue;
			}
			continue;
		}
		if (line.substr(delimIndex + 1).compare(ATTACK_A_EXTENSION) == 0)
		{
			// if we read from the working directory
			if ((hasAAttackFile != 1) && (path.empty() == true))
			{
				tmpInputFiles.push_back(line);
				hasAAttackFile = 1;
				continue;
			}
			// if we read from a given path
			if ((hasAAttackFile != 1) && (path.empty() == false))
			{
				fullPath << line;
				tmpInputFiles.push_back(fullPath.str());
				hasAAttackFile = 1;
				fullPath.str("");
				fullPath << path << "\\";
				continue;
			}
			continue;
		}
		if (line.substr(delimIndex + 1).compare(ATTACK_B_EXTENSION) == 0)
		{
			// if we read from the working directory
			if ((hasBAttackFile != 1) && (path.empty() == true))
			{
				tmpInputFiles.push_back(line);
				hasBAttackFile = 1;
				continue;
			}
			// if we read from a given path
			if ((hasBAttackFile != 1) && (path.empty() == false))
			{
				fullPath << line;
				tmpInputFiles.push_back(fullPath.str());
				hasBAttackFile = 1;
				fullPath.str("");
				fullPath << path << "\\";
			}
		}
		fullPath.str("");
		fullPath << path << "\\";
	}
	/* check flags */
	if (hasBoardFile == 0)
	{
		messages.push_back(missingBoardFile.str());
	}
	if (hasAAttackFile == 0)
	{
		messages.push_back(missingAAtackFile.str());
	}
	if (hasBAttackFile == 0)
	{
		messages.push_back(missingBAttckFile.str());
	}

	//set files in the next order: boardfile in pos[0], a_attck file in pos[1], b_attcak_file in pos[2]
	for (size_t i = 0; i < tmpInputFiles.size(); i++)
	{
		delimIndex = tmpInputFiles[i].find('.');
		fileExtension = tmpInputFiles[i].substr(delimIndex + 1);
		if (fileExtension.compare(BOARD_EXTENSION) == 0)
		{
			inputFiles[0] = tmpInputFiles[i];
			continue;
		}
		if (fileExtension.compare(ATTACK_A_EXTENSION) == 0)
		{
			inputFiles[1] = tmpInputFiles[i];
			continue;
		}
		if (fileExtension.compare(ATTACK_B_EXTENSION) == 0)
		{
			inputFiles[2] = tmpInputFiles[i];
		}
	}

	//clean env
	inputFilesContainer.close();
	return SUCCESS;
}