#pragma once

#include <list>
#include <stack>
#include <map>
#include "Card.h"
#include "Character.h"

//CardCombos
#if 1
struct CardComboDataBase
{
	Card::Type type = Card::Type::NONE;
	std::shared_ptr<Sprite> infomation;
	virtual ~CardComboDataBase() = default; //dynamic_castにバーチャルが必要
};

struct CardComboAttack final : public CardComboDataBase
{
public:
	//CardComboAttack(const CardComboAttack&) = default;
	//CardComboAttack() = default;
	//CardComboAttack& operator=(CardComboAttack&) = default;
	unsigned int Attackcost = 0u;
	int AttackDamage = 0;
	unsigned int AreaAttackCost = 0u;
	unsigned int VAreaAttackCost = 0u;
	bool UseShield = false;
};

struct CardComboDefence final : public CardComboDataBase
{
	int getShield;
	int GetBlock;
	bool movecostGetShield;
	int heal;
};

struct CardComboMove final : public CardComboDataBase
{
	int moveCost;
	int attackDamege;
	int knockbackCost;
	int knockbackDamege;
	int knockbackTakeDamege;
};

struct CardComboDebuff final : public CardComboDataBase
{
	int takeDamage;
	//std::vector<Character*> takeDamagetargets;
	int heal;
	int attackDamage;
};

struct CardComboNoUseing final : public CardComboDataBase
{
	bool isNodata = true;
};
#endif // 1

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
	//ランダムでカードを生成*手持ちには追加されないのでAddCardなどを使用してください
	std::shared_ptr<Card> DrawCard(const std::pair<Card::Type, unsigned int>* const pair, const size_t pairSize);
	//手札にカードを追加
	void AddCard(std::shared_ptr<Card>& card);
	//手札の一番左に追加
	const bool AddCardFront(const std::shared_ptr<Card>& card);
	//山札にかーどを追加
	void AddCardReserved(const std::shared_ptr<Card>& card);
	//カードを削除
	void EraseItem(std::shared_ptr<Card>& item);
	//即座にカードを削除**for文を回している間は呼ばないでください
	void QuickEraseItem(std::shared_ptr<Card>& item);
	//移動しているカードがあるかどうか
	const bool IsMoving()const noexcept;
	//カード置き場全てにカードが置かれているか
	const bool IsFillSetCards()const noexcept;

	//カード置き場から次に使用するカードを取得する
	const Card::Type GetUseCard()noexcept;
	//カード置き場から次に使用するカードを削除&取得する
	CardComboDataBase* PopAndGetUseCard()noexcept;
	//カード置き場にカードが残っているか動かを取得する
	const bool IsSetCardsEmpty()const noexcept;

	//全てのカードを削除
	void ALLClear();
	//カードを上限まで補充する
	void Replenish();

	//Getter&Setter*********************************************************************************************
#if 1
	//カードを持つことができるか
	const bool& GetIsMoveable()const noexcept { return isMoveable; }
	//カードを持つことができるか
	void SetIsMoveable(const bool flag)noexcept { isMoveable = flag; }
	const std::shared_ptr<Card> GetSetCards(int index) const noexcept { index %= SET_CARD_MAX; return SetCards[index]; }
	void ResetPrevType()noexcept { PrevUseCardType = Card::Type::NONE; }
	const unsigned int GetHaveSpecial()const noexcept { return haveSpecial; }
#endif // 1
	//Getter&Setter*********************************************************************************************

private://functions
	void Erase();

	//マウスが持つカードの変更
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
	std::shared_ptr<CardComboDataBase> CardComboDatas[static_cast<int>(Card::Type::MAX)][static_cast<int>(Card::Type::MAX)];
	std::unique_ptr<Sprite> CardNextComboInfos[static_cast<int>(Card::Type::MAX)];
	Sprite HandsCardSprite;
	Sprite cardInfoBack;
	std::unique_ptr<Sprite> SetCardSprites[2];
	DirectX::XMFLOAT2 testdatas[2];
};