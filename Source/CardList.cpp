
#include "Common.h"
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
	unsigned int sumPercent = 0;
	for (size_t i = 0; i < pairSize; i++)
	{
		sumPercent += pair[i].second;
	}
	std::uniform_int_distribution<unsigned int> random(0, sumPercent);
	int result = random(CommonClass::random_engine);
	for (size_t i = 0; i < pairSize; i++)
	{
		result -= pair[i].second;
		if (result <= 0)return std::make_unique<CardAttack>(DirectX::XMFLOAT2{ 0,0 }, DirectX::XMFLOAT2{100,100});//todo : ここでのードクラスを変更
	}

	return std::make_unique<CardAttack>(DirectX::XMFLOAT2{ 0,0 }, DirectX::XMFLOAT2{ 100,100 });//todo :	一応最後のタイプのカードを生成
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
