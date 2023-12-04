#include "EnemyBoss1.h"
#include "Input\Input.h"
#include "Stage.h"
#include "AttackManager.h"
#include "NormalAttack.h"
#include "BumpAttack.h"

EnemyBoss1::EnemyBoss1(Character* p) :
	Enemy(p)
{
	//model = std::make_unique<Model>("Data/Model/Jammo/Jammo.mdl");
	model = std::make_unique<Model>("Data/Model/Enemy/Boss/Boss.mdl");

	//ÉXÉPÅ[ÉãÇÃí≤êÆ
	scale.x = scale.y = scale.z = 0.18f;

	height = 1.0f;
	enemyType = ENEMY_TYPE::BOSS1;
	actMax = 1;
	actNo = 0;
	state = State::Act_Init;
	moveMax = 2;
	attackPower = 7;
	isActEnd = false;
	SetDirection(CommonClass::DirectionFace::BackRight);
}

void EnemyBoss1::UpdateState(float elapsedTime)
{
	switch (state)
	{
	case State::Idle_Init:
		if (!this->model->IsPlayAnimation(ANIMATION_BOSS::BossIdle))
			this->model->PlayAnimation(ANIMATION_BOSS::BossIdle, true);
		state = State::Idle;
		[[fallthrough]];
	case State::Idle:

		break;

	case State::Act_Init:
		this->model->PlayAnimation(ANIMATION_BOSS::BossIdle, true);
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
		this->model->PlayAnimation(ANIMATION_BOSS::BossSpin, false);
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
		this->model->PlayAnimation(Animation::Attack, true, 0.2f);
		state = State::Attacking;
		[[fallthrough]];
	case State::Attacking:
		if ((!attack || (attack && attack->GetIsDestroy())) && !IsMoving())
		{
			targetMovePos = { -1, -1 };
			attack = nullptr;
			SetState(State::Act_Init);
		}
		break;

	case State::KnockedBack_Init:
		this->SetDirection({position.x - player->GetPosition().x, position.y - player->GetPosition().y});
		actTimer = 0.5f;
		state = State::KnockedBack;
		[[fallthrough]];
	case State::KnockedBack:
		actTimer -= elapsedTime;
		if (!IsMoving() && actTimer < 0.0f)
		{
			SetState(State::Idle_Init);
			break;
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

void EnemyBoss1::InitializeAttack(float elapsedTime)
{
	if (attack && !attack->GetIsDestroy()) return;
	//Ç‘Ç¬ÇØÇÈçUåÇ
	std::vector<Square*> attackSq;
	bool isBumpAttack = true;
	SetDirection(player->GetPosition());
	DirectX::XMINT2 targetPos = position;

	switch (GetDirection())
	{
	case CommonClass::DirectionFace::Back:
		for (auto& pos : GetSquaresPositionX(position.y + size.y - 1))
		{
			for (auto& sq : Stage::Instance()->GetSquaresByDirection(pos.x, pos.y, attackRange, this->GetDirection()))
			{
				attackSq.emplace_back(sq);
			}
		}
		targetPos.y += attackRange;
		break;
	case CommonClass::DirectionFace::Front:
		for (auto& pos : GetSquaresPositionX(position.y))
		{
			for (auto& sq : Stage::Instance()->GetSquaresByDirection(pos.x, pos.y, attackRange, this->GetDirection()))
			{
				attackSq.emplace_back(sq);
			}
		}
		targetPos.y -= attackRange;
		break;
	case CommonClass::DirectionFace::Left:
		for (auto& pos : GetSquaresPositionY(position.x))
		{
			for (auto& sq : Stage::Instance()->GetSquaresByDirection(pos.x, pos.y, attackRange, this->GetDirection()))
			{
				attackSq.emplace_back(sq);
			}
		}
		targetPos.x -= attackRange;
		break;
	case CommonClass::DirectionFace::Right:
		for (auto& pos : GetSquaresPositionY(position.x + size.x - 1))
		{
			for (auto& sq : Stage::Instance()->GetSquaresByDirection(pos.x, pos.y, attackRange, this->GetDirection()))
			{
				attackSq.emplace_back(sq);
			}
		}
		targetPos.x += attackRange;
		break;
	default:
		isBumpAttack = false;
		break;
	}

	if (isBumpAttack)
	{
		std::vector<DirectX::XMINT2> posVec;
		for (auto& sq : attackSq)
		{
			posVec.emplace_back(sq->GetPos());
		}
		attack = new BumpAttack(this, attackPower, TargetAttackEnum::Target_Player, GetDirection(), posVec, 0.5f);
		AttackManager::Instance().Register(attack);

		for (auto& square : attackSq)
		{
			square->SetType(Square::Type::AttackArea);
		}
		

		if (!this->IsTargetMoveAttackPosValid(targetPos))
		{
			//todo: stunèàóùÇ±Ç±Ç≈çÏê¨
			targetPos.x = std::clamp(targetPos.x, 0, abs((int)Common::SQUARE_NUM_X - size.x));
			targetPos.y = std::clamp(targetPos.y, 0, abs((int)Common::SQUARE_NUM_Y - size.y));
		}
		targetMovePos = targetPos;
		return;
	}
	else //Jump Attack
	{

	}
}
