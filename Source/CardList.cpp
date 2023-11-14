
#include <random>
#include "CardList.h"

void CardList::Update(float elapsedTime)
{
	for (auto& card : Cards)
	{
		card->Update(elapsedTime);
	}

	Erase();
}

std::shared_ptr<CardBase> CardList::HitCheck(const DirectX::XMFLOAT2& screenPos)const
{
	for (auto& card : Cards)
	{
		if (card->HitCheck(screenPos))return card;
	}
	return nullptr;
}

std::shared_ptr<CardBase> CardList::DrowCard(std::pair<CardBase::Type, unsigned int>* pair, const size_t& pairSize)
{
	std::random_device seed;
	std::default_random_engine engine(seed());
	unsigned int sumPercent = 0;
	for (size_t i = 0; i < pairSize; i++)
	{
		sumPercent += pair->second;
	}
	std::uniform_int_distribution<unsigned int> random(0, sumPercent);
	unsigned int result = random(engine);

	return std::shared_ptr<CardBase>();
}

void CardList::AddCard(std::shared_ptr<CardBase>& card)
{
	Cards.emplace_back(card);
}

void CardList::EraseItem(std::shared_ptr<CardBase>& item)
{
	eraser.emplace_back(item);
}

void CardList::Erase()
{
	for (auto& erase : eraser)
	{
		const auto& it = std::find(Cards.begin(), Cards.end(), erase);
		Cards.erase(it);
	}
	eraser.clear();
}
