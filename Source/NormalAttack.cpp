#include "NormalAttack.h"
#include "Stage.h"
#include "Character.h"
#include "CameraController.h"

void NormalAttack::Update(float elapsedTime)
{
	if (!isAttackActivated)
		return;

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
				for (auto& sq : targetAttackPos)
				{
					//if (sq.x == e.targetChara->GetPosition().x && sq.y == e.targetChara->GetPosition().y)
					auto target = Stage::Instance()->GetSquare(sq.x, sq.y)->GetCharacter();
					if (target && target == e.targetChara)
					{
						e.targetChara->ApplyDamage(damage);
						if (damage >= 15)
							CameraController::Instance().ShakeCamera(1.0, 6);
						else
							CameraController::Instance().ShakeCamera(0.25f, 1);
						break;
					}
				}
				e.isAttacked = true;
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
