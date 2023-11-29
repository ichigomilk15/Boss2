#pragma once

#include <DirectXMath.h>

class HitBox2D
{
public://constractors
	HitBox2D() =default;
	HitBox2D(const DirectX::XMFLOAT2& leftTop,const DirectX::XMFLOAT2& BoxSize);
	HitBox2D(const HitBox2D&) = default;
	~HitBox2D();

	static HitBox2D CreateBoxFromTopLeft(const DirectX::XMFLOAT2& topleft, const DirectX::XMFLOAT2& BoxSize);
	static HitBox2D CreateBoxFromCenter(const DirectX::XMFLOAT2& center, const DirectX::XMFLOAT2& BoxSize);

public:

	const bool Hit(const HitBox2D& dst)const noexcept;
	const bool Hit(const DirectX::XMFLOAT2& pos)const noexcept;

	void AddVelocity(const DirectX::XMFLOAT2& vector);

	//Getter&Setter***************************************************************************
#if 1
	const DirectX::XMFLOAT2& GetLeftTop()const noexcept { return leftTop; }
	void SetLeftTop(const DirectX::XMFLOAT2& pos)noexcept { this->leftTop = pos; };
	const DirectX::XMFLOAT2& GetBoxSize()const noexcept { return BoxSize; }
	void SetBoxSize(const DirectX::XMFLOAT2& size)noexcept { BoxSize = size; }
#endif // 1
	//Getter&Setter***************************************************************************


private:
	DirectX::XMFLOAT2 leftTop;
	DirectX::XMFLOAT2 BoxSize;
};