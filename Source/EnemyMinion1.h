#pragma once

#include "Graphics\Model.h"
#include "Enemy.h"
#include "Audio\AudioSource.h"

// �����ړ��̓G(��)
class EnemyMinion1 : public Enemy
{
public:
	EnemyMinion1(Character* p);

private:
	void UpdateState(float elapsedTime) override;

	//�_���[�W���󂯂���
	void OnDamaged() override;

	//���S�������ɌĂ΂��
	void OnDead() override;

	//�I�[�f�B�I�̏�����
	void InitializeAudio();

private:
	struct Minion1Ses
	{
		std::unique_ptr<AudioSource> attackSe;
		std::unique_ptr<AudioSource> damageSe;
		std::unique_ptr<AudioSource> deathSe;
		std::unique_ptr<AudioSource> walkSe;
	}minion1Ses;
};


