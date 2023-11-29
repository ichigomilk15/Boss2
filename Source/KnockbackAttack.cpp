#include "KnockbackAttack.h"
#include "Stage.h"

void KnockbackAttack::Update(float elapsedTime)
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
				for (int cost = knockbackCost; cost > 0; --cost)
				{
					DirectX::XMINT2 movePos = e.targetChara->GetPosition();
					switch (knockbackDir)
					{
					case CommonClass::Front:
						--movePos.y;
						break;
					case CommonClass::Back:
						++movePos.y;
						break;
					case CommonClass::Left:
						--movePos.x;
						break;
					case CommonClass::Right:
						++movePos.x;
						break;
					}
					if (e.targetChara->IsTargetMovePosValid(movePos))
					{
						e.targetChara->SetTargetMovePosition(movePos);
						e.targetChara->SetState(State::KnockedBack_Init);
						break;
					}
				}
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

void KnockbackAttack::Initialize()
{
	this->lifeTimer = 2.0f;
	this->startAppearTimer = 0.0f;

	for (auto& pos : targetAttackPos)
	{
		Stage::Instance()->GetSquare(pos.x, pos.y)->SetType(Square::Type::AttackAreaChosen);
	}
}
