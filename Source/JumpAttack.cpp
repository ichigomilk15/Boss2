#include "JumpAttack.h"
#include "Stage.h"
#include "CameraController.h"

void JumpAttack::Update(float elapsedTime)
{
	if (startAppearTimer > 0.0f)
	{
		startAppearTimer -= elapsedTime;
		return;
	}

	//ダメージを与える期間処理
	this->damageTimer -= elapsedTime;
	if (damageTimer <= 0.0f)
	{
		for (auto& e : targetAttack)
		{
			if (!e.isAttacked)
			{
				if (SetTargetCharaKnockedPos())
				{
					int range = Stage::Instance()->GetTargetPosCost(e.targetChara->GetPosition(), centerAttackPos);
					if (range == 0)
					{
						e.targetChara->ApplyDamage(damageCenter);
					}
					else
					{
						e.targetChara->ApplyDamage(damageEdge);
					}
					e.targetChara->SetState(State::KnockedBack_Init);
					CameraController::Instance().ShakeCamera(1.05f, 6);
				}
				e.isAttacked = true;
			}
		}

		if (!isCardAttacked)
		{
			AttackCard();
		}
	}

	// 寿命処理
	this->lifeTimer -= elapsedTime;
	if (this->lifeTimer < 0.0f)
	{
		// 自分を削除
		Destroy();
	}
}

void JumpAttack::Initialize()
{
	this->lifeTimer = 2.0f;
	this->startAppearTimer = 0.0f;

	for (auto& pos : targetAttackPos)
	{
		Stage::Instance()->GetSquare(pos.x, pos.y)->SetType(Square::Type::AttackAreaChosen);
	}
}

bool JumpAttack::SetTargetCharaKnockedPos()
{
	const int bumpMoveTarget = 1;
	for (auto& e : targetAttack)
	{
		const DirectX::XMINT2 pos = e.targetChara->GetPosition();
		bool isTargetFound = false;
		for (auto& sqPos : targetAttackPos)
		{
			if (sqPos.x == pos.x && sqPos.y == pos.y)
			{
				isTargetFound = true;
				break;
			}
		}
		if (!isTargetFound)
			return false;

		Character* target = e.targetChara;
		for (int i = 1; i < 5; ++i)
		{
			//左
			if (target->IsTargetMovePosValid({ pos.x - i, pos.y }))
			{
				e.targetChara->SetTargetMovePosition({ pos.x - i, pos.y });
				isTargetFound = true;
				break;
			}
			//上
			if (target->IsTargetMovePosValid({ pos.x, pos.y - i }))
			{
				e.targetChara->SetTargetMovePosition({ pos.x, pos.y - i });
				isTargetFound = true;
				break;
			}
			//右
			if (target->IsTargetMovePosValid({ pos.x + i, pos.y }))
			{
				e.targetChara->SetTargetMovePosition({ pos.x + i, pos.y });
				isTargetFound = true;
				break;
			}
			//下
			if (target->IsTargetMovePosValid({ pos.x, pos.y + i }))
			{
				e.targetChara->SetTargetMovePosition({ pos.x, pos.y + i });
				isTargetFound = true;
				break;
			}
		}
		return isTargetFound;
	}
}