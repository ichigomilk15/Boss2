#pragma once

#include "Graphics\Model.h"
#include "Enemy.h"

// �����ړ��̓G(��)
class EnemyBoss1 : public Enemy
{
public:
	EnemyBoss1(Character* p);

private:
	void UpdateState(float elapsedTime) override;
};


