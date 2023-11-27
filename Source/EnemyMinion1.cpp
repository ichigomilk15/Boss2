#include "EnemyMinion1.h"
#include "Input\Input.h"
#include "Stage.h"
#include "NormalAttack.h"
#include "AttackManager.h"

EnemyMinion1::EnemyMinion1(Character* p) :
	Enemy(p)
{
	//model = std::make_unique<Model>("Data/Model/Jammo/Jammo.mdl");
	model = std::make_unique<Model>("Data/Model/Enemy/Normal1/NormalEnemy1.mdl");

	//スケールの調整
	scale.x = scale.y = scale.z = 0.15f;

	height = 1.0f;
	enemyType = ENEMY_TYPE::BOSS1;
	actMax = 3;
	actNo = 0;
	state = State::Act_Init;
	moveMax = 2;
	attackPower = 7;
	attackAdjacentRange = 2;
	isActEnd = false;
	SetDirection(CommonClass::DirectionFace::BackRight);
}

void EnemyMinion1::UpdateState(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	switch (state)
	{
	case State::Idle_Init:
		this->model->PlayAnimation(Animation::Idle, true);
		state = State::Idle;
		[[fallthrough]];
	case State::Idle:

		break;

	case State::Act_Init:
		this->model->PlayAnimation(Animation::Idle, true);
		actTimer = 0.5f;
		state = State::Act;
		Stage::Instance()->ResetAllSquare();
		[[fallthrough]];
	case State::Act:
		actTimer -= elapsedTime;
		if (actTimer < 0.0f)
		{
			if (actNo < actMax)
				SetState(ChooseAct(elapsedTime));
			else
				SetState(State::Act_Finish_Init);

			++actNo;
			break;
		}
		break;

	case State::Move_Init:
		actTimer = 0.5f;
		this->targetMovePos = { -1, -1 };
		moveRange = moveMax;
		state = State::Move;
		[[fallthrough]];
	case State::Move:
		actTimer -= elapsedTime;
		if (actTimer < 0.0f)
		{
			UpdateMove(elapsedTime);
		}

		if (IsMoving())
		{
			SetState(State::Moving_Init);
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
			SetState(State::Act_Init);
			break;
		}
		break;

	case State::Attack_Init:
		this->model->PlayAnimation(Animation::Run, false);
		actTimer = 1.0f;
		state = State::Attack;
		[[fallthrough]];
	case State::Attack:
		actTimer -= elapsedTime;
		if (actTimer < 0.0f || !model->IsPlayAnimation())
		{
			if (attack && !attack->GetIsDestroy())
			{
				SetState(State::Attacking_Init);
				break;
			}
			else
			{
				InitializeAttack(elapsedTime);
			}
		}
		break;

	case State::Attacking_Init:
		this->model->PlayAnimation(Animation::Attack, false, 0.0f);
		state = State::Attacking;
		[[fallthrough]];
	case State::Attacking:
		if (!attack || (attack && attack->GetIsDestroy()) || !model->IsPlayAnimation())
		{
			attack = nullptr;
			SetState(State::Act_Init);
		}
		break;

	case State::Act_Finish_Init:
		actTimer = 1.0f;
		isActEnd = true;
		state = State::Act_Finish;
		[[fallthrough]];
	case State::Act_Finish:
		actTimer -= elapsedTime;
		if (actTimer < 0.0f)
		{
			SetState(State::Idle_Init);
			break;
		}
		break;

	default:
		break;
	}
}

State EnemyMinion1::ChooseAct(float elapsedTime)
{
	const DirectX::XMINT2 playerPos = player->GetPosition();
	if (Stage::Instance()->IsAdjacent({ position.x, position.y }, playerPos))
	{
		InitializeAttack(elapsedTime);
		return State::Attack_Init;
	}

	//攻撃範囲にプレイヤーを検索
	auto attackSquares = Stage::Instance()->GetSquaresEdgeAdjacent(position.x, position.y, attackAdjacentRange);
	for (auto& sq : attackSquares)
	{
		const auto sqPos = sq->GetPos();
		if (sqPos.x == playerPos.x && sqPos.y == playerPos.y)
		{
			InitializeAttack(elapsedTime);
			return State::Attack_Init;
			break;
		}
	}

	return State::Move_Init;
}

void EnemyMinion1::InitializeAttack(float elapsedTime)
{
	if (attack && !attack->GetIsDestroy()) return;
	std::vector<Square*> attackSq;
	DirectX::XMINT2 dirPos = { player->GetPosition().x - position.x, player->GetPosition().y - position.y };

	if (dirPos.x > 0)
		SetDirection(CommonClass::DirectionFace::Right);
	else if(dirPos.x < 0)
		SetDirection(CommonClass::DirectionFace::Left);
	else if (dirPos.y > 0)
		SetDirection(CommonClass::DirectionFace::Back);
	else if (dirPos.y < 0)
		SetDirection(CommonClass::DirectionFace::Front);

	attackSq = Stage::Instance()->GetSquaresByDirection(this->position.x, this->position.y, attackAdjacentRange, this->GetDirection());
	std::vector<DirectX::XMINT2> posVec;
	for (auto& sq : attackSq)
	{
		posVec.emplace_back(sq->GetPos());
	}
	attack = new NormalAttack(this, 1, posVec);
	AttackManager::Instance().Register(attack);

	for (auto& square : attackSq)
	{
		square->SetType(Square::Type::AttackArea);
	}
}

bool EnemyMinion1::UpdateMove(float elapsedTime)
{
	int difX = player->GetPosition().x - position.x;
	int difY = player->GetPosition().y - position.y;

	if (difX == 0 && difY == 0) return false;
	if (Stage::Instance()->IsAdjacent({ position.x, position.y }, player->GetPosition())) return false;

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
