#include "NormalAttack.h"
#include "Stage.h"
#include "Character.h"

void NormalAttack::Update(float elapsedTime)
{
	if (startAppearTimer > 0.0f)
	{
		startAppearTimer -= elapsedTime;
		return;
	}

	//�_���[�W��^������ԏ���
	this->damageTimer -= elapsedTime;
	if (damageTimer <= 0.0f)
	{
		for (auto& e : targetAttack)
		{
			if (!e.isAttacked)
			{
				e.targetChara->ApplyDamage(damage);
				e.isAttacked = true;
			}
		}
	}

	// ��������
	this->lifeTimer -= elapsedTime;
	if (this->lifeTimer < 0.0f)
	{
		// �������폜
		Destroy();
	}
}

void NormalAttack::Initialize()
{
	this->lifeTimer = 2.0f;
	this->startAppearTimer = 0.0f;

	for (auto& pos : targetAttackPos)
	{
		Stage::Instance()->GetSquare(pos.x, pos.y)->SetType(Square::Type::AttackAreaChosen);
	}
	//this->weaponEffects.hitEffect = std::make_unique<Effect>("Data/Effect/Player/EnemyHit2.efk");
}
