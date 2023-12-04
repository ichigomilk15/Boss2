#pragma once

#include "Graphics\Model.h"
#include "Enemy.h"

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
};


