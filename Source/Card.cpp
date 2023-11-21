
#include "Graphics/Shader.h"

#include "Card.h"
#include "Graphics/Model.h"
#include "Input/Input.h"
#include "CardList.h"

#ifdef _DEBUG
#include "Graphics/ImGuiRenderer.h"
#endif // _DEBUG


//todo : ÉJÅ[ÉhâÊëúí«â¡ÇµÇΩÇ∆Ç´Ç…í«â¡ÇµÇƒÇ≠ÇæÇ≥Ç¢
const char*spriteName[static_cast<int>(Card::Type::MAX)] =
{
    "./Data/Sprite/Attack.png",//none
    "./Data/Sprite/Attack.png",//attack
    "./Data/Sprite/Defense.png",//defense
    "./Data/Sprite/Move.png",//move
    "./Data/Sprite/Attack.png",//special
    "./Data/Sprite/Attack.png",//bad
};

Card::Card(DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, const Type type) :
    pos(pos),
    center(DirectX::XMFLOAT2{pos.x+size.x*0.5f,pos.y+size.y*0.5f}),
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
    //à⁄ìÆ
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
        .0f,
        1.0f, 1.0f, 1.0f, 1.0f);
}

void Card::DrawIMGUI()
{
    if (ImGui::InputFloat2("position", &pos.x)) {};
    if (ImGui::InputFloat2("target", &targetPos.x)) {};
    if (ImGui::RadioButton("isMoveLock", moveLock)) {};
}

const bool Card::HitMouse()
{
    const Mouse& mouse = Input::Instance().GetMouse();
    return HitCheck(DirectX::XMFLOAT2(static_cast<float>(mouse.GetPositionX()), static_cast<float>(mouse.GetPositionY())));
}

const bool Card::HitCheck(DirectX::XMFLOAT2 screenPos)
{
    if (pos.x != targetPos.x || pos.y != targetPos.y)return false;//à⁄ìÆíÜÇÕîªíËÇéÊÇÁÇ»Ç¢
    return (screenPos.x>pos.x&&screenPos.y>pos.y&&
        screenPos.x<pos.x+size.x&&screenPos.y<pos.y+size.y);
}
