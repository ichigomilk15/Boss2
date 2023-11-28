#pragma once

#include <DirectXMath.h>
#include <memory>
#include <d3d11.h>

#include "Collision.h"
#include <optional>
#include <map>

class Card;
class Shader;

class Square
{
public://class
	enum class Type : unsigned short
	{
		NONE,
		AttackArea,
		AttackAreaChosen,
		MoveArea,
		MoveAreaChosen,
		Inaccessible,
		MAX,
	};

	struct TypeDetail
	{
		const char* semantic;
		const DirectX::XMFLOAT4 color;
	};

	std::map<Type, TypeDetail> typeMaps;

public://function
	Square(const DirectX::XMINT2& pos);
	virtual ~Square();

	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	const bool Raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	void ResetSquare();

	const bool CheckSameType(const std::vector<Type>& types) const;

	//Getter&Setter*************************************************************************
#if 1
	void SetIsaccessible(bool isAccessible);
	const bool GetIsaccessible() const { return isAccessible; }
	const DirectX::XMFLOAT3& GetWorldPos()const noexcept { return worldPos; }
	const DirectX::XMINT2& GetPos() const noexcept { return pos; }
	void SetType(Type type);
	const Type& GetType()const noexcept { return type; }
	Card* GetCard()const noexcept { return card.get(); }
	void SetCard(const std::shared_ptr<Card>& card)noexcept { this->card = card; }
	void ResetCard()noexcept { card.reset(); }
	std::shared_ptr<Card> GetSharedCard()noexcept { return card; }
	//std::shared_ptr<Card> GetSharedCard()noexcept { std::shared_ptr<Card> temp = card; card.reset(); return temp; }
#endif // 1
	//Getter&Setter*************************************************************************
private://function
	const DirectX::XMMATRIX GetTransform()const;
	void UpdateDirty();
private://members
	Type type = Type::NONE;
	std::shared_ptr<Card> card;
	std::weak_ptr<Model> SquareBorder;
	std::weak_ptr<Model> SquareArea;
	DirectX::XMINT2	  pos;
	DirectX::XMFLOAT3 worldPos;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT4 rotate;
	DirectX::XMFLOAT4 areaColor;

	bool typeChanged = false; //É^ÉCÉvÇÃïœä∑Ç™Ç†Ç¡ÇΩèÍçátrueÇ…Ç∑ÇÈ
	bool isAccessible = true;
};