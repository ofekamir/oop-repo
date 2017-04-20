#pragma once
#include <vector>

using namespace std;

#define BOARD_LEN 10
#define EMPTY_CELL ' '
#define SUCCESS 0
#define ERROR -1
#define FULL_BOARD_LEN 12
#define NUM_SHIP_TYPES 4
#define SHIPS_FOR_PLAYER 5
#define RUBBER_BOAT 'b'
#define ROCKET_SHIP 'p'
#define SUBMARINE 'm'
#define DESTROYER 'd'
#define HORIZONTAL 1
#define VERTICAL 0
#define RUB_LEN 1; //number of blocks for each ship
#define ROC_LEN 2;
#define SUB_LEN 3;
#define DES_LEN 4;
class GameUtils {
public:

	/* init board with EMPTY-CELL symbol */
	static void cleanBoard(char board[FULL_BOARD_LEN][FULL_BOARD_LEN]);

	/* fill board with data from board file in path */
	static int fillBoardFromFile(string path, char board[FULL_BOARD_LEN][FULL_BOARD_LEN]);

	/* Each player board is prepared in advance and hidden from the opponent!
	* fillPlayerBoard gets the full board with both players ships,
	* and fills the given player's board with his ships only. */
	static void fillPlayerBoard(int player, char playerBoard[FULL_BOARD_LEN][FULL_BOARD_LEN], char fullBoard[FULL_BOARD_LEN][FULL_BOARD_LEN]);

	/* print board for debug */
	static void printBoard(char board[FULL_BOARD_LEN][FULL_BOARD_LEN], bool fullPrint);

	static void showMessages(vector<string> messages);

	/* getInputFiles function gets the cmd args and a vector of input files with
	* size 3 initialized with empty string in each object and returns success
	* iff it succeeded to fill the given inputFiles vector with the 3 reguired files paths
	* and the returned mesaages vector is with size of 0 (no failures).
	* The inputFiles vector is filled in the following structure:
	* pos0 = battle board
	* pos1 = a attack board
	* pos2 = b attack board	*/
	static int getInputFiles(vector<string> & inputFiles, vector<string> & messages, int argc, char *argv[]);

private:
	GameUtils() = delete; //disallow creating instance of class

	~GameUtils() = delete; //disallow destructor

						   /* check if board is valid and return 0 if so. return -1 if invalid and
						   * print proper messages */
	static int validateBoard(char board[FULL_BOARD_LEN][FULL_BOARD_LEN]);

	/* remove ships with wrong shape or size from board. returns -1 if there are invalid ships and prints proper
	* messages */
	static bool GameUtils::markInvalidShips(char boardCpy[FULL_BOARD_LEN][FULL_BOARD_LEN],
		char board[FULL_BOARD_LEN][FULL_BOARD_LEN]);

	static bool isShip(char i);

	static int fetchInputFiles(vector<string> & inputFiles, vector<string> & messages, const string path);

};