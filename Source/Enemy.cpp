#include "Enemy.h"
#include "EnemyManager.h"
#include "Stage.h"
#include "AttackManager.h"
#include "NormalAttack.h"

void Enemy::Update(float elapsedTime)
{
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
	shader->Draw(dc, model.get());
}

void Enemy::DrawDebugGUI()
{
	ImGui::Text("HP %d", health);
}

void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}

void Enemy::OnDead()
{
}

void Enemy::ResetStatus()
{
	Character::ResetStatus();
	actNo = 0;
	isActEnd = false;
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
			float targetX = position.x + (difX / abs(difX));
			float targetY = position.y;
			if (Stage::Instance()->GetSquare(targetX, targetY)->GetType() != Square::Type::Inaccessible)
			{
				isValidHorizontal = true;
			}
		}
	}
	//垂直チェック
	{
		if (difY != 0)
		{
			float targetX = position.x;
			float targetY = position.y + (difY / abs(difY));
			if (Stage::Instance()->GetSquare(targetX, targetY)->GetType() != Square::Type::Inaccessible)
			{
				isValidVertical = true;
			}
		}
	}

	if (isValidHorizontal && !isValidVertical)
	{
		float targetX = position.x + (difX / abs(difX));
		float targetY = position.y;
		if (Stage::Instance()->GetSquare(targetX, targetY)->GetType() != Square::Type::Inaccessible)
		{
			targetMovePos.x = targetX;
			targetMovePos.y = targetY;
			return true;
		}
	}
	if (isValidVertical && !isValidHorizontal /*&& abs(difY) > abs(difX)*/)
	{
		float targetX = position.x;
		float targetY = position.y + (difY / abs(difY));
		if (Stage::Instance()->GetSquare(targetX, targetY)->GetType() != Square::Type::Inaccessible)
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
			float targetX = position.x + (difX / abs(difX));
			float targetY = position.y;
			if (Stage::Instance()->GetSquare(targetX, targetY)->GetType() != Square::Type::Inaccessible)
			{
				targetMovePos.x = targetX;
				targetMovePos.y = targetY;
				return true;
			}
		}
		float targetX = position.x;
		float targetY = position.y + (difY / abs(difY));
		if (Stage::Instance()->GetSquare(targetX, targetY)->GetType() != Square::Type::Inaccessible)
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
	DirectX::XMINT2 dirPos = { player->GetPosition().x - position.x, player->GetPosition().y - position.y };

	if (dirPos.x > 0)
		SetDirection(CommonClass::DirectionFace::Right);
	else if (dirPos.x < 0)
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
