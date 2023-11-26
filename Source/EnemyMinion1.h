#pragma once

#include "Graphics\Model.h"
#include "Enemy.h"

// �����ړ��̓G(��)
class EnemyMinion1 : public Enemy
{
public:
	EnemyMinion1(Character* p);

	//�ړ��̍X�V����
	bool UpdateMove(float elapsedTime)override;

private:
	void UpdateState(float elapsedTime) override;
	//�Z�b�g�J�[�h�ɂ��A�N�V���������߂�X�V����
	State ChooseAct(float elapsedTime);

	//�U���A�N�V�����X�V����
	void InitializeAttack(float elapsedTime) override;
};


