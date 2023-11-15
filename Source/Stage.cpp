
#include "Stage.h"
#include "Graphics/Model.h"
#include "Graphics/Shader.h"
#ifdef _DEBUG
#include "Graphics/ImGuiRenderer.h"
#endif // _DEBUG

#include "Square.h"
#include "Graphics\DebugRenderer.h"
#include "Graphics\Graphics.h"

Stage::Stage():
    //squares(),
    model(std::make_unique<Model>("./Data/Model/Stage/BackGround.mdl")),
    squareBorder(std::make_shared<Model>("./Data/Model/Stage/SquareBorder.mdl")),
    squareArea(std::make_shared<Model>("./Data/Model/Stage/SquareArea.mdl"))
{
    this->scale = { 3.0f,0.01f,3.0f };
    this->position = { 0.0f, -1.0f, 0.0f };
}

void Stage::ClearStage() noexcept
{
    for (auto& y : squares)
    {
        for (auto& x : y)
        {
            x.reset();
        }
    }
}

void Stage::CreateStage()
{
    using namespace Common;
    ClearStage();
    const DirectX::XMFLOAT3 lefttop = {-SquareWidth*(SQUARE_NUM_X/2.0f-0.5f),0.0f,SquareHeight*(SQUARE_NUM_Y/2.0f-0.5f)};
    for (unsigned int y = 0; y < SQUARE_NUM_Y; y++)
    {
        for (unsigned int x = 0; x < SQUARE_NUM_X; x++)
        {
            squares[y][x] = std::make_shared<Square>(
                DirectX::XMFLOAT3{ lefttop.x + (SquareWidth * x),lefttop.y,lefttop.z - (SquareHeight * y) });
        }
    }
}

void Stage::Update(float elapsedTime)
{
    DirectX::XMMATRIX Transform =
        DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
        DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rotate)) *
        DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    DirectX::XMFLOAT4X4 transform;
    DirectX::XMStoreFloat4x4(&transform, Transform);
    model->UpdateTransform(transform);

    for (auto& y : squares)
    {
        for (auto& x : y)
        {
            x->Update(elapsedTime);
        }
    }
}

void Stage::Render(ID3D11DeviceContext* dc,Shader* shader)
{
    for (auto& y : squares)
    {
        for (auto& x : y)
        {
            x->Render(dc, shader);
        }
    }

    shader->Draw(dc, model.get());
}

void Stage::DrawIMGUI()
{
#ifdef _DEBUG
    if (ImGui::Begin("Stage", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_None))
    {
        ImGui::InputFloat3("position", &position.x);
        ImGui::InputFloat3("scale", &scale.x);
        ImGui::InputFloat4("rotate", &rotate.x);
    }
    ImGui::End();
#endif // _DEBUG
}

const bool Stage::Raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    for (auto& y : squares)
    {
        for (auto& x : y)
        {
            if (x->Raycast(start, end, hit))
                return true;
        }
    }
    return false;
}

const bool Stage::IsInArea(int x, int y) const noexcept
{
    using namespace Common;
    return (x>=0&&
        y>=0&&
        x<Common::SQUARE_NUM_X&&
        y<SQUARE_NUM_Y);
}

std::pair<unsigned int, unsigned int> Stage::Search(int x, int y, int cost, Type type)
{
    return std::pair<unsigned int, unsigned int>();
}
