#pragma once

#include <list>
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
	std::shared_ptr<Card> DrowCard(std::pair<Card::Type,unsigned int>* pair,const size_t& pairSize);
	void AddCard(std::shared_ptr<Card>& card);
	const bool AddCardFront(std::shared_ptr<Card>& card);
	void EraseItem(std::shared_ptr<Card>& item);
private://functions
	void Erase();

private://static memberes
	static constexpr unsigned int CARD_MAX = 5u;
	static constexpr unsigned int SPECIAL_CARD_MAX = 1u;//–¢ŽÀ‘•
private://members
	std::list<std::shared_ptr<Card>> cards, eraser;
	unsigned int haveSpecial = 0u;
};