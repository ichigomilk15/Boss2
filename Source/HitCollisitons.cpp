#include "HitCollisions.h"


HitBox2D::HitBox2D(const DirectX::XMFLOAT2& leftTop, const DirectX::XMFLOAT2& BoxSize):
    leftTop(leftTop),BoxSize(BoxSize)
{
}

HitBox2D::~HitBox2D()
{
}

const bool HitBox2D::Hit(const HitBox2D& dst) const noexcept
{
    return !(
        this->leftTop.x > dst.leftTop.x + dst.BoxSize.x ||
        this->leftTop.y > dst.leftTop.y + dst.BoxSize.y ||
        this->leftTop.x + this->BoxSize.x < dst.leftTop.x ||
        this->leftTop.y + this->BoxSize.y < dst.leftTop.y);
}

const bool HitBox2D::Hit(const DirectX::XMFLOAT2& pos) const noexcept
{
    return (
        this->leftTop.x < pos.x
        &&this->leftTop.y < pos.y
        &&this->leftTop.x+this->BoxSize.x > pos.x
        &&this->leftTop.y+this->BoxSize.y > pos.y);
}

void HitBox2D::AddVelocity(const DirectX::XMFLOAT2& vector)
{
    this->leftTop.x += vector.x;
    this->leftTop.y += vector.y;
}
