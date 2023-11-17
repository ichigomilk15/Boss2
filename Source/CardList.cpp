#define NOMINMAX
#include "Common.h"
#include <random>
#include "CardList.h"
#include "Graphics/Graphics.h"
#undef NOMINMAX

void CardList::Update(float elapsedTime)
{
	Replenish();

	if (cards.empty())return;

	DirectX::XMFLOAT2 pos = 
		{Graphics::Instance().GetScreenWidth()*0.3f,Graphics::Instance().GetScreenHeight()-cards.front()->GetSize().y};
	for (auto& card : cards)
	{
		card->SetPosition(pos);
		pos.x += card->GetSize().x + 20.0f;
		card->Update(elapsedTime);
		if (card->GetPosition().x < 0.0f)//todo : debug用
			eraser.emplace_back(card);
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

std::shared_ptr<Card> CardList::DrowCard(const std::pair<Card::Type, unsigned int>* const pair, const size_t pairSize)
{
	unsigned int sumPercent = 0;
	for (size_t i = 0; i < pairSize; i++)
	{
		sumPercent += pair[i].second;
	}
	std::uniform_int_distribution<unsigned int> random(0, sumPercent);//ランダム生成
	int result = random(CommonClass::random_engine);
	for (size_t i = 0; i < pairSize; i++)
	{
		result -= pair[i].second;
		if (result <= 0)return std::make_unique<Card>(CARD_SPAWM_POS, CARD_SIZE,pair[i].first);
	}

	return std::make_unique<Card>(CARD_SPAWM_POS, CARD_SIZE,pair[pairSize-1].first);
}

void CardList::AddCard(std::shared_ptr<Card>& card)
{
	cards.emplace_back(card);
}

const bool CardList::AddCardFront(std::shared_ptr<Card>& card)
{
	if (haveSpecial >= SPECIAL_CARD_MAX)
		return false;
	++haveSpecial;
	cards.emplace_front(card);
	return true;
}

void CardList::AddCardReserved(std::shared_ptr<Card>& card)
{
	reservedCards.push(card);
}

void CardList::EraseItem(std::shared_ptr<Card>& item)
{
	eraser.emplace_back(item);
}

void CardList::Replenish()
{
	if (cards.size() < (CARD_MAX + haveSpecial) % std::numeric_limits<unsigned int>::max())
	{
		if (!reservedCards.empty())//引くカードが確定しているならば
		{
			AddCard(reservedCards.top());
			reservedCards.pop();
		}
		else//引く予定のカードがないならば
		{
			//確率設定
			const std::pair<Card::Type, unsigned int> param[] =
			{
				{Card::Type::ATTACK,100},
				{Card::Type::MOVE,120},
				{Card::Type::DEFENCE,100},
			};
			std::shared_ptr<Card> drow = DrowCard(param, std::size(param));
			AddCard(drow);
		}
	}
}

void CardList::Erase()
{
	for (auto& erase : eraser)
	{
		if (erase->GetType() == Card::Type::SPECIAL)--haveSpecial;
		const auto& it = std::find(cards.begin(), cards.end(), erase);
		if (it==cards.end())continue;//存在しなかったら何もしない
		cards.erase(it);
	}
	eraser.clear();
}
