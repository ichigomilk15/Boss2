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

// ’¼üˆÚ“®‚Ì“G(‰¼)
class EnemyBoss1 : public Enemy
{
public:
	EnemyBoss1(Character* p);

private:
	void UpdateState(float elapsedTime) override;
	void InitializeAttack(float elapsedTime) override;

	//ƒoƒ“ƒvUŒ‚Œã‚Ìˆ—
	State AfterBumpAttack();

private:
	struct BumpAttackDetail
	{
		DirectX::XMINT2 targetChargingMovePos = { -1, -1 };
		int attackPow = 0;
		int stunTurn = 1;	//UŒ‚Œã•Ç‚É‚Ô‚Â‚©‚Á‚½‚çƒXƒ^ƒ“‚³‚ê‚é
	} bumpAttackDetail;
	struct JumpAttackDetail
	{
		DirectX::XMINT2 targetJumpMovePos = { -1, -1 };
		int attackPowCenter = 25;
		int attackPowEdge = 15;
	} jumpAttackDetail;
};


