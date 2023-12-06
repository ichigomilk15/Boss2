
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
    "./Data/Sprite/Back.png",//none
    "./Data/Sprite/Attack.png",//attack
    "./Data/Sprite/Defense.png",//defense
    "./Data/Sprite/Move.png",//move
    "./Data/Sprite/buff.png",//special
    "./Data/Sprite/debuff.png",//bad
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
    if (!moveLock&&IsMoving())
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
#if _DEBUG
    if (ImGui::InputFloat2("position", &pos.x)) {};
    if (ImGui::InputFloat2("target", &targetPos.x)) {};
    if (ImGui::RadioButton("isMoveLock", moveLock)) {};
#endif
}

const bool Card::HitMouse()
{
    const Mouse& mouse = Input::Instance().GetMouse();
    return HitCheck(DirectX::XMFLOAT2(static_cast<float>(mouse.GetPositionX()), static_cast<float>(mouse.GetPositionY())));
}

const bool Card::HitCheck(DirectX::XMFLOAT2 screenPos)
{
    if (IsMoving())return false;//à⁄ìÆíÜÇÕîªíËÇéÊÇÁÇ»Ç¢
    return (screenPos.x>pos.x&&screenPos.y>pos.y&&
        screenPos.x<pos.x+size.x&&screenPos.y<pos.y+size.y);
}

void Card::Swap(Card* card)
{
    std::swap(card->type, this->type);
    std::swap(card->targetPos, this->targetPos);
    std::swap(card->pos, this->pos);

    this->sprite.swap(card->sprite);
}

const bool Card::IsMoving() const noexcept
{
    return (pos.x != targetPos.x || pos.y != targetPos.y);
}

void Card::SetType(const Type type)
{
    this->type = type;
    this->sprite = std::make_unique<Sprite>(spriteName[static_cast<int>(type)]);
}
