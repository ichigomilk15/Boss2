#pragma once

#include <memory>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Graphics/Sprite.h"

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
    CardBase(DirectX::XMFLOAT3 pos,DirectX::XMFLOAT2 size);
    ~CardBase();

    virtual void Update(float elapsedTime) = 0;
    virtual void Render(ID3D11DeviceContext* dc,Shader* shader);

    const bool HitMouse();
    const bool HitCheck(DirectX::XMFLOAT2 screenPos);

    /// <summary>
    /// �J�[�h�d�l���Ɏ��s����֐�
    /// </summary>
    /// <returns>���s�ɐ���������</returns>
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
    DirectX::XMFLOAT2 size;
    std::unique_ptr<Sprite> sprite;
};

