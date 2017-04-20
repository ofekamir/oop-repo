#pragma once

#include <utility> // For std::pair

enum class AttackResult {
	Miss, Hit, Sink
};

class IBattleshipGameAlgo {
public:

	// Class distructor
	virtual ~IBattleshipGameAlgo() = default;

	// SetBoard is called once to notify player on his board
	virtual void setBoard(const char board[12][12], int numRows, int numCols) = 0;

	// Ask a player for his next move
	virtual std::pair<int, int> attack() = 0;

	// Notify a player on the last move result
	virtual void notifyOnAttackResult(int player, int row, int col, AttackResult result) = 0; // notify on last move result
};