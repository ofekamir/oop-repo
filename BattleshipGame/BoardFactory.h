#pragma once
#define EMPTY_CELL ' '
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
#include <string>
using namespace std;
class BoardFactory
{
public:
	/* constructor */
	BoardFactory(int rows, int cols, string path);
	//todo: comment
	int getPlayerBoards(char** boardA, char** boardB);
	/* free allocated memory associated with board */
	static void destroyBoard(char** board, int rows, int cols);
private:
	/* destructor. */
	~BoardFactory();
	/* disable copy constructor. */
	BoardFactory(const BoardFactory& that) = delete;
	/* temp board. */
	char **fullBoard = nullptr;
	/* board properties. */
	int boardRows=-1;
	int boardCols=-1;
	/* path to board file. */
	string boardPath;
	/* fill fullBoard with empty cell symbol. */
	void cleanBoard() const;
	/* return 2d char array. return null ptr if error. */
	char** initBoard() const;
	/* fill board with data from board file in path. */
	int fillBoardFromFile() const;
	/* check if board is valid and return 0 if so. return -1 if invalid and
	 * print proper messages. */
	int validateBoard() const;
	/* remove ships with wrong shape or size from board. returns -1 if there are
	 * invalid ships and prints proper messages. */
	bool markInvalidShips(char** boardCpy) const;
	/* Each player board is prepared in advance and hidden from the opponent!
	* fillPlayerBoard gets the full board with both players ships,
	* and fills the given player's board with his ships only. */
	void fillPlayerBoards(char** boardA, char** boardB) const;
};
