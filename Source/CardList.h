#pragma once

#include <list>
#include <stack>
#include "Card.h"

class CardList final
{
public://constracter
	CardList() =default;
	~CardList() = default;
public://functions

	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc);
	std::shared_ptr<Card> HitCheck(const DirectX::XMFLOAT2& screenPos)const;
	std::shared_ptr<Card> DrowCard(const std::pair<Card::Type,unsigned int>* const pair,const size_t pairSize);
	void AddCard(std::shared_ptr<Card>& card);
	const bool AddCardFront(std::shared_ptr<Card>& card);
	void AddCardReserved(std::shared_ptr<Card>& card);
	void EraseItem(std::shared_ptr<Card>& item);
private://functions
	void Replenish();
	void Erase();

private://static memberes
	static constexpr unsigned int CARD_MAX = 5u;
	static constexpr unsigned int SPECIAL_CARD_MAX = 1u;
	static constexpr DirectX::XMFLOAT2 CARD_SIZE = DirectX::XMFLOAT2{ 90.0f,140.0f };
	static constexpr DirectX::XMFLOAT2 CARD_SPAWM_POS = DirectX::XMFLOAT2{ .0f,.0f };
private://members
	std::list<std::shared_ptr<Card>> cards, eraser;
	std::stack<std::shared_ptr<Card>> reservedCards;
	unsigned int haveSpecial = 0u;
};