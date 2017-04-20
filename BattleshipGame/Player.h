#pragma once

#include "IBattleshipGameAlgo.h"
#include <map>
#include "Ship.h"
#include "GameUtils.h"
#include <fstream>
#include <memory>

using namespace std;

class Player : public IBattleshipGameAlgo {
public:

	// Class constructor
	explicit Player(int playerNum, string attackFilePath);

	// Class distructor
	~Player();

	//void setBoard(const char **board, int numRows, int numCols) override;
	void Player::setBoard(const char board[FULL_BOARD_LEN][FULL_BOARD_LEN], int numRows, int numCols) override;

	//Declare on the next requested attack move
	pair<int, int> attack() override;

	//Get the other player's request for attack move, and execute it on board
	//return the AttackResult (Miss / Hit / Sink) and the score.
	pair <AttackResult, int> execute(pair<int, int> attack);

	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;

	int getplayerNum() const;

	int getPlayerScore() const;

	void incrementPlayerScore(int points);

	//Set to true when a player finishes all his attacks in file
	bool finishedAttacks() const;

	//Set to true by player if enemy destroyed all his ships
	bool isDefeated() const;


private:
	int _playerNum;
	int _playerScore;
	ifstream _attackFile; //The input file with all attacks to be executed
	string _attackFilePath;
	bool _attackFileOpened;
	bool _finishedAttacks;
	int _numActiveShips; //number of active ships
	map<pair<int, int>, pair<shared_ptr<Ship>, bool> > _shipsMap;
	/* The players board is implemented using a private map.
	* Each map entry holds a board coordinate (like a matrix cell) as key,
	* and a ship object as value. Since each ship can take a few cells,
	* different keys may hold the same ship object. */

	//Prints the items in shipsMap as (<int,int>, <ship char>)
	void printShipsMap();
};