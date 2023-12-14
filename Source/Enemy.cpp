#include "Enemy.h"
#include "EnemyManager.h"
#include "Stage.h"
#include "AttackManager.h"
#include "NormalAttack.h"
#include "Player.h"
#include "PhaseManager.h"

void Enemy::Update(float elapsedTime)
{
	UpdateDeath(elapsedTime);

	// 速力処理更新
	UpdateVelocity(elapsedTime);

	//ステート更新処理
	UpdateState(elapsedTime);

	//オブジェクト行列を更新
	UpdateTransform();

	this->model->UpdateAnimation(elapsedTime);
	//モデル行列更新
	model->UpdateTransform(transform);
}

void Enemy::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	//上2マスにプレイヤーがいたら半透明化にする
	if (IsConcealPlayer())
	{
		MakeHalfTransparent();
	}
	else
	{
		MakeFullTransparent();
	}
	shader->Draw(dc, model.get());
}

void Enemy::DrawDebugGUI()
{
	ImGui::InputInt("HP", &health);
}

void Enemy::Destroy()
{
	destroyedStatus.isDestroyed = true;
	EnemyManager::Instance().Remove(this);
}

void Enemy::OnDead()
{
	//Destroy();
	destroyedStatus.destroyedTime = 2.0f;
	state = State::Death_Init;
}

void Enemy::ResetStatus()
{
	Character::ResetStatus();
	actNo = 0;
}

bool Enemy::ChooseTargetMove(float elapsedTime)
{
	int difX = player->GetPosition().x - position.x;
	int difY = player->GetPosition().y - position.y;

	if (difX == 0 && difY == 0) return false;
	if (Stage::Instance()->IsAdjacent({ position.x, position.y }, player->GetPosition())) return false;

	bool isValidHorizontal = false;
	bool isValidVertical = false;

	//水平チェック
	{
		if (difX != 0)
		{
			int targetX = position.x + (difX / abs(difX));
			int targetY = position.y;
			if (IsTargetMovePosValid({ targetX, targetY }))
			{
				isValidHorizontal = true;
			}
		}
	}
	//垂直チェック
	{
		if (difY != 0)
		{
			int targetX = position.x;
			int targetY = position.y + (difY / abs(difY));
			if (IsTargetMovePosValid({ targetX, targetY }))
			{
				isValidVertical = true;
			}
		}
	}

	if (isValidHorizontal && !isValidVertical)
	{
		int targetX = position.x + (difX / abs(difX));
		int targetY = position.y;
		if (IsTargetMovePosValid({ targetX, targetY }))
		{
			targetMovePos.x = targetX;
			targetMovePos.y = targetY;
			return true;
		}
	}
	if (isValidVertical && !isValidHorizontal /*&& abs(difY) > abs(difX)*/)
	{
		int targetX = position.x;
		int targetY = position.y + (difY / abs(difY));
		if (IsTargetMovePosValid({ targetX, targetY }))
		{
			targetMovePos.x = targetX;
			targetMovePos.y = targetY;
			return true;
		}
	}

	if (isValidHorizontal && isValidVertical)
	{
		if (abs(difX) > abs(difY))
		{
			int targetX = position.x + (difX / abs(difX));
			int targetY = position.y;
			if (IsTargetMovePosValid({ targetX, targetY }))
			{
				targetMovePos.x = targetX;
				targetMovePos.y = targetY;
				return true;
			}
		}
		int targetX = position.x;
		int targetY = position.y + (difY / abs(difY));
		if (IsTargetMovePosValid({ targetX, targetY }))
		{
			targetMovePos.x = targetX;
			targetMovePos.y = targetY;
			return true;
		}
	}

	return false;
}

void Enemy::InitializeAttack(float elapsedTime)
{
	if (attack && !attack->GetIsDestroy()) return;
	std::vector<Square*> attackSq;
	//DirectX::XMINT2 dirPos = { player->GetPosition().x - position.x, player->GetPosition().y - position.y };

	SetDirection(player->GetPosition());
	/*if (dirPos.x > 0)
		SetDirection(CommonClass::DirectionFace::Right);
	else if (dirPos.x < 0)
		SetDirection(CommonClass::DirectionFace::Left);
	else if (dirPos.y > 0)
		SetDirection(CommonClass::DirectionFace::Back);
	else if (dirPos.y < 0)
		SetDirection(CommonClass::DirectionFace::Front);*/

	//todo:: double check front & back (range)
	switch (GetDirection())
	{
	case CommonClass::DirectionFace::Back:
		for (auto& pos : GetSquaresPositionX(position.y + size.y - 1))
		{
			for (auto& sq : Stage::Instance()->GetSquaresByDirection(pos.x, pos.y, attackRange, CommonClass::DirectionFace::Back))
			{
				attackSq.emplace_back(sq);
			}
		}
		break;
	case CommonClass::DirectionFace::Front:
		for (auto& pos : GetSquaresPositionX(position.y))
		{
			for (auto& sq : Stage::Instance()->GetSquaresByDirection(pos.x, pos.y, attackRange, CommonClass::DirectionFace::Front))
			{
				attackSq.emplace_back(sq);
			}
		}
		break;
	case CommonClass::DirectionFace::Left:
		for (auto& pos : GetSquaresPositionY(position.x))
		{
			for (auto& sq : Stage::Instance()->GetSquaresByDirection(pos.x, pos.y, attackRange, CommonClass::DirectionFace::Left))
			{
				attackSq.emplace_back(sq);
			}
		}
		break;
	case CommonClass::DirectionFace::Right:
	default:
		for (auto& pos : GetSquaresPositionY(position.x + size.x - 1))
		{
			for (auto& sq : Stage::Instance()->GetSquaresByDirection(pos.x, pos.y, attackRange, CommonClass::DirectionFace::Right))
			{
				attackSq.emplace_back(sq);
			}
		}
		break;
	}
	std::vector<DirectX::XMINT2> posVec;
	for (auto& sq : attackSq)
	{
		posVec.emplace_back(sq->GetPos());
	}
	attack = new NormalAttack(this, attackPower, TargetAttackEnum::Target_Player, posVec, 0.5f);
	AttackManager::Instance().Register(attack);

	for (auto& square : attackSq)
	{
		square->SetType(Square::Type::AttackArea);
	}
}

State Enemy::ChooseAct(float elapsedTime)
{
	const DirectX::XMINT2 playerPos = player->GetPosition();
	if (Stage::Instance()->IsAdjacent(this, player) >= 0)
	{
		InitializeAttack(elapsedTime);
		return State::Attack_Init;
	}

	int cost = INT_MAX;
	for (int y = position.y; y < position.y + size.y; y++)
	{
		for (int x = position.x; x < position.x + size.x; x++)
		{
			int tempCost = Stage::Instance()->GetTargetPosCost({ x, y }, player->GetPosition());
			cost = (cost > tempCost) ? tempCost : cost;
		}
	}
	auto test = IsInTheSameRow(*player);
	auto test2 = IsInTheSameColumn(*player);

	if (cost <= attackRange &&
		(IsInTheSameRow(*player) || IsInTheSameColumn(*player)))
	{
		InitializeAttack(elapsedTime);
		return State::Attack_Init;
	}
	//攻撃範囲にプレイヤーを検索
	/*auto attackSquares = Stage::Instance()->GetSquaresEdgeAdjacent(position.x, position.y, 2);
	for (auto& sq : attackSquares)
	{
		const auto sqPos = sq->GetPos();
		if (sqPos.x == playerPos.x && sqPos.y == playerPos.y)
		{
			InitializeAttack(elapsedTime);
			return State::Attack_Init;
			break;
		}
	}*/

	return State::Move_Init;
}

void Enemy::MakeHalfTransparent()
{
	model->ChangeMaterialColor(0, { 1.0f, 1.0f, 1.0f, 0.5f });
}
void Enemy::MakeFullTransparent()
{
	model->ChangeMaterialColor(0, { 1.0f, 1.0f, 1.0f, 1.0f });
}

void Enemy::UpdateDeath(float elapsedTime)
{
	if (!GetIsDead())
		return;

	destroyedStatus.destroyedTime -= elapsedTime;
	if (destroyedStatus.destroyedTime <= 0.0f)
	{
		Destroy();
	}
}

bool Enemy::IsConcealPlayer()
{
	if (enemyType != ENEMY_TYPE::BOSS1) return false;

	for (auto& sq : GetSquaresPositionX(GetPosition().y))
	{
		//上1マスにプレイヤーがいた判定
		if (Stage::Instance()->IsInArea(sq.x, sq.y - 1))
		{
			auto chara = Stage::Instance()->GetSquare(sq.x, sq.y - 1)->GetCharacter();
			if (chara && PhaseManager::Instance().GetFhase() <= PhaseManager::Phase::Phase_PlayerAct)
			{
				return true;
			}
		}
		//上2マスにプレイヤーがいた判定
		if (Stage::Instance()->IsInArea(sq.x, sq.y - 2))
		{
			auto chara = Stage::Instance()->GetSquare(sq.x, sq.y - 2)->GetCharacter();
			if (chara && PhaseManager::Instance().GetFhase() <= PhaseManager::Phase::Phase_PlayerAct)
			{
				return true;
			}
		}
	}
	return false;
}
