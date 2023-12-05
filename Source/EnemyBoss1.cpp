#include "EnemyBoss1.h"
#include "Input\Input.h"
#include "Stage.h"
#include "AttackManager.h"
#include "NormalAttack.h"
#include "BumpAttack.h"
#include "JumpAttack.h"
#include "CameraController.h"

EnemyBoss1::EnemyBoss1(Character* p) :
	Enemy(p)
{
	//model = std::make_unique<Model>("Data/Model/Jammo/Jammo.mdl");
	model = std::make_unique<Model>("./Data/Model/Enemy/Boss/Boss.mdl");
	icon = std::make_unique<Sprite>("./Data/Sprite/icon_boss.png");

	//スケールの調整
	scale.x = scale.y = scale.z = 0.18f;

	height = 1.0f;
	enemyType = ENEMY_TYPE::BOSS1;
	actMax = 1;
	actNo = 0;
	state = State::Act_Init;
	moveMax = 2;
	bumpAttackDetail.attackPow = 7;
	isActEnd = false;
	hpBarUseScale = 1.0f;
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

	case State::AttackCharge_Init:
		this->model->PlayAnimation(ANIMATION_BOSS::BossDrop, true);
		Stage::Instance()->ResetAllSquare();
		--attackChargeTurn;
		state = State::AttackCharge;
		actTimer = 0.5f;
		if (attackChargeTurn >= 0)
			isActEnd = true;
		[[fallthrough]];
	case State::AttackCharge:
		--actTimer;
		if (attackChargeTurn < 0 && actTimer <= 0.0f)
		{
			if (dynamic_cast<BumpAttack*>(attack))
				SetState(State::Attacking_Init);
			else if (dynamic_cast<JumpAttack*>(attack))
				SetState(State::AttackingJump_Fly_Init);

			++actNo;
			break;
		}
		break;

	case State::Attack_Init:
		actTimer = 0.5f;
		state = State::Attack;
		[[fallthrough]];
	case State::Attack:
		actTimer -= elapsedTime;
		if (actTimer < 0.0f || !model->IsPlayAnimation())
		{
			if (attack && !attack->GetIsDestroy())
			{
				if (dynamic_cast<BumpAttack*> (attack) || dynamic_cast<JumpAttack*>(attack))
				{
					SetState(State::AttackCharge_Init);
					attackChargeTurn = 1;
				}
				else
				{
					SetState(State::Attacking_Init);
				}
				break;
			}
			else
			{
				InitializeAttack(elapsedTime);
			}
		}
		break;

	case State::Attacking_Init:
		this->model->PlayAnimation(ANIMATION_BOSS::BossSpin, false, 0.2f);
		targetMovePos = bumpAttackDetail.targetChargingMovePos;
		bumpAttackDetail.targetChargingMovePos = { -1, -1 };
		actTimer = (bumpAttackDetail.stunTurn > 0) ? 0.0f : 1.0f;
		AttackManager::Instance().Register(attack);
		state = State::Attacking;
		[[fallthrough]];
	case State::Attacking:
		actTimer -= elapsedTime;
		if (!IsMoving() && actTimer <= 0.0f)
		{
			SetState(AfterBumpAttack());
			break;
		}
		break;

	case State::AttackingJump_Fly_Init:
		this->model->PlayAnimation(ANIMATION_BOSS::BossJump, false, 0.2f);
		targetMovePos = jumpAttackDetail.targetJumpMovePos;
		jumpAttackDetail.targetJumpMovePos = { -1, -1 };
		actTimer = 1.0f;
		AddImpulse({ 0.0f, 0.8f, 0.0f });
		state = State::AttackingJump_Fly;
		[[fallthrough]];
	case State::AttackingJump_Fly:
		actTimer -= elapsedTime;
		if (!IsMoving())
		{
			if (velocity.y <= 0.000001f)
			{
				SetState(State::AttackingJump_Stump_Init);
				break;
			}
			else
			{
				velocity.y = 0.0f;
			}
		}
		break;

	case State::AttackingJump_Stump_Init:
		AddImpulse({ 0.0f, -1.0f, 0.0f });
		state = State::AttackingJump_Stump;
		[[fallthrough]];
	case State::AttackingJump_Stump:
		actTimer -= elapsedTime;
		if (isGround)
		{
			Stage::Instance()->ResetAllSquare();
			AttackManager::Instance().Register(std::move(attack));
			attack = nullptr;
			CameraController::Instance().ShakeCamera(0.75f, 7);
			Stage::Instance()->ResetAllSquareDrawType();
			SetState(State::Act_Init);
			break;
		}
		break;

	case State::Stunned_Init:
		this->model->PlayAnimation(ANIMATION_BOSS::BossDamage, true);
		actTimer = 1.0f;
		--bumpAttackDetail.stunTurn;
		if (bumpAttackDetail.stunTurn >= 0)
			isActEnd = true;
		state = State::Stunned;
		[[fallthrough]];
	case State::Stunned:
		if (bumpAttackDetail.stunTurn < 0)
		{
			if (actTimer -= elapsedTime < 0.0f)
			{
				++actNo;
				state = State::Act_Init;
				break;
			}
		}
		break;

	case State::KnockedBack_Init:
		this->SetDirection({ position.x - player->GetPosition().x, position.y - player->GetPosition().y });
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
		else
		{

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

void EnemyBoss1::InitializeAttack(float elapsedTime) //バンプ攻撃
{
	if (attack && !attack->GetIsDestroy()) return;
	//ぶつける攻撃
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

	//バンプ攻撃
	if (isBumpAttack)
	{
		bumpAttackDetail.attackPow = 20;
		std::vector<DirectX::XMINT2> posVec;
		for (auto& sq : attackSq)
		{
			posVec.emplace_back(sq->GetPos());
		}
		attack = new BumpAttack(this, bumpAttackDetail.attackPow, TargetAttackEnum::Target_Player, GetDirection(), posVec, 0.5f);
		//AttackManager::Instance().Register(attack);

		for (auto& square : attackSq)
		{
			square->SetType(Square::Type::AttackArea);
			square->SetDrawType(Square::DrawType::ChargeAttack);
		}


		if (!this->IsTargetMoveAttackPosValid(targetPos))
		{
			//todo: stun処理ここで作成
			targetPos.x = std::clamp(targetPos.x, 0, abs((int)Common::SQUARE_NUM_X - size.x));
			targetPos.y = std::clamp(targetPos.y, 0, abs((int)Common::SQUARE_NUM_Y - size.y));
			bumpAttackDetail.stunTurn = 1;
		}
		//targetMovePos = targetPos;
		bumpAttackDetail.targetChargingMovePos = targetPos;
		return;
	}
	else //ジャンプ攻撃
	{
		jumpAttackDetail.attackPow = 30;
		std::vector<DirectX::XMINT2> posVec;
		std::vector<Square*> atkSquare = Stage::Instance()->GetSquaresBoxRange(player->GetPosition().x, player->GetPosition().y, 2);
		atkSquare.emplace_back(Stage::Instance()->GetSquare(player->GetPosition().x, player->GetPosition().y).get());
		for (auto& sq : atkSquare)
		{
			posVec.emplace_back(sq->GetPos());
			sq->SetType(Square::Type::AttackArea);
			sq->SetDrawType(Square::DrawType::ChargeAttack);
		}
		attack = new JumpAttack(this, jumpAttackDetail.attackPow, TargetAttackEnum::Target_Player, posVec);
		jumpAttackDetail.targetJumpMovePos = player->GetPosition();
		return;
	}
}

State EnemyBoss1::AfterBumpAttack()
{
	targetMovePos = { -1, -1 };
	attack = nullptr;
	Stage::Instance()->ResetAllSquareDrawType();
	if (bumpAttackDetail.stunTurn > 0)
	{
		CameraController::Instance().ShakeCamera(0.75f, 5);
		return State::Stunned_Init;
	}
	else
	{
		return State::Act_Init;
	}
}
