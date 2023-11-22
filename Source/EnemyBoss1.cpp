#include "EnemyBoss1.h"
#include "Input\Input.h"
#include "Stage.h"

EnemyBoss1::EnemyBoss1(Character* p) :
    Enemy(p)
{
    this->model = std::make_unique<Model>("Data/Model/Mr.Incredible/Mr.Incredible.mdl");

    //ƒXƒP[ƒ‹‚Ì’²®
    scale.x = scale.y = scale.z = 0.03f;

    height = 1.0f;
    enemyType = ENEMY_TYPE::BOSS1;
	state = State::Act_Init;
}

void EnemyBoss1::UpdateState(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	switch (state)
	{
	case State::Idle_Init:

		state = State::Idle;
		[[fallthrough]];
	case State::Idle:

		break;

	case State::Act_Init:
		state = State::Act;
		[[fallthrough]];
		break;
	case State::Act:
		if (gamePad.GetButtonDown() & GamePad::BTN_Y)
		{
			state = State::Move_Init;
			break;
		}
		break;

	case State::Move_Init:
		this->targetMovePos = { -1, -1 };
		moveRange = moveMax;
		state = State::Move;
		[[fallthrough]];
	case State::Move:
		UpdateMove(elapsedTime);

		if (IsMoving())
		{
			this->state = State::Moving_Init;
			break;
		}
		break;

	case State::Moving_Init:
		this->SetDirection(this->targetMovePos);
		state = State::Moving;
		[[fallthrough]];
	case State::Moving:
		if (!IsMoving())
		{
			this->state = State::Act_Init;
			break;
		}
		break;

	default:
		break;
	}
}

bool EnemyBoss1::UpdateMove(float elapsedTime)
{
	int difX = player->GetPosition().x - position.x;
	int difY = player->GetPosition().y - position.y;

	if (difX == 0 && difY == 0) return false;
	if (Stage::Instance()->IsAdjacent({position.x, position.y}, player->GetPosition())) return false;

	if (abs(difX) > abs(difY))
	{
		targetMovePos.x = position.x + (difX / abs(difX));
		targetMovePos.y = position.y;
	}
	else if (abs(difY) > abs(difX))
	{
		targetMovePos.x = position.x;
		targetMovePos.y = position.y + (difY / abs(difY));
	}
	else
	{
		targetMovePos.x = position.x + (difX / abs(difX));
		targetMovePos.y = position.y;
	}
	return true;
}
