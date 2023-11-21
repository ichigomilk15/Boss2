#pragma once

#include <vector>
#include "AttackParent.h"

class AttackManager
{
private:
	AttackManager() {};
	~AttackManager() { Clear(); };
public:
	inline static AttackManager& Instance() { static AttackManager instance; return instance; }

	// XVˆ—
	void Update(float elapsedTime);

	// •`‰æˆ—
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// ’eŠÛ“o˜^
	void Register(AttackParent* attack);

	// ’eŠÛ‘Síœ
	void Clear();

	// ’eŠÛ”æ“¾
	int GetAttackCount() { return static_cast<int>(attacks.size()); }

	// ’eŠÛæ“¾
	AttackParent* GetProjectile(int index) { return attacks.at(index); }

	// ’eŠÛíœ
	void Remove(AttackParent* attack);
private:
	std::vector<AttackParent*>		attacks;
	std::vector<AttackParent*>		removes;
};

