#pragma once
#include "AttackParent.h"

class KnockbackAttack : public AttackParent
{
public:
    KnockbackAttack(Character* parent, const int damage, const int knockbackCost, const int knockbackDir, 
        const TargetAttackEnum target, const std::vector<DirectX::XMINT2> targetAttackPos, const float damageTimer = 0.0f) 
        :
        AttackParent(parent, damage, target, targetAttackPos, damageTimer),
        knockbackCost(knockbackCost),
        knockbackDir(knockbackDir)
    {
        Initialize();
    };
    ~KnockbackAttack() {};

    // çXêVèàóù
    void Update(float elapsedTime)override;

private:
    void Initialize() override;

private:
    float startAppearTimer = 0.0f;
    int knockbackCost = 1;
    int knockbackDir;
};