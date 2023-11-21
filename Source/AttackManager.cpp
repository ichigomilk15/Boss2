#include "AttackManager.h"
#include "Collision.h"

void AttackManager::Update(float elapsedTime)
{
	for (AttackParent* attack : attacks)
	{
		attack->Update(elapsedTime);
		if (attack->GetIsDestroy())
		{
			Remove(attack);
		}
	}
	// XV”jŠüˆ—
	for (AttackParent* attack : removes)
	{
		std::vector<AttackParent*>::iterator it = std::find(attacks.begin(), attacks.end(), attack);
		if (it != attacks.end())
		{
			attacks.erase(it);
		}

		delete attack;
	}
	removes.clear();

}

void AttackManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (AttackParent* attack : attacks)
	{
		attack->Render(dc, shader);
	}
}

void AttackManager::Register(AttackParent* attack)
{
	attacks.emplace_back(attack);
}

void AttackManager::Clear()
{
	for (AttackParent* attack : attacks)
	{
		delete attack;
	}
	attacks.clear();
}

void AttackManager::Remove(AttackParent* attack)
{
	removes.emplace_back(attack);
}
