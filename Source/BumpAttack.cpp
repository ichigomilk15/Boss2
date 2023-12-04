#include "BumpAttack.h"
#include "Stage.h"

void BumpAttack::Update(float elapsedTime)
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
				SetTargetCharaBumpPos();
				e.targetChara->ApplyDamage(damage);
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

void BumpAttack::Initialize()
{
	this->lifeTimer = 2.0f;
	this->startAppearTimer = 0.0f;

	for (auto& pos : targetAttackPos)
	{
		Stage::Instance()->GetSquare(pos.x, pos.y)->SetType(Square::Type::AttackAreaChosen);
	}
}

void BumpAttack::SetTargetCharaBumpPos()
{
	const int bumpMoveTarget = 1;
	for (auto& e : targetAttack)
	{
		auto pos = e.targetChara->GetPosition();
		switch (direction)
		{
		case CommonClass::DirectionFace::Back:
		case CommonClass::DirectionFace::Front:
			if (parent->GetWhichHorizontalSide({pos.x, pos.y}) > 0)
			{
				if (e.targetChara->IsTargetMovePosValid({ pos.x + bumpMoveTarget, pos.y }))
				{
					e.targetChara->SetTargetMovePosition({ pos.x + bumpMoveTarget, pos.y });
				}
			}
			else
			{
				if (e.targetChara->IsTargetMovePosValid({ pos.x - bumpMoveTarget, pos.y }))
				{
					e.targetChara->SetTargetMovePosition({ pos.x - bumpMoveTarget, pos.y });
				}
			}
			break;
		case CommonClass::DirectionFace::Left:
		case CommonClass::DirectionFace::Right:
			if (parent->GetWhichVerticalSide({ pos.x, pos.y }) > 0)
			{
				if (e.targetChara->IsTargetMovePosValid({ pos.x, pos.y + bumpMoveTarget }))
				{
					e.targetChara->SetTargetMovePosition({ pos.x, pos.y + bumpMoveTarget });
				}
			}
			else
			{
				if (e.targetChara->IsTargetMovePosValid({ pos.x, pos.y - bumpMoveTarget }))
				{
					e.targetChara->SetTargetMovePosition({ pos.x, pos.y - bumpMoveTarget });
				}
			}
			e.targetChara->SetState(State::KnockedBack_Init);
			break;
		}
	}
}
