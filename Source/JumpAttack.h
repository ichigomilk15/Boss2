#pragma once
#include "AttackParent.h"

class JumpAttack : public AttackParent
{
public:
    JumpAttack(Character* parent, const int damage,
        const TargetAttackEnum target, const std::vector<DirectX::XMINT2> targetAttackPos, const float damageTimer = 0.0f)
        :
        AttackParent(parent, damage, target, targetAttackPos, damageTimer),
        boxHitRange(boxHitRange)
    {
        Initialize();
    };
    ~JumpAttack() {};

    // XVˆ—
    void Update(float elapsedTime)override;

private:
    void Initialize() override;
    bool SetTargetCharaKnockedPos();
private:
    float startAppearTimer = 0.0f;
    int boxHitRange = 0;
};