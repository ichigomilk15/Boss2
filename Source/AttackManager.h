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

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// �e�ۓo�^
	void Register(AttackParent* attack);

	// �e�ۑS�폜
	void Clear();

	// �e�ې��擾
	int GetAttackCount() { return static_cast<int>(attacks.size()); }

	// �e�ێ擾
	AttackParent* GetProjectile(int index) { return attacks.at(index); }

	// �e�ۍ폜
	void Remove(AttackParent* attack);
private:
	std::vector<AttackParent*>		attacks;
	std::vector<AttackParent*>		removes;
};

