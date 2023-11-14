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
	std::shared_ptr<CardBase> HitCheck(const DirectX::XMFLOAT2& screenPos)const;
	std::shared_ptr<CardBase> DrowCard(std::pair<CardBase::Type,unsigned int>* pair,const size_t& pairSize);
	void AddCard(std::shared_ptr<CardBase>& card);
	void EraseItem(std::shared_ptr<CardBase>& item);
private://functions
	void Erase();
private://members
	std::list<std::shared_ptr<CardBase>> Cards, eraser;
};