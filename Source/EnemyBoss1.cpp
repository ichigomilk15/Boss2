#include "EnemyBoss1.h"
#include "Input\Input.h"
#include "Stage.h"
#include "AttackManager.h"
#include "NormalAttack.h"
#include "BumpAttack.h"
#include "JumpAttack.h"
#include "CameraController.h"
#include "PhaseManager.h"
#include "Audio\AudioLoader.h"
#include "EffectManager.h"

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

	InitializeAudio();
	boss1Ses.startLineSe.get()->Play(false);

	effects.dizzy = std::make_unique<Effect>("./Data/Effect/dizzy.efk");
	effects.shockWave = std::make_unique<Effect>("./Data/Effect/shockwave.efk");
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
		{
			isAttackCharging = true;
			if (PhaseManager::Instance().GetFhase() >= PhaseManager::Phase::Phase_Enemy_Init)
				isActEnd = true;
		}
		[[fallthrough]];
	case State::AttackCharge:
		--actTimer;
		if (attackChargeTurn < 0 && actTimer <= 0.0f)
		{
			if (dynamic_cast<BumpAttack*>(attack))
				SetState(State::Attacking_Init);
			else if (dynamic_cast<JumpAttack*>(attack))
				SetState(State::AttackingJump_Fly_Init);
			isAttackCharging = false;
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
		if ((actTimer < 0.0f || !model->IsPlayAnimation()) && !player->IsMoving())
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
		boss1Ses.attackLineSe.get()->Play(false);
		boss1Ses.bumpAtkSe.get()->Play(false);
		attack->ActivateAttack();
		attack = nullptr;
		//AttackManager::Instance().Register(std::move(attack));
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
		boss1Ses.attackLineSe.get()->Play(false);
		state = State::AttackingJump_Stump;
		[[fallthrough]];
	case State::AttackingJump_Stump:
		actTimer -= elapsedTime;
		if (isGround)
		{
			boss1Ses.jumpAtkSe.get()->Play(false);
			Stage::Instance()->ResetAllSquare();
			attack->ActivateAttack();
			attack = nullptr;
			//AttackManager::Instance().Register(std::move(attack));
			CameraController::Instance().ShakeCamera(1.05f, 6);
			Stage::Instance()->ResetAllSquareDrawType();
			SetState(State::Attack_Init);
			effects.shockWave->Play(positionWorld, 2.0f);
			break;
		}
		break;

	case State::Stunned_Init:
		this->model->PlayAnimation(ANIMATION_BOSS::BossDamage, true);
		actTimer = 1.0f;
		--bumpAttackDetail.stunTurn;
		boss1Ses.panicSe.get()->Stop();
		boss1Ses.panicSe.get()->Play(true);
		if (bumpAttackDetail.stunTurn >= 0)
		{
			isActEnd = true;
			EffectManager::Instance().GetEffekseerManager()->SendTrigger(effects.dizzy->GetHandle(), 0);
		}
		state = State::Stunned;
		[[fallthrough]];
	case State::Stunned:
		if (bumpAttackDetail.stunTurn < 0)
		{
			if (actTimer -= elapsedTime < 0.0f)
			{
				++actNo;
				boss1Ses.panicSe.get()->Stop();
				effects.dizzy->Stop(effects.dizzy->GetHandle());
				boss1Ses.angrySe.get()->Play(false);
				state = State::Attack_Init;
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
			if (isAttackCharging)
			{
				CancelChargeAttack();
				//SetState(State::Attack_Init);
				SetState(State::Idle_Init);
			}
			else if (bumpAttackDetail.stunTurn >= 0)
			{
				SetState(State::Stunned);
			}
			else
			{
				SetState(State::Idle_Init);
			}
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
		bumpAttackDetail.stunTurn = 0;
		bumpAttackDetail.attackPow = 20;
		std::vector<DirectX::XMINT2> posVec;
		for (auto& sq : attackSq)
		{
			posVec.emplace_back(sq->GetPos());
		}
		attack = new BumpAttack(this, bumpAttackDetail.attackPow, TargetAttackEnum::Target_Player, GetDirection(), posVec, 0.5f, false);
		AttackManager::Instance().Register(attack);

		for (auto& square : attackSq)
		{
			square->SetType(Square::Type::AttackArea);
			square->InputDrawType(Square::DrawType::ChargeAttack);
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
		jumpAttackDetail.attackPowCenter = 25;
		jumpAttackDetail.attackPowEdge = 15;
		std::vector<DirectX::XMINT2> posVec;
		targetPos = player->GetPosition();
		if (!this->IsTargetMoveAttackPosValid(targetPos))
		{
			//todo: stun処理ここで作成
			targetPos.x = std::clamp(targetPos.x, 0, abs((int)Common::SQUARE_NUM_X - size.x));
			targetPos.y = std::clamp(targetPos.y, 0, abs((int)Common::SQUARE_NUM_Y - size.y));
		}

		std::vector<Square*> atkSquare = Stage::Instance()->GetSquaresBoxRange(targetPos.x, targetPos.y, 2);
		atkSquare.emplace_back(Stage::Instance()->GetSquare(targetPos.x, targetPos.y).get());
		for (auto& sq : atkSquare)
		{
			posVec.emplace_back(sq->GetPos());
			sq->SetType(Square::Type::AttackArea);
			sq->InputDrawType(Square::DrawType::ChargeAttack);
		}
		//attack = new JumpAttack(this, jumpAttackDetail.attackPow, TargetAttackEnum::Target_Player, posVec);
		attack = new JumpAttack(this, jumpAttackDetail.attackPowCenter, jumpAttackDetail.attackPowEdge, TargetAttackEnum::Target_Player, player->GetPosition(), posVec, 0.0f, false);
		AttackManager::Instance().Register(attack);
		jumpAttackDetail.targetJumpMovePos = targetPos;
		return;
	}
}

State EnemyBoss1::ChooseAct(float elapsedTime)
{
	return State::Attack_Init;
}

State EnemyBoss1::AfterBumpAttack()
{
	targetMovePos = { -1, -1 };
	attack = nullptr;
	Stage::Instance()->ResetAllSquare();
	Stage::Instance()->ResetAllSquareDrawType();
	if (bumpAttackDetail.stunTurn > 0)
	{
		CameraController::Instance().ShakeCamera(1.25f, 8);
		boss1Ses.wallHitSe.get()->Play(false);
		--bumpAttackDetail.stunDefence;
		if (bumpAttackDetail.stunDefence > 0)
		{
			return State::Attack_Init;
		}
		else
		{
			InitStunDefence();
			auto nodetransform = model->FindNode("J_head_end")->worldTransform;
			DirectX::XMFLOAT3 pos = { nodetransform._41,nodetransform._42,nodetransform._43 };
			effects.dizzy->Play(pos, 0.8f);
			return State::Stunned_Init;
		}
	}
	else
	{
		return State::Attack_Init;
	}
}

void EnemyBoss1::InitStunDefence()
{
	float perc = (float)health / maxHealth * 100.0f;
	if (perc >= 80.0f)
	{
		bumpAttackDetail.stunDefence = 1;
	}
	else if (perc >= 40.0f)
	{
		bumpAttackDetail.stunDefence = 2;
	}
	else
	{
		bumpAttackDetail.stunDefence = 3;
	}
}

void EnemyBoss1::OnDamaged()
{
	boss1Ses.damageSe.get()->Play(false);
	Enemy::OnDamaged();
}

void EnemyBoss1::OnDead()
{
	boss1Ses.deathSe.get()->Play(false);
	Enemy::OnDead();
}

void EnemyBoss1::InitializeAudio()
{
	AudioLoader::Load(AUDIO::SE_BOSS1_ATTACKLINE, boss1Ses.attackLineSe);
	AudioLoader::Load(AUDIO::SE_BOSS1_BUMPATTACK, boss1Ses.bumpAtkSe);
	AudioLoader::Load(AUDIO::SE_BOSS1_JUMPATTACK, boss1Ses.jumpAtkSe);
	AudioLoader::Load(AUDIO::SE_BOSS1_START, boss1Ses.startLineSe);
	AudioLoader::Load(AUDIO::SE_BOSS1_DAMAGE, boss1Ses.damageSe);
	AudioLoader::Load(AUDIO::SE_BOSS1_DEATH, boss1Ses.deathSe);
	AudioLoader::Load(AUDIO::SE_BOSS1_PANIC, boss1Ses.panicSe);
	AudioLoader::Load(AUDIO::SE_BOSS1_ANGRY, boss1Ses.angrySe);
	AudioLoader::Load(AUDIO::SE_BOSS1_WALLHIT, boss1Ses.wallHitSe);

}
