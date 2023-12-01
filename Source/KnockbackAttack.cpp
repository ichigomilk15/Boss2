#include "KnockbackAttack.h"
#include "Stage.h"

void KnockbackAttack::Update(float elapsedTime)
{
	if (startAppearTimer > 0.0f)
	{
		startAppearTimer -= elapsedTime;
		return;
	}

	//ƒ_ƒ[ƒW‚ð—^‚¦‚éŠúŠÔˆ—
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
						movePos.y += cost;
						break;
					case CommonClass::Back:
						movePos.y += cost;
						break;
					case CommonClass::Left:
						movePos.x += cost;
						break;
					case CommonClass::Right:
						movePos.x += cost;
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

	// Žõ–½ˆ—
	this->lifeTimer -= elapsedTime;
	if (this->lifeTimer < 0.0f)
	{
		// Ž©•ª‚ðíœ
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
