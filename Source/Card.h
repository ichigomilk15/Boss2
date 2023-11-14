#pragma once

#include <memory>
#include <d3d11.h>
#include <DirectXMath.h>

class Model;
class Shader;

class CardBase
{
public://class
    enum class Type
    {
        NONE,
        ATTACK,
        DEFENCE,
        MOVE,
        SPECIAL,
        BAD,
        MAX,
    };
public:
    CardBase();
    ~CardBase();

    virtual void Update(float elapsedTime) = 0;
    virtual void Render(ID3D11DeviceContext* dc,Shader* shader);

    /// <summary>
    /// カード仕様時に実行する関数
    /// </summary>
    /// <returns>実行に成功したか</returns>
    virtual const bool ActiveAction() = 0;

    //Getter&Setter**********************************************************
#if 1
    const Type GetType()const noexcept { return type; }
    void SetType(const Type type)noexcept { this->type = type; }
#endif // 1
    //Getter&Setter**********************************************************
private:
    Type type = Type::NONE;
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT3 scale;
    std::unique_ptr<Model> model;
};

