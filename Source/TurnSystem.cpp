#include "TurnSystem.h"
#include <EnemyManager.h>

void TurnSystem::ChangeTurn()
{
	switch (subject)
	{
	case TURN_ENUM::ENEMY_TURN:
		ChangeToPlayerTurn();
		break;
	case TURN_ENUM::PLAYER_TURN:
		ChangeToEnemyTurn();
		break;
	}
	++turnNum;
}

void TurnSystem::ChangeToPlayerTurn()
{
	subject = TURN_ENUM::PLAYER_TURN;
}

void TurnSystem::ChangeToEnemyTurn()
{
	subject = TURN_ENUM::ENEMY_TURN;
}
