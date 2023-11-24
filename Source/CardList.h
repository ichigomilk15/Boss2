#pragma once

#include <list>
#include <stack>
#include "Card.h"

class CardManager final
{
private://constracter
	CardManager();
	~CardManager() = default;
	CardManager(CardManager&) = delete;
	CardManager operator=(CardManager&) = delete;
public://functions
	static CardManager& Instance()noexcept { static CardManager instance; return instance; }

	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc);
	void DrawDebugGUI();
	std::shared_ptr<Card> HitCheck(const DirectX::XMFLOAT2& screenPos)const;
	std::shared_ptr<Card> DrowCard(const std::pair<Card::Type,unsigned int>* const pair,const size_t pairSize);
	void AddCard(std::shared_ptr<Card>& card);
	const bool AddCardFront(std::shared_ptr<Card>& card);
	void AddCardReserved(std::shared_ptr<Card>& card);
	void EraseItem(std::shared_ptr<Card>& item);
	void QuickEraseItem(std::shared_ptr<Card>& item);
	const bool IsMoving()const noexcept;

	void ALLClear();
	void Replenish();

	//Getter&Setter******************************************************************************************************
#if 1
	const bool& GetIsMoveable()const noexcept { return isMoveable; }
	void SetIsMoveable(const bool flag)noexcept { isMoveable = flag; }
#endif // 1
	//Getter&Setter******************************************************************************************************

private://functions
	void Erase();

	void ChangeHaveCard(std::shared_ptr<Card>* card);
public://static memberes
	static constexpr unsigned int CARD_MAX = 5u;
	static constexpr unsigned int SPECIAL_CARD_MAX = 1u;
	static constexpr DirectX::XMFLOAT2 CARD_SIZE = DirectX::XMFLOAT2{ 90.0f,140.0f };
	static constexpr DirectX::XMFLOAT2 CARD_SPAWM_POS = DirectX::XMFLOAT2{ .0f,.0f };
	static constexpr float CARD_DISTANCE = 20.0f;
	static constexpr unsigned int SET_CARD_MAX = 3u;

private://members
	const DirectX::XMFLOAT2 HAND_CARDS_START_POS;//手札の当たり判定左上
	const DirectX::XMFLOAT2 SET_CARDS_START_POS;//セットの当たり判定左上
	std::list<std::shared_ptr<Card>> cards;//手持ちのカード
	std::vector<std::shared_ptr<Card>> eraser;//削除待機カード
	std::stack<std::shared_ptr<Card>> reservedCards;//次に引く時に割り込むカード
	std::shared_ptr<Card> SetCards[SET_CARD_MAX];//セットに置いてあるカード
	std::weak_ptr<Card> haveCard;//マウスが操作しているカード
	unsigned int haveSpecial = 0u;
	bool isMoveable;

	Card::Type PrevUseCardType = Card::Type::NONE;
	Sprite sprite;
};