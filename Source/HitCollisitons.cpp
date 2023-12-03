#include "HitCollisions.h"


HitBox2D::HitBox2D(const DirectX::XMFLOAT2& leftTop, const DirectX::XMFLOAT2& BoxSize):
    leftTop(leftTop),BoxSize(BoxSize)
{
}

HitBox2D::~HitBox2D()
{
}

HitBox2D HitBox2D::CreateBoxFromTopLeft(const DirectX::XMFLOAT2& topleft, const DirectX::XMFLOAT2& BoxSize)
{
    return HitBox2D(topleft,BoxSize);
}

HitBox2D HitBox2D::CreateBoxFromCenter(const DirectX::XMFLOAT2& center, const DirectX::XMFLOAT2& BoxSize)
{
    DirectX::XMFLOAT2 topleft = { center.x - BoxSize.x * 0.5f,center.y - BoxSize.y * 0.5f };
    return HitBox2D(topleft,BoxSize);
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
