#pragma once

#include "Graphics\Model.h"
#include "Enemy.h"
#include "Audio\AudioSource.h"

enum ANIMATION_BOSS
{
	BossIdle,
	BossSpin,
	BossJump,
	BossDrop,
	BossDamage
};

// �����ړ��̓G(��)
class EnemyBoss1 : public Enemy
{
public:
	EnemyBoss1(Character* p);

private:
	void UpdateState(float elapsedTime) override;
	void InitializeAttack(float elapsedTime) override;
	State ChooseAct(float elapsedTime) override;
	//�o���v�U����̏���
	State AfterBumpAttack();

	void InitStunDefence();

	//�_���[�W���󂯂���
	void OnDamaged() override;

	//���S�������ɌĂ΂��
	void OnDead() override;

	//�I�[�f�B�I�̏�����
	void InitializeAudio();

private:
	struct BumpAttackDetail
	{
		DirectX::XMINT2 targetChargingMovePos = { -1, -1 };
		int attackPow = 0;
		int stunTurn = -1;	//�U����ǂɂԂ�������X�^���^�[���ʂ����߂�
		int stunDefence = 1;
	} bumpAttackDetail;
	struct JumpAttackDetail
	{
		DirectX::XMINT2 targetJumpMovePos = { -1, -1 };
		int attackPowCenter = 25;
		int attackPowEdge = 15;
	} jumpAttackDetail;

	struct Boss1Ses
	{
		std::unique_ptr<AudioSource> startLineSe;
		std::unique_ptr<AudioSource> attackLineSe;
		std::unique_ptr<AudioSource> bumpAtkSe;
		std::unique_ptr<AudioSource> jumpAtkSe;
		std::unique_ptr<AudioSource> damageSe;
		std::unique_ptr<AudioSource> deathSe;
		std::unique_ptr<AudioSource> angrySe;
		std::unique_ptr<AudioSource> panicSe;
		std::unique_ptr<AudioSource> wallHitSe;
	}boss1Ses;

	struct
	{
		std::unique_ptr<Effect> dizzy;
	}effects;
};


