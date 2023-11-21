#pragma once
#include "Graphics/Graphics.h"
#include "Graphics/Model.h"
#include "Effect.h"

class Character;

class AttackParent
{
public: //function
	AttackParent(Character* parent, const float damage, const std::vector<DirectX::XMINT2> targetAttackPos = {}) :parent(parent), damage(damage)
	{
		for (auto& pos : targetAttackPos)
		{
			this->targetAttackPos.emplace_back(pos);
		}
		Initialize();
	};
	virtual ~AttackParent();

	// XVˆ—
	virtual void Update(float elapsedTime) = 0;

	// •`‰æˆ—
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader);

	// ”jŠü
	void Destroy();

	//parent
	Character* GetParent()const { return parent; }

	const bool GetIsDestroy()const { return isDestroy; }

	//Getter&Setter*************************************************************************
#if 1
	const std::vector<DirectX::XMINT2> GetTargetAttackPos() const { return targetAttackPos; }
#endif
protected: //function
	virtual void Initialize();

protected: //members

	std::vector<DirectX::XMINT2>        targetAttackPos = {  };
	DirectX::XMFLOAT3	    scale = { 1,1,1 };
	int                     damage = 1;
	bool                    isDestroy = false;
	Character* parent;
};