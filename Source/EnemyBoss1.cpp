#include "EnemyBoss1.h"
#include "Input\Input.h"
#include "Stage.h"

EnemyBoss1::EnemyBoss1(Character* p) :
	Enemy(p)
{
	//model = std::make_unique<Model>("Data/Model/Jammo/Jammo.mdl");
	model = std::make_unique<Model>("Data/Model/Enemy/Normal1/NormalEnemy1.mdl");

	//ƒXƒP[ƒ‹‚Ì’²®
	scale.x = scale.y = scale.z = 0.6f;

	height = 1.0f;
	enemyType = ENEMY_TYPE::BOSS1;
	actMax = 0;
	actNo = 0;
	state = State::Act_Init;
	moveMax = 2;
	attackPower = 7;
	//attackAdjacentRange = 2;
	isActEnd = false;
	health = 100;
	SetDirection(CommonClass::DirectionFace::BackRight);
}

void EnemyBoss1::UpdateState(float elapsedTime)
{
	switch (state)
	{
	case State::Idle_Init:
		if (!this->model->IsPlayAnimation(Animation::Idle))
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
			if (!ChooseTargetMove(elapsedTime))
			{
				SetState(State::Act_Init);
				break;
			}
			else
			{
				SetState(State::Moving_Init);
				break;
			}
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
