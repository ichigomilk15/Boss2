#pragma once

#include "Graphics\Model.h"
#include "Enemy.h"

// �����ړ��̓G(��)
class EnemyMinion1 : public Enemy
{
public:
	EnemyMinion1(Character* p);


private:
	void UpdateState(float elapsedTime) override;
};


