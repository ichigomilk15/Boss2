#pragma once

#include <memory>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Graphics/Sprite.h"

class Model;
class Shader;

class Card
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
public://functions
    Card(DirectX::XMFLOAT2 pos,DirectX::XMFLOAT2 size,const Type type);
    Card(Card&) = default;
    ~Card();

    virtual void Update(float elapsedTime);
    virtual void Render(ID3D11DeviceContext* dc);
    virtual void DrawIMGUI();

    const bool HitMouse();
    const bool HitCheck(DirectX::XMFLOAT2 screenPos);

    void Swap(Card* card);

    /// <summary>
    /// カード使用時に実行する関数
    /// </summary>
    /// <returns>実行に成功したか</returns>
    virtual const bool ActiveAction(void* input) { return false; };

    //Getter&Setter**********************************************************
#if 1
    const bool IsMoving()const noexcept;

    const Type& GetType()const noexcept { return type; }
    void SetType(const Type type)noexcept { this->type = type; }
    const DirectX::XMFLOAT2& GetPosition()const noexcept { return pos; }
    void SetPosition(const DirectX::XMFLOAT2& pos)noexcept { targetPos = pos; }
    void SetRawPosition(const DirectX::XMFLOAT2& pos)noexcept { this->pos = pos; }//即時移動するとき以外使用しないで下さい
    const DirectX::XMFLOAT2& GetSize()const noexcept { return size; }
    const bool GetMoveLock()const noexcept { return moveLock; }
    void SetMoveLock(const bool lock)noexcept { moveLock = lock; }
#endif // 1
    //Getter&Setter**********************************************************
private://static members
    static constexpr float MOVE_SPEED = 800.0f;
protected://members
    Type type = Type::NONE;
    DirectX::XMFLOAT2 targetPos;
    DirectX::XMFLOAT2 pos;
    DirectX::XMFLOAT2 center;
    DirectX::XMFLOAT2 size;
    bool moveLock = false;
    std::unique_ptr<Sprite> sprite;
};