
#include "Graphics/Shader.h"

#include "Card.h"
#include "Graphics/Model.h"
#include "Input/Input.h"

//todo : カード画像追加したときに追加してください
const char*spriteName[static_cast<int>(Card::Type::MAX)] =
{
    "",
    "./Data/Sprite/Attack.png",
    "./Data/Sprite/Defense.png",
    "./Data/Sprite/Move.png",
    "",
    "",
};

Card::Card(DirectX::XMFLOAT2 pos,DirectX::XMFLOAT2 size,Type type):
    pos(pos),
    targetPos(pos),
    size(size),
    type(type),
    sprite(std::make_unique<Sprite>(spriteName[static_cast<int>(type)]))
{

}

Card::~Card()
{
}

void Card::Update(float elapsedTime)
{
    Mouse& mouse = Input::Instance().GetMouse();
    //マウスによる操作
    if (this->HitCheck(DirectX::XMFLOAT2{ static_cast<float>(mouse.GetPositionX()),static_cast<float>(mouse.GetPositionY()) }))
    {
        if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
        {
            moveLock = true;
        }
    }
    if (moveLock&& mouse.GetButton() & Mouse::BTN_LEFT)
    {
        pos.x += static_cast<float>(mouse.GetPositionX() - mouse.GetOldPositionX());
        pos.y += static_cast<float>(mouse.GetPositionY() - mouse.GetOldPositionY());
    }
    else if (mouse.GetButtonUp() & Mouse::BTN_LEFT)
    {
        moveLock = false;
    }

    //移動
    if (!moveLock&&( pos.x != targetPos.x || pos.y != targetPos.y))
    {
        DirectX::XMVECTOR Pos = DirectX::XMLoadFloat2(&pos);
        DirectX::XMVECTOR Target = DirectX::XMLoadFloat2(&targetPos);
        const float moveSpeed = MOVE_SPEED * elapsedTime;
        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Target, Pos);
        if (moveSpeed*moveSpeed > DirectX::XMVectorGetX(DirectX::XMVector2LengthSq(Vec)))
        {
            this->pos = this->targetPos;
        }
        else
        {
            Vec = DirectX::XMVector2Normalize(Vec);
            Vec = DirectX::XMVectorScale(Vec, moveSpeed);
            DirectX::XMStoreFloat2(&pos, DirectX::XMVectorAdd(Pos, Vec));
        }
    }
}

void Card::Render(ID3D11DeviceContext* dc)
{
    sprite->Render(dc,
        pos.x, pos.y,
        size.x, size.y,
        .0f, .0f,
        static_cast<float>(sprite->GetTextureWidth()),static_cast<float>(sprite->GetTextureHeight()),
        DirectX::XMConvertToRadians(.0f),
        1.0f, 1.0f, 1.0f, 1.0f);
}

const bool Card::HitMouse()
{
    const Mouse& mouse = Input::Instance().GetMouse();
    return HitCheck(DirectX::XMFLOAT2(static_cast<float>(mouse.GetPositionX()), static_cast<float>(mouse.GetPositionY())));
}

const bool Card::HitCheck(DirectX::XMFLOAT2 screenPos)
{
    return (screenPos.x>pos.x&&screenPos.y>pos.y&&
        screenPos.x<pos.x+size.x&&screenPos.y<pos.y+size.y);
}
