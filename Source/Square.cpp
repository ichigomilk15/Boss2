
#include "Square.h"
#include "Graphics/Shader.h"
#include "Card.h"
#include "Stage.h"

Square::Square(const DirectX::XMFLOAT3& pos):
    worldPos(pos),
    scale(5.0f*0.2f,0.01f,5.0f*0.2f),
    rotate(),
    card(nullptr),
    type(Type::NONE),
    SquareBorder(Stage::Instance()->squareBorder),
    SquareArea(Stage::Instance()->squareArea)
{
}

Square::~Square()
{
}

void Square::Update(float elapsedTime)
{

}

void Square::Render(ID3D11DeviceContext* dc, Shader* shader)
{

    if (!SquareBorder.expired())
    {
        //枠の表示
        std::shared_ptr<Model> model = this->SquareBorder.lock();
        DirectX::XMMATRIX Transform =
            DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
            DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rotate)) *
            DirectX::XMMatrixTranslation(worldPos.x, worldPos.y, worldPos.z);
        DirectX::XMFLOAT4X4 transform;
        DirectX::XMStoreFloat4x4(&transform, Transform);
        model->UpdateTransform(transform);
        shader->Draw(dc, model.get());

        //何かのアクション範囲なら板を表示
        if (type != Type::NONE&&!SquareArea.expired())
        {
            static DirectX::XMFLOAT4 colors[static_cast<int>(Square::Type::MAX)] = {
                {1.0f,1.0f,1.0f,0.5f},
                {1.0f,.0f,.0f,0.5f},
            };
            model = this->SquareArea.lock();
            const_cast<ModelResource::Material*>(&model->GetResource()->GetMaterials().at(0))->color =
                colors[static_cast<int>(type)];
            model->UpdateTransform(transform);
            shader->Draw(dc, model.get());
        }
    }
}

const bool Square::Raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    if (!SquareArea.expired())
    {
        std::shared_ptr<Model> model = this->SquareArea.lock();
        DirectX::XMMATRIX Transform =
            DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
            DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rotate)) *
            DirectX::XMMatrixTranslation(worldPos.x, worldPos.y, worldPos.z);
        DirectX::XMFLOAT4X4 transform;
        DirectX::XMStoreFloat4x4(&transform, Transform);
        model->UpdateTransform(transform);
        return Collision::IntersectRayVsModel(start,end,model.get(),hit);
    }

    return false;
}
