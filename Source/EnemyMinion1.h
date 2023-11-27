#pragma once

#include "Graphics\Model.h"
#include "Enemy.h"

// 直線移動の敵(仮)
class EnemyMinion1 : public Enemy
{
public:
	EnemyMinion1(Character* p);

	//移動の更新処理
	bool UpdateMove(float elapsedTime)override;

private:
	void UpdateState(float elapsedTime) override;
	//セットカードによるアクションを決める更新処理
	State ChooseAct(float elapsedTime);

	//攻撃アクション更新処理
	void InitializeAttack(float elapsedTime) override;
};


