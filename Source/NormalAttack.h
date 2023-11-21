#pragma once
#include "AttackParent.h"

class NormalAttack : public AttackParent
{
public:
    NormalAttack(Character* parent, const int damage, const std::vector<DirectX::XMINT2> targetAttackPos) :
        AttackParent(parent, damage, targetAttackPos)
    {
        Initialize();
    };
    ~NormalAttack() {};

    // XVˆ—
    void Update(float elapsedTime)override;

private:
    void Initialize() override;

private:
    float lifeTimer = 0.0f;
    float startAppearTimer = 0.0f;
};