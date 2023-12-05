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

// 直線移動の敵(仮)
class EnemyBoss1 : public Enemy
{
public:
	EnemyBoss1(Character* p);

private:
	void UpdateState(float elapsedTime) override;
	void InitializeAttack(float elapsedTime) override;
	State ChooseAct(float elapsedTime) override;
	//バンプ攻撃後の処理
	State AfterBumpAttack();

private:
	struct BumpAttackDetail
	{
		DirectX::XMINT2 targetChargingMovePos = { -1, -1 };
		int attackPow = 0;
		int stunTurn = -1;	//攻撃後壁にぶつかったらスタンターン量を決める
	} bumpAttackDetail;
	struct JumpAttackDetail
	{
		DirectX::XMINT2 targetJumpMovePos = { -1, -1 };
		int attackPow = 30;
	} jumpAttackDetail;
};


