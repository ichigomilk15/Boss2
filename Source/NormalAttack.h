#pragma once
#include "AttackParent.h"

class NormalAttack : public AttackParent
{
public:
    NormalAttack(Character* parent, const int damage, const TargetAttackEnum target, const std::vector<DirectX::XMINT2> targetAttackPos, const float damageTimer = 0.0f) :
        AttackParent(parent, damage, target, targetAttackPos, damageTimer)
    {
        Initialize();
    };
    ~NormalAttack() {};

    // XVˆ—
    void Update(float elapsedTime)override;

private:
    void Initialize() override;

private:
    float startAppearTimer = 0.0f;
};