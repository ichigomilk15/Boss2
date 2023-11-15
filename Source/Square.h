#pragma once

#include <DirectXMath.h>
#include <memory>
#include <d3d11.h>

#include "Collision.h"
#include <optional>

class CardBase;
class Shader;

class Square
{
public://class
	enum class Type : unsigned short
	{
		NONE,
		MAX,
	};
public://function
	Square(const DirectX::XMFLOAT3& pos, CardBase* card = nullptr,Type type=Type::NONE);
	virtual ~Square();

	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	const bool Raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
	
	//Getter&Setter*************************************************************************
#if 1
	const DirectX::XMFLOAT3& GetWorldPos()const noexcept { return worldPos; }
	const Type& GetType()const noexcept { return type; }
	std::optional<CardBase*> GetCard()const noexcept { return card.value().get(); }

	void ChangeSomething() { scale = { 7.0f,0.01f,7.0f }; }
#endif // 1
	//Getter&Setter*************************************************************************
private:
	Type type = Type::NONE;
	std::optional<std::unique_ptr<CardBase>> card;
	std::unique_ptr<Model> model;
	DirectX::XMFLOAT3 worldPos;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT4 rotate;
};