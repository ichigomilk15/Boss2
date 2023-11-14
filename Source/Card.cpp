
#include "Card.h"
#include "Graphics/Model.h"
#include "Input/Input.h"

CardBase::CardBase(DirectX::XMFLOAT3 pos,DirectX::XMFLOAT2 size):
    pos(pos),
    size(size),
    sprite()
{

}

CardBase::~CardBase()
{
}

void CardBase::Render(ID3D11DeviceContext* dc, Shader* shader)
{
}

const bool CardBase::HitMouse()
{
    const Mouse& mouse = Input::Instance().GetMouse();
    return HitCheck(DirectX::XMFLOAT2(static_cast<float>(mouse.GetPositionX()), static_cast<float>(mouse.GetPositionY())));
}

const bool CardBase::HitCheck(DirectX::XMFLOAT2 screenPos)
{
    return (screenPos.x>pos.x&&screenPos.y>pos.y&&
        screenPos.x<pos.x+size.x&&screenPos.y<pos.y+size.y);
}
