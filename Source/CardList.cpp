#define NOMINMAX
#include "Common.h"
#include <random>
#include "CardList.h"
#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "Collision.h"
#undef NOMINMAX


CardManager::CardManager():
	HAND_CARDS_START_POS(DirectX::XMFLOAT2{ Graphics::Instance().GetScreenWidth() * 0.3f,Graphics::Instance().GetScreenHeight() - CARD_SIZE.y }),
	SET_CARDS_START_POS({Graphics::Instance().GetScreenWidth()*0.1f,Graphics::Instance().GetScreenHeight()*0.2f}),
	sprite()
{
}

void CardManager::Update(float elapsedTime)
{
	Mouse& mouse = Input::Instance().GetMouse();
	const DirectX::XMFLOAT2 ScreenSize = { Graphics::Instance().GetScreenWidth(),Graphics::Instance().GetScreenHeight() };

	Replenish();

	if (cards.empty())return;

	haveSpecial = 0u;

	//手持ちカードの更新	
	DirectX::XMFLOAT2 pos = HAND_CARDS_START_POS;
	for (auto& card : cards)
	{
		if (card->GetType() == Card::Type::SPECIAL)++haveSpecial;
		if (mouse.GetButtonDown() & Mouse::BTN_LEFT && card->HitCheck(mouse.GetPosition()))
		{
			ChangeHaveCard(&card);
		}

		card->SetPosition(pos);
		pos.x += card->GetSize().x + CARD_DISTANCE;
		card->Update(elapsedTime);
		if (card->GetPosition().x < 0.0f)//todo : debug用
			eraser.emplace_back(card);
	}

	//セットカードの更新
	pos = DirectX::XMFLOAT2{ ScreenSize.x * 0.1f,ScreenSize.y * 0.2f };
	for (size_t i = 0; i < SET_CARD_MAX; ++i, pos.y += CARD_SIZE.y + CARD_DISTANCE)
	{
		auto& card = SetCards[i];
		if (card.get() == nullptr)continue;
		if (card->GetType() == Card::Type::SPECIAL)++haveSpecial;

		if (mouse.GetButtonDown() & Mouse::BTN_LEFT && card->HitCheck(mouse.GetPosition()))
			ChangeHaveCard(&card);

		card->SetPosition(pos);
		card->Update(elapsedTime);
	}

	//マウスが持っているカードを更新	
	if (!haveCard.expired())
	{
		//マウスがカードを離したら
		if (mouse.GetButtonUp() & Mouse::BTN_LEFT)
		{
			std::shared_ptr<Card> card = haveCard.lock();
			
			//手札との判定
			DirectX::XMFLOAT2 boxSize = DirectX::XMFLOAT2{ (CARD_SIZE.x + CARD_DISTANCE) * cards.size(),CARD_SIZE.y };
			if (Collision2D::BoxVsPos(HAND_CARDS_START_POS, boxSize, mouse.GetPosition()))
			{
				auto& it = std::find(cards.begin(), cards.end(), card);
				if (it == cards.end())
				{
					QuickEraseItem(card);
					AddCard(card);
				}
			}
			//セットとの判定
			else 
			{
				DirectX::XMFLOAT2 pos = SET_CARDS_START_POS;
				boxSize = CARD_SIZE;
				for (size_t i = 0; i < SET_CARD_MAX; i++)
				{
					if (SetCards[i] == card)continue;
					if (Collision2D::BoxVsPos(pos, boxSize, mouse.GetPosition()))
					{
						if (SetCards[i] == nullptr)
						{
							QuickEraseItem(card);
							SetCards[i] = card;
						}
					}
					pos.y += CARD_SIZE.y + CARD_DISTANCE;
				}
			}
			ChangeHaveCard(nullptr);
		}
		//マウスを追尾
		else
		{
			auto& card = haveCard.lock();
			pos = card->GetPosition();
			pos.x += mouse.GetPositionX() - mouse.GetOldPositionX();
			pos.y += mouse.GetPositionY() - mouse.GetOldPositionY();
			card->SetRawPosition(pos);
		}
	}

	Erase();
}

void CardManager::Render(ID3D11DeviceContext* dc)
{
	DirectX::XMFLOAT2 pos = SET_CARDS_START_POS;
	for (size_t i = 0; i < SET_CARD_MAX; i++)
	{
		sprite.Render(dc,
			pos.x, pos.y,
			CARD_SIZE.x, CARD_SIZE.y,
			.0f, .0f,
			sprite.GetTextureWidthf(), sprite.GetTextureHeightf(),
			.0f,
			.0f, .0f, .0f, 1.0f);
		
		pos.y += CARD_SIZE.y + CARD_DISTANCE;
	}
	for (auto& card : cards)
	{
		card->Render(dc);
	}

	for (auto& card : SetCards)
	{
		if (card.get() == nullptr)continue;
		card->Render(dc);
	}


}

void CardManager::DrawDebugGUI()
{
	if (ImGui::Begin("CardManager",nullptr,ImGuiWindowFlags_::ImGuiWindowFlags_None))
	{
		int size = static_cast<int>(cards.size());
		if (ImGui::InputInt("HandCards", &size, 0)) {};
		size = haveSpecial;
		if (ImGui::InputInt("HaveSpecial", &size, 0)) {};
		size = static_cast<int>(reservedCards.size());
		if (ImGui::InputInt("ReservedCard", &size, 0)) {};
		if (ImGui::CollapsingHeader("HaveCard"))
		{
			if (!haveCard.expired())
			{
				haveCard.lock()->DrawIMGUI();
			}
		}
		if (ImGui::CollapsingHeader("OnMouseCard"))
		{
			if (auto& card = this->HitCheck(Input::Instance().GetMouse().GetPosition()))
			{
				card->DrawIMGUI();
			}
		}
		if (ImGui::Button("DrowCard"))
		{
			Replenish();
		}
		if (ImGui::Button("DrawSpecial"))
		{
			AddCardFront(std::make_shared<Card>(DirectX::XMFLOAT2{ .0f,.0f }, CARD_SIZE, Card::Type::SPECIAL));
		}
		if (ImGui::Button("GetDebuff"))
		{
			AddCardReserved(std::make_shared<Card>(DirectX::XMFLOAT2{ .0f,.0f }, CARD_SIZE, Card::Type::BAD));
		}
	}
	ImGui::End();
}

std::shared_ptr<Card> CardManager::HitCheck(const DirectX::XMFLOAT2& screenPos)const
{
	for (auto& card : cards)
	{
		if (card->HitCheck(screenPos))return card;
	}
	for (auto& card : SetCards)
	{
		if (card == nullptr)continue;
		if (card->HitCheck(screenPos))return card;
	}
	return nullptr;
}

std::shared_ptr<Card> CardManager::DrowCard(const std::pair<Card::Type, unsigned int>* const pair, const size_t pairSize)
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

void CardManager::AddCard(std::shared_ptr<Card>& card)
{
	cards.emplace_back(card);
}

const bool CardManager::AddCardFront(std::shared_ptr<Card>& card)
{
	if (haveSpecial >= SPECIAL_CARD_MAX)
		return false;
	++haveSpecial;
	cards.emplace_front(card);
	return true;
}

void CardManager::AddCardReserved(std::shared_ptr<Card>& card)
{
	reservedCards.push(card);
}

void CardManager::EraseItem(std::shared_ptr<Card>& item)
{
	eraser.emplace_back(item);
}

void CardManager::QuickEraseItem(std::shared_ptr<Card>& item)
{
	cards.remove_if([&](std::shared_ptr<Card> src) {return src == item; });
	for (auto& card : SetCards)
	{
		if (card == item)card.reset();
	}
}

void CardManager::ALLClear()
{
	this->cards.clear();
	this->eraser.clear();
	this->haveCard.reset();
	while (!this->reservedCards.empty())
	{
		this->reservedCards.pop();
	}
	for (auto& card : SetCards)
	{
		card.reset();
	}
	this->haveSpecial = 0u;
}

void CardManager::Replenish()
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
			const std::pair<Card::Type, unsigned int> param[] =
			{
				{Card::Type::ATTACK,100},
				{Card::Type::MOVE,120},
				{Card::Type::DEFENCE,100},
			};
			size_t paramSize = std::size(param);
			std::shared_ptr<Card> drow = DrowCard(param, paramSize);
			AddCard(drow);
		}
	}
}

void CardManager::Erase()
{
	for (auto& erase : eraser)
	{
		cards.remove_if([&](std::shared_ptr<Card> src) {return src == erase; });

		{
			const auto& it = std::find(std::begin(SetCards), std::end(SetCards), erase);
			if (true) {};

		}
	}
	eraser.clear();
}

void CardManager::ChangeHaveCard(std::shared_ptr<Card>* card)
{
	if (!haveCard.expired())
	{
		haveCard.lock()->SetMoveLock(false);
	}

	if (card == nullptr)
	{
		haveCard.reset();
	}
	else
	{
		(*card)->SetMoveLock(true);
		haveCard = *card;
	}
}
