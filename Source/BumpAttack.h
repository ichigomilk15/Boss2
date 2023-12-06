#pragma once
#include "AttackParent.h"

class BumpAttack : public AttackParent
{
public:
    BumpAttack(Character* parent, const int damage, const TargetAttackEnum target, const int direction, const std::vector<DirectX::XMINT2> targetAttackPos, const float damageTimer = 0.0f, const bool isActive = true) :
        AttackParent(parent, damage, target, targetAttackPos, damageTimer, isActive),
        direction(direction)
    {
        Initialize();
    };
    ~BumpAttack() {};

    // XVˆ—
    void Update(float elapsedTime)override;

private:
    void Initialize() override;
    bool SetTargetCharaBumpPos();

private:
    float startAppearTimer = 0.0f;
    int direction;
    //int bumpMoveTarget = 2; 
};