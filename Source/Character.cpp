#define NOMINMAX
#include "Character.h"
#include "CustomMathf.h"
#include "Stage.h"
#undef NOMINMAX

void Character::UpdateTransform()
{
	SetAngle(CommonClass::directionMaps.find(direction)->second);
	float posWorldFinalX = positionWorld.x + pivotAdjustPosWorld.x;
	float posWorldFinalY = positionWorld.y + pivotAdjustPosWorld.y;
	float posWorldFinalZ = positionWorld.z + pivotAdjustPosWorld.z;

	//�X�P�[���s��
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//��]�s����쐬
	//DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
	DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
	DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
	DirectX::XMMATRIX R = Y * X * Z;
	//�ʒu�s������
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(posWorldFinalX, posWorldFinalY, posWorldFinalZ);

	//�O�̍s����g�ݍ��킹, ���[���h�s����쐬
	DirectX::XMMATRIX W = S * R * T;

	//�v�Z�������[���h�s������o��
	DirectX::XMStoreFloat4x4(&transform, W);
}

void Character::SetPositionWorld(const DirectX::XMINT2& position)
{
	this->position = position;
	this->positionWorld = Stage::Instance()->GetSquare(position.x, position.y)->GetWorldPos();
}

void Character::SetTargetMoveTranslation(const DirectX::XMINT2& moveDir)
{
	if (IsTargetMovePosValid({ position.x + moveDir.x, position.y + moveDir.y }))
	{
		targetMovePos.x = position.x + moveDir.x;
		targetMovePos.y = position.y + moveDir.y;
	}
}

const std::vector<DirectX::XMINT2> Character::GetSquaresPosition() const
{
	std::vector<DirectX::XMINT2> squaresPos;
	for (int y = position.y; y < position.y + size.y; ++y)
	{
		for (int x = position.x; x < position.x + size.x; ++x)
		{
			squaresPos.emplace_back(DirectX::XMINT2{ x, y });
		}
	}
	return squaresPos;
}
const std::vector<DirectX::XMINT2> Character::GetSquaresPositionX(const int row) const
{
	std::vector<DirectX::XMINT2> squaresPos;
	for (int x = position.x; x < position.x + size.x; ++x)
	{
		squaresPos.emplace_back(DirectX::XMINT2{ x, row });
	}
	return squaresPos;
}

const std::vector<DirectX::XMINT2> Character::GetSquaresPositionY(const int column) const
{
	std::vector<DirectX::XMINT2> squaresPos;
	for (int y = position.y; y < position.y + size.y; ++y)
	{
		squaresPos.emplace_back(DirectX::XMINT2{ column, y });
	}
	return squaresPos;
}

void Character::SetDirection(int dir)
{
	dir %= CommonClass::directionMaps.size();
	direction = dir;
}

void Character::SetDirection(const DirectX::XMINT2 targetPos)
{
	for (int y = position.y; y < position.y + size.y; y++)
	{
		for (int x = position.x; x < position.x + size.x; x++)
		{
			if ((x == targetPos.x || y == targetPos.y) && !(x == targetPos.x && y == targetPos.y))
			{
				SetDirection(CommonClass::GetDirectionTarget({ x, y }, targetPos));
				return;
			}
		}
	}
	SetDirection(CommonClass::GetDirectionTarget(position, targetPos));
}

void Character::UpdateVelocity(float elapsedTime)
{
	// �o�߃t���[��
	float elapsedFrame = 60.0f * elapsedTime;


	{
		isGround = false;
		positionWorld.y += velocity.y * elapsedFrame;
		velocity.y += Common::gravity * elapsedTime;

		if (positionWorld.y <= pivotAdjustPosWorld.y)
		{
			positionWorld.y = pivotAdjustPosWorld.y;
			velocity.y = 0.0f;
			isGround = true;
		}
	}

	if (IsMoving())
	{
		const DirectX::XMFLOAT3 targetPosWorld = Stage::Instance()->GetWorldPos(targetMovePos);
		const DirectX::XMFLOAT3 formerPosWorld = Stage::Instance()->GetWorldPos(position);
		float rangeX = targetPosWorld.x - formerPosWorld.x;
		float rangeZ = targetPosWorld.z - formerPosWorld.z;
		this->positionWorld.x += rangeX * 1 / (60.0f * Common::moveSpeed) * elapsedFrame;
		this->positionWorld.z += rangeZ * 1 / (60.0f * Common::moveSpeed) * elapsedFrame;

		if (moveTimer >= Common::moveSpeed)
		{
			this->position = this->targetMovePos;
			this->positionWorld = Stage::Instance()->GetWorldPos(this->position);
			moveTimer = 0.0f;
		}
		moveTimer += elapsedTime;
	}
}

bool Character::IsTargetMovePosValid(const DirectX::XMINT2& targetPos)
{
	for (int y = targetPos.y; y < targetPos.y + size.y; ++y)
	{
		for (int x = targetPos.x; x < targetPos.x + size.x; ++x)
		{
			if (!Stage::Instance()->IsInArea(x, y))
				return false;
			auto findChara = Stage::Instance()->GetSquare(x, y)->GetCharacter();
			if (findChara != nullptr && findChara != this)
			{
				return false;
			}
		}
	}
	return true;
}

bool Character::IsTargetMoveAttackPosValid(const DirectX::XMINT2& targetPos)
{
	for (int y = targetPos.y; y < targetPos.y + size.y; ++y)
	{
		for (int x = targetPos.x; x < targetPos.x + size.x; ++x)
		{
			if (!Stage::Instance()->IsInArea(x, y))
				return false;
		}
	}
	return true;
}

void Character::Heal(const int hp)
{
	this->health += hp;
	this->health = std::min(health, maxHealth);
}

int Character::GetWhichHorizontalSide(const DirectX::XMINT2& pos)
{
	int pivotX = position.x + ((size.x + 1) / 2 - 1);
	return pos.x - pivotX;
}

int Character::GetWhichVerticalSide(const DirectX::XMINT2& pos)
{
	int pivotY = position.y + ((size.y + 1) / 2 - 1);
	return pos.y - pivotY;
}

void Character::ResetStatus()
{
	shield = 0;
	block = 0;
	isActEnd = false;
}

bool Character::ApplyDamage(int damage)
{
	//�_���[�W���O�̏ꍇ�͌��N��Ԃ�ύX����K�v���Ȃ�
	if (damage <= 0) return false;
	//���S���Ă���ꍇ�͌��N��Ԃ�ύX���Ȃ�
	if (health <= 0) return false;

	damage -= block;
	if (damage <= 0)
		return false;

	if (shield > 0)
	{
		int tempDamage = (shield >= damage) ? damage : shield;
		shield -= tempDamage;
		damage -= tempDamage;
		if (damage <= 0)
			return false;
	}

	//�_���[�W����
	health -= damage;

	//���S�ʒm
	if (health <= 0)
	{
		OnDead();
	}
	//�_���[�W�ʒm
	else
	{
		OnDamaged();
	}

	//���N��Ԃ��ύX�����ꍇ��true��Ԃ�
	return true;
}

bool Character::IsMoving() const
{
	return ((position.x != targetMovePos.x || position.y != targetMovePos.y) &&
		Stage::Instance()->IsInArea(targetMovePos.x, targetMovePos.y));
}

//bool Character::IsAttacking() const
//{
//	return false;
//}

void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
	//���͂ɗ͂�^����
	velocity.x += impulse.x;
	velocity.y += impulse.y;
	velocity.z += impulse.z;
}

void Character::Move(int vx, int vy)
{
	int x = position.x + vx;
	int y = position.y + vy;
	if (Stage::Instance()->IsInArea(x, y))
	{
		//�ړ������x�N�g����ݒ�
		position.x = x;
		position.y = y;
	}
}
