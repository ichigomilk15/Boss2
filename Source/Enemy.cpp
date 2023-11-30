#include "Enemy.h"
#include "EnemyManager.h"
#include "Stage.h"
#include "AttackManager.h"
#include "NormalAttack.h"

void Enemy::Update(float elapsedTime)
{
	// ���͏����X�V
	UpdateVelocity(elapsedTime);

	//�X�e�[�g�X�V����
	UpdateState(elapsedTime);

	//�I�u�W�F�N�g�s����X�V
	UpdateTransform();

	this->model->UpdateAnimation(elapsedTime);
	//���f���s��X�V
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

	//�����`�F�b�N
	{
		if (difX != 0)
		{
			int targetX = position.x + (difX / abs(difX));
			int targetY = position.y;
			if (Stage::Instance()->GetSquare(targetX, targetY)->GetType() != Square::Type::Inaccessible)
			{
				isValidHorizontal = true;
			}
		}
	}
	//�����`�F�b�N
	{
		if (difY != 0)
		{
			int targetX = position.x;
			int targetY = position.y + (difY / abs(difY));
			if (Stage::Instance()->GetSquare(targetX, targetY)->GetType() != Square::Type::Inaccessible)
			{
				isValidVertical = true;
			}
		}
	}

	if (isValidHorizontal && !isValidVertical)
	{
		int targetX = position.x + (difX / abs(difX));
		int targetY = position.y;
		if (Stage::Instance()->GetSquare(targetX, targetY)->GetType() != Square::Type::Inaccessible)
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
			int targetX = position.x + (difX / abs(difX));
			int targetY = position.y;
			if (Stage::Instance()->GetSquare(targetX, targetY)->GetType() != Square::Type::Inaccessible)
			{
				targetMovePos.x = targetX;
				targetMovePos.y = targetY;
				return true;
			}
		}
		int targetX = position.x;
		int targetY = position.y + (difY / abs(difY));
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

	attackSq = Stage::Instance()->GetSquaresByDirection(this->position.x, this->position.y, 2, this->GetDirection());
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

	//�U���͈͂Ƀv���C���[������
	auto attackSquares = Stage::Instance()->GetSquaresEdgeAdjacent(position.x, position.y, 2);
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
