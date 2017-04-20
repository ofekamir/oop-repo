#include "Player.h"
#include <iostream>
#include <string>


// Constructor
Player::Player(int playerNum, string attackFilePath) {
	_playerNum = playerNum;
	_playerScore = 0;
	_finishedAttacks = false;
	_shipsMap.clear();
	_attackFilePath = attackFilePath;
	_attackFileOpened = false;
	_numActiveShips = 5;
}

// Distructor
Player::~Player()
{
	if (_attackFileOpened) 	//Close attack file:
	{
		_attackFile.close();
	}
	//Remove all map elements while freeing the shared_ptr<Ship> ships:
	_shipsMap.clear();
}

int Player::getplayerNum() const
{
	return _playerNum;
}

int Player::getPlayerScore() const
{
	return _playerScore;
}

void Player::incrementPlayerScore(int points)
{
	if (points > 0)
	{
		_playerScore += points;
	}
}

bool Player::finishedAttacks() const
{
	return _finishedAttacks;
}

bool Player::isDefeated() const
{
	return _numActiveShips <= 0;
}

/* Iterate over the board and for each ship, create a ship object pointer
* find add all cells that belong to this ship.
* For each of these cells, add a pair of (cell, ship) to the shipsMap.
* Mark cells as visited */
void Player::setBoard(const char board[FULL_BOARD_LEN][FULL_BOARD_LEN], int numRows, int numCols) {
	int i, j, k, shipLen, direction;
	//shared_ptr<Ship> ship;
	pair<int, int> xy;
	pair<shared_ptr<Ship>, bool> shipAndHit;
	char visited = 'x', cell;
	char boardCpy[FULL_BOARD_LEN][FULL_BOARD_LEN];
	//copy board content (so we can mark visited cells):
	for (i = 0; i < FULL_BOARD_LEN; i++) {
		for (j = 0; j < FULL_BOARD_LEN; j++) {
			boardCpy[i][j] = board[i][j];
		}
	}

	//Iterate over the board and create the shipsMap:
	for (i = 0; i < FULL_BOARD_LEN; i++) {
		for (j = 0; j < FULL_BOARD_LEN; j++) {
			cell = boardCpy[i][j];
			if (cell == EMPTY_CELL || cell == visited) { continue; }
			boardCpy[i][j] = visited;
			//cell is a ship character!
			shared_ptr<Ship> ship(new Ship(cell)); //create the ship object
												   //Add to shipsMap all cells which belong to the current ship:
			shipLen = ship->getLife();
			direction = (boardCpy[i][j + 1] == cell) ? HORIZONTAL : VERTICAL;
			k = 0;
			while (k < shipLen)
			{
				if (direction == HORIZONTAL)
				{
					xy = { i, j + k };
					boardCpy[i][j + k] = visited;
				}
				else if (direction == VERTICAL)
				{
					xy = { i + k, j };
					boardCpy[i + k][j] = visited;

				}
				shipAndHit = { ship , false };
				_shipsMap.insert(std::make_pair(xy, shipAndHit));
				k++;
			}
		}
	}
	//printShipsMap();
}



//Get a legal attack (pair of valid ints) if exists:
/* This method reads the next attack point from the given attack file
* and returns is as a pair <int, int>)
* The functions checks if the current pair is valid and if not,
* according to Guidelines, proceeds to the next line in the file */
int i = 0;
bool debugMode = false;
pair<int, int> Player::attack()
{
	if (debugMode)
	{
		pair<int, int> attacks[9] = { { 1,4 },{ 2,8 },{ 2,9 },{ 4,6 },{ 4,7 },{ 4,8 },{ 9,1 },{ 10,9 },{ 10,10 } };
		return attacks[i++];
	}

	pair<int, int> attack;
	pair<int, int> attackError = { -1, -1 };
	pair<string, string> rowCol;
	string line;
	size_t idx = 0, delimIndex = 0; //holds the position of the next character after the number
	int row, col;

	if (_attackFileOpened == false)
	{
		_attackFile.open(_attackFilePath);
		if (!_attackFile.is_open())
		{
			cout << "Error: failed to open player attack file" << endl;
			return attackError;
		}
	}
	_attackFileOpened = true;

	while (getline(_attackFile, line)) //checked: getline catch \r\n and \n.
	{
		delimIndex = line.find(',');
		if (delimIndex == string::npos) { continue; } //string::npos returns when ',' was not found
		rowCol = std::make_pair(line.substr(0, delimIndex), line.substr(delimIndex + 1));
		//remove ' ' from the end of the key part
		while (!rowCol.first.empty() && rowCol.first.back() == ' ')
		{
			rowCol.first.erase(rowCol.first.length() - 1, 1);
		}
		//remove leading ' ' in the value part
		while (!rowCol.second.empty() && rowCol.second.front() == ' ')
		{
			rowCol.second.erase(0, 1);
		}
		// validate each part has no more than 2 chars
		if (rowCol.first.size() > 2 || rowCol.second.size() > 2) { continue; }

		//check if each part is a number
		try
		{
			row = stoi(rowCol.first, &idx);
			if (rowCol.first.size() == 2 && rowCol.first[idx] != 0)
			{
				continue;
			}
		}
		catch (const std::invalid_argument& ia)
		{
			(void)ia;
			continue;
		}
		try
		{
			col = stoi(rowCol.second, &idx);
			if (rowCol.second.size() == 2 && rowCol.second[idx] != 0)
			{
				continue;
			}
		}
		catch (const std::invalid_argument& ia)
		{
			(void)ia;
			continue;
		}

		// check that each number is in the legal range:
		if (row > 10 || col > 10 || row < 1 || col < 1)
		{
			continue; // according to Guidelines, proceed to next line
		}

		//Reaching here means we got a legal attack pair <int, int> 
		//convert pair of strings to pair of integers
		attack.first = row;
		attack.second = col;
		return attack;
	}

	//Reaching here means no valid attack could be found:
	_attackFile.close();
	_finishedAttacks = true;
	return attackError;
}



void Player::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	//Do nothing for now...	
}


/* Search for the attack point in shipsMap:
* If attack point is not in map --> it's a Miss!
* Else, hit the ship in map by taking one off the ship life counter
* If ship life is 0 (already sank) consider a Miss.
* If ship life has now become 0 --> it's a Sink!
* Else, return Hit. */
pair <AttackResult, int> Player::execute(pair<int, int> attack)
{
	auto found = _shipsMap.find(attack);
	if (found == _shipsMap.end()) //attack pint not in map --> Miss
	{
		//cout << "Miss" << endl;
		return { AttackResult::Miss, 0 };
	}
	auto ship = found->second.first; //attack point is in map --> get the ship
	auto shipWasHit = found->second.second;

	if (shipWasHit == true) //Not the first hit on this specific cell (i,j)
	{
		//		//As mentioned on the forum, we can choose to return 'Miss' in this case. 
		//		//Can easily change this behaviour to 'Hit' in next exercise..
		if (ship->getLife() == 0) //ship already sank.. Miss
		{
			//cout << "Miss (hit a sunken ship)" << endl;
			return { AttackResult::Miss, 0 };
		}
		//		cout << "Hit (ship was already hit befor but still has'nt sunk..)" << endl;
		return { AttackResult::Hit, -1 }; //-1 indicates it's not the first hit
	}

	ship->hit(); //ship--; //Hit the ship (Take one off the ship life)
	found->second.second = true; //Mark cell as a 'Hit'
								 //cout << "Hit ship " << ship->getType() << endl;

	if (ship->getLife() == 0) //It's a Sink
	{
		_numActiveShips--;
		//cout << "Sink! Score: " << ship->getSinkPoints() << endl;
		return { AttackResult::Sink, ship->getSinkPoints() };
	}

	return { AttackResult::Hit, 0 }; //Hit
}


void Player::printShipsMap()
{
	pair<int, int> cell;
	int count = 1;
	for (auto iter = _shipsMap.begin(); iter != _shipsMap.end(); ++iter)
	{
		cell = iter->first;
		auto ship = iter->second.first;
		std::cout << "Map entry " << count << " is:";
		std::cout << "(" << cell.first << "," << cell.second << ")";
		std::cout << " shipType: " << ship->getType() << endl;
		count++;
	}
}