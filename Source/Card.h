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
    CardBase(DirectX::XMFLOAT2 pos,DirectX::XMFLOAT2 size);
    ~CardBase();

    virtual void Update(float elapsedTime) = 0;
    virtual void Render(ID3D11DeviceContext* dc,Shader* shader);

    const bool HitMouse();
    const bool HitCheck(DirectX::XMFLOAT2 screenPos);

    /// <summary>
    /// カード仕様時に実行する関数
    /// </summary>
    /// <returns>実行に成功したか</returns>
    virtual const bool ActiveAction(void* input) = 0;

    //Getter&Setter**********************************************************
#if 1
    const Type GetType()const noexcept { return type; }
    void SetType(const Type type)noexcept { this->type = type; }
#endif // 1
    //Getter&Setter**********************************************************
protected:
    Type type = Type::NONE;
    DirectX::XMFLOAT2 pos;
    DirectX::XMFLOAT2 size;
    std::unique_ptr<Sprite> sprite;
};

//***************************************************************************************************
// CardAttack Class
//***************************************************************************************************

class CardAttack : public CardBase
{
public:
    CardAttack(DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size) :CardBase(pos, size) { type = Type::ATTACK; };
    ~CardAttack() {};

    virtual void Update(float elapsedTime)override {};
    virtual const bool ActiveAction(void* input)override { return false; };


private:

};