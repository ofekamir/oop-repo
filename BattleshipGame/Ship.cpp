#include <locale>
#include "Ship.h"
#include "GameUtils.h"
#include <iostream>

Ship::Ship(char _type) {

	type = _type;
	switch (tolower(_type)) {
	case RUBBER_BOAT:
		life = RUB_LEN;
		sinkPoints = 2;
		break;
	case ROCKET_SHIP:
		life = ROC_LEN;
		sinkPoints = 3;
		break;
	case SUBMARINE:
		life = SUB_LEN;
		sinkPoints = 7;
		break;
	case DESTROYER:
		life = DES_LEN;
		sinkPoints = 8;
	default:
		break;
	}
}

Ship::~Ship() = default;


char Ship::getType() const
{
	return type;
}

int Ship::getSinkPoints() const
{
	return sinkPoints;
}

int Ship::getLife() const
{
	return life;
}

void Ship::hit()
{
	life--;
}