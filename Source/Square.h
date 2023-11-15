#pragma once

#include <DirectXMath.h>
#include <memory>
#include <d3d11.h>

#include "Collision.h"

class CardBase;
class Shader;

class Square
{
public://class
	enum class Type : unsigned short
	{
		NONE,
		AttackArea,
		MAX,
	};
public://function
	Square(const DirectX::XMFLOAT3& pos);
	virtual ~Square();

	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	const bool Raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	//Getter&Setter*************************************************************************
#if 1
	const DirectX::XMFLOAT3& GetWorldPos()const noexcept { return worldPos; }
	const Type& GetType()const noexcept { return type; }
	CardBase* GetCard()const noexcept { return card.get(); }
	std::shared_ptr<CardBase> GetUniqueCard()noexcept { return std::move(card); }
	//std::shared_ptr<CardBase> GetUniqueCard()noexcept { std::shared_ptr<CardBase> temp = card; card.reset(); return temp; }
#endif // 1
	//Getter&Setter*************************************************************************
private:
private://members
	Type type = Type::NONE;
	std::shared_ptr<CardBase> card;
	std::weak_ptr<Model> SquareBorder;
	std::weak_ptr<Model> SquareArea;
	DirectX::XMFLOAT3 worldPos;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT4 rotate;
};