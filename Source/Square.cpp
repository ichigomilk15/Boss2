
#include "Square.h"
#include "Graphics/Shader.h"
#include "Card.h"

Square::Square(const DirectX::XMFLOAT3& pos, CardBase* card, Type type):
    worldPos(pos),
    scale(5.0f,0.01f,5.0f),
    rotate(),
    card(card),
    type(type),
    model(std::make_unique<Model>("./Data/Model/Cube/Cube.mdl"))
{
}

Square::~Square()
{
}

void Square::Update(float elapsedTime)
{
    auto test = GetCard();
    DirectX::XMMATRIX Transform =
        DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
        DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rotate)) *
        DirectX::XMMatrixTranslation(worldPos.x, worldPos.y, worldPos.z);
    DirectX::XMFLOAT4X4 transform;
    DirectX::XMStoreFloat4x4(&transform, Transform);
    model->UpdateTransform(transform);
}

void Square::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model.get());
}

const bool Square::Raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::IntersectRayVsModel(start,end,model.get(),hit);
}
