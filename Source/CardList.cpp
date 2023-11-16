
#include "Common.h"
#include <random>
#include "CardList.h"
#include "Graphics/Graphics.h"

void CardList::Update(float elapsedTime)
{
	DirectX::XMFLOAT2 pos = 
		{Graphics::Instance().GetScreenWidth()*0.1f,Graphics::Instance().GetScreenHeight()-cards.front()->GetSize().y};
	for (auto& card : cards)
	{
		card->SetPosition(pos);
		pos.x += card->GetSize().x + 10.0f;
		card->Update(elapsedTime);
	}

	Erase();
}

void CardList::Render(ID3D11DeviceContext* dc)
{
	for (auto& card : cards)
	{
		card->Render(dc);
	}
}

std::shared_ptr<Card> CardList::HitCheck(const DirectX::XMFLOAT2& screenPos)const
{
	for (auto& card : cards)
	{
		if (card->HitCheck(screenPos))return card;
	}
	return nullptr;
}

std::shared_ptr<Card> CardList::DrowCard(std::pair<Card::Type, unsigned int>* pair, const size_t& pairSize)
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
		if (result <= 0)return std::make_unique<Card>(DirectX::XMFLOAT2{ 0,0 }, DirectX::XMFLOAT2{100,200},pair[i].first);
	}

	return std::make_unique<Card>(DirectX::XMFLOAT2{ 0,0 }, DirectX::XMFLOAT2{ 100,100 },pair[pairSize-1].first);
}

void CardList::AddCard(std::shared_ptr<Card>& card)
{
	cards.emplace_back(card);
}

void CardList::EraseItem(std::shared_ptr<Card>& item)
{
	eraser.emplace_back(item);
}

void CardList::Erase()
{
	for (auto& erase : eraser)
	{
		const auto& it = std::find(cards.begin(), cards.end(), erase);
		cards.erase(it);
	}
	eraser.clear();
}
