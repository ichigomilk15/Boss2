
#include "Square.h"
#include "Graphics/Shader.h"
#include "Card.h"
#include "Stage.h"

Square::Square(const DirectX::XMINT2& pos) :
	pos(pos),
	scale(5.0f * 0.2f, 0.01f, 5.0f * 0.2f),
	rotate(),
	card(nullptr),
	type(Type::NONE),
	SquareBorder(Stage::Instance()->squareBorder),
	SquareArea(Stage::Instance()->squareArea)
{
    this->worldPos = Stage::Instance()->GetWorldPos(pos);

	typeMaps.insert({ Type::NONE, TypeDetail{ "None", {1.0f,1.0f,1.0f,0.5f} } });
	typeMaps.insert({ Type::AttackArea, TypeDetail{ "AttackArea", {.85f,.1f,.1f,0.5f}, } });
    typeMaps.insert({ Type::AttackAreaChosen, TypeDetail{ "AttackAreaChosen", {1.0f,.0f,.0f,0.8f}, } });
	typeMaps.insert({ Type::MoveArea, TypeDetail{ "MoveArea", {.2f,.2f,1.0f,0.5f}, } });
	typeMaps.insert({ Type::MoveAreaChosen, TypeDetail{ "MoveAreaChosen", {1.0f,1.0f,.0f,0.5f}, } });
    typeMaps.insert({ Type::MoveAreaChosen, TypeDetail{ "Inaccessible", {0.2f,0.2f,0.2f,0.3f}, } });
	typeMaps.insert({ Type::MAX, TypeDetail{ "Max", {.0f,.0f,.0f,.0f} } });
}

Square::~Square()
{
}

void Square::Update(float elapsedTime)
{
	this->UpdateDirty();
}

void Square::Render(ID3D11DeviceContext* dc, Shader* shader)
{

    if (!SquareBorder.expired())
    {
        //枠の表示
        std::shared_ptr<Model> model = this->SquareBorder.lock();
        DirectX::XMMATRIX Transform = GetTransform();
        DirectX::XMFLOAT4X4 transform;
        DirectX::XMStoreFloat4x4(&transform, Transform);
        model->UpdateTransform(transform);
        shader->Draw(dc, model.get());

        //何かのアクション範囲なら板を表示
        if (type != Type::NONE&&!SquareArea.expired())
        {
            model = this->SquareArea.lock();
            model->ChangeMaterialColor(0u, this->areaColor);
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
        DirectX::XMMATRIX Transform = GetTransform();
        DirectX::XMFLOAT4X4 transform;
        DirectX::XMStoreFloat4x4(&transform, Transform);
        model->UpdateTransform(transform);
        return Collision::IntersectRayVsModel(start,end,model.get(),hit);
    }

	return false;
}

void Square::SetType(Type type)
{
	this->type = type;
	this->typeChanged = true;
}

void Square::ResetSquare()
{
    
	this->typeChanged = type==Type::NONE?false:true;
	this->type = Type::NONE;
}

void Square::UpdateDirty()
{
	if (typeChanged)
	{
		areaColor = typeMaps.find(type)->second.color;
		typeChanged = false;
	}
}

const DirectX::XMMATRIX Square::GetTransform() const
{
    return DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
        DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rotate)) *
        DirectX::XMMatrixTranslation(worldPos.x, worldPos.y, worldPos.z);
}
