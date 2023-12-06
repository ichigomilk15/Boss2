#include "AttackParent.h"
#include "PlayerManager.h"
#include "EnemyManager.h"
#include "Stage.h"
#include <algorithm>

AttackParent::AttackParent(Character* parent, const int damage, TargetAttackEnum target, const std::vector<DirectX::XMINT2> targetAttackPos, const float damageTimer, const bool isAttackActive) :
	parent(parent),
	damage(damage),
	damageTimer(damageTimer),
	lifeTimer(2.0f),
	isAttackActivated(isAttackActive)
{
	if (this->damageTimer > lifeTimer || this->damageTimer < 0.0f)
		this->damageTimer = lifeTimer;

	for (auto& pos : targetAttackPos)
	{
		this->targetAttackPos.emplace_back(pos);
	}

	switch (target)
	{
	case TargetAttackEnum::Target_Player:
	{
		DirectX::XMINT2 pPos = PlayerManager::Instance().GetFirstPlayer()->GetPosition();
		targetAttack.emplace_back(TargetAttack{ PlayerManager::Instance().GetFirstPlayer(), false });
		break;
	}
	break;
	case TargetAttackEnum::Target_Enemy:
	{
		for (auto& e : EnemyManager::Instance().GetList())
		{
			DirectX::XMINT2 ePos = e->GetPosition();
			DirectX::XMINT2 eSize = e->GetSize();
			auto listE = EnemyManager::Instance().GetList();
			std::vector<Character*> listTarget;
			for (auto& target : targetAttack)
			{
				listTarget.emplace_back(target.targetChara);
			}
			/*for (int y = ePos.y; y < ePos.y + eSize.y; y++)
			{
				for (int x = ePos.x; x < ePos.x + eSize.x; x++)
				{
					if (x == pos.x && y == pos.y)
					{*/
			if (std::find(listTarget.begin(), listTarget.end(), e) == listTarget.end())
			{
				targetAttack.emplace_back(TargetAttack{ e, false });
			}
			/*}
		}
	}*/
		}
	}
	break;
	}
	Initialize();
};

AttackParent::~AttackParent()
{
}

void AttackParent::Render(ID3D11DeviceContext* dc, Shader* shader)
{
}

void AttackParent::Destroy()
{
	isDestroy = true;
}

void AttackParent::Initialize()
{
}

void AttackParent::AttackCard()
{
	for (auto& sqPos : targetAttackPos)
	{
		if (Card* spCard = Stage::Instance()->GetSquare(sqPos.x, sqPos.y)->GetCard())
		{
			spCard->SetType(Card::Type::DEBUFF);
		}
	}
	isCardAttacked= true;
}
