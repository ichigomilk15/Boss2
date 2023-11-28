#pragma once

#include "Graphics\Model.h"
#include "Enemy.h"

// ’¼üˆÚ“®‚Ì“G(‰¼)
class EnemyBoss1 : public Enemy
{
public:
	EnemyBoss1(Character* p);

private:
	void UpdateState(float elapsedTime) override;
};


