#pragma once
#include "AttackParent.h"

class JumpAttack : public AttackParent
{
public:
    JumpAttack(Character* parent, const int damageCenter, const int damageEdge,
        const TargetAttackEnum target, const DirectX::XMINT2 centerAttackPos, const std::vector<DirectX::XMINT2> targetAttackPos, const float damageTimer = 0.0f, const bool isActive = true)
        :
        AttackParent(parent, damageCenter, target, targetAttackPos, damageTimer, isActive),
        boxHitRange(boxHitRange),
        damageCenter(damageCenter),
        damageEdge(damageEdge),
        centerAttackPos(centerAttackPos)
    {
        Initialize();
    };
    ~JumpAttack() {};

    // çXêVèàóù
    void Update(float elapsedTime)override;

private:
    void Initialize() override;
    bool SetTargetCharaKnockedPos();
private:
    float startAppearTimer = 0.0f;
    int boxHitRange = 0;
    int damageCenter = 25;
    int damageEdge = 15;
    DirectX::XMINT2 centerAttackPos = { -1, -1 };
};