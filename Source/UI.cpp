
#include "UI.h"

UI::UI(const char* name):UI(std::string(name))
{
}

UI::UI(const std::string& name)
{
    this->name = name;
}

UI::~UI()
{
    for (auto& component : components)
    {
        delete component;
    }
}

void UI::AddChild(std::unique_ptr<UI> child)
{
    childs.emplace_back();
    childs.back().swap(child);
}

UI* UI::SearchChildFromName(const std::string& name)noexcept
{
    for (auto& child : childs)
    {
        if (child->name == name)return child.get();
    }

    return nullptr;
}

void UI::ChildsRender(ID3D11DeviceContext* dc)
{
    for (auto& child : childs)
    {
        child->ChildsRender(dc);
    }
}

RenderComponent::RenderComponent(const char* filename):
    Component(),sprite(std::make_unique<Sprite>(filename)),color(1.0f,1.0f,1.0f,1.0f)
{
}

void RenderComponent::Render(ID3D11DeviceContext* dc)
{
    const HitBox2D& Boxs = parent->GetHitBox();
    const DirectX::XMFLOAT2& LeftTop = Boxs.GetLeftTop();
    const DirectX::XMFLOAT2& Size = Boxs.GetBoxSize();
    sprite->Render(dc,
        LeftTop.x,LeftTop.y,
        Size.x,Size.y,
        .0f, .0f,
        sprite->GetTextureWidthf(), sprite->GetTextureHeightf(),
        .0f,
        color.x,color.y,color.z,color.w
    );
}
