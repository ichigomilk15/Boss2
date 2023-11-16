#pragma once

#include <DirectXMath.h>
#include <memory>
#include <d3d11.h>

#include "Collision.h"
#include <optional>
#include <map>

class CardBase;
class Shader;

class Square
{
public://class
	enum class Type : unsigned short
	{
		NONE,
		AttackArea,
		MoveArea,
		MoveAreaChosen,
		MAX,
	};

	struct TypeDetail
	{
		const char* semantic;
		const DirectX::XMFLOAT4 color;
	};

	std::map<Type, TypeDetail> typeMaps;

#if 0
	//template<Type> struct Map;
	//template<> struct Map<Type::NONE>
	//{
	//	static constexpr char* semantic = "None";
	//	static constexpr DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,0.5f };
	//};
	//template<> struct Map<Type::AttackArea>
	//{
	//	static constexpr char* semantic = "AttackArea";
	//	static constexpr DirectX::XMFLOAT4 color = { 1.0f,.0f,.0f,0.5f };
	//};
	//template<> struct Map<Type::MoveArea>
	//{
	//	static constexpr char* semantic = "MoveArea";
	//	static constexpr DirectX::XMFLOAT4 color = { .2f,.2f,1.0f,0.5f };
	//};

	//template<Type T>Map<T> GetMap()
	//{
	//	switch (type)
	//	{
	//	case Type::NONE:
	//		return Map<Type::NONE>();
	//		break;
	//	case Type::AttackArea:
	//		return Map<Type::AttackArea>();
	//		break;
	//	case Type::MoveArea:
	//		return Map<Type::MoveArea>();
	//		break;
	//	default:
	//		return Map<Type::NONE>();
	//		break;
	//	}
	//}
#endif
public://function
	Square(const DirectX::XMFLOAT3& pos);
	virtual ~Square();

	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	const bool Raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	void ResetSquare();

	//Getter&Setter*************************************************************************
#if 1
	const DirectX::XMFLOAT3& GetWorldPos()const noexcept { return worldPos; }
	void SetType(Type type);
	const Type& GetType()const noexcept { return type; }
	CardBase* GetCard()const noexcept { return card.get(); }
	std::shared_ptr<CardBase> GetUniqueCard()noexcept { return std::move(card); }
	//std::shared_ptr<CardBase> GetUniqueCard()noexcept { std::shared_ptr<CardBase> temp = card; card.reset(); return temp; }
#endif // 1
	//Getter&Setter*************************************************************************
private:
	void UpdateDirty(); 
private://members
	Type type = Type::NONE;
	std::shared_ptr<CardBase> card;
	std::weak_ptr<Model> SquareBorder;
	std::weak_ptr<Model> SquareArea;
	DirectX::XMFLOAT3 worldPos;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT4 rotate;
	DirectX::XMFLOAT4 areaColor;

	bool typeChanged = false; //É^ÉCÉvÇÃïœä∑Ç™Ç†Ç¡ÇΩèÍçátrueÇ…Ç∑ÇÈ
};