#pragma once

#include "Sprite.h"
#include <DirectXMath.h>

class NumberSprite
{
public:
	static NumberSprite& Instance() { static NumberSprite instance; return instance; };
private:
	NumberSprite();
	~NumberSprite()=default;

public:
	void NumberOut(const char* nums,ID3D11DeviceContext* dc,const DirectX::XMFLOAT2& pos,const DirectX::XMFLOAT2& size,const DirectX::XMFLOAT4& color)const ;

private:
	Sprite numbers;
};