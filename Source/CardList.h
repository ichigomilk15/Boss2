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
	virtual ~CardComboDataBase() = default; //dynamic_cast�Ƀo�[�`�������K�v
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
	//�����_���ŃJ�[�h�𐶐�*�莝���ɂ͒ǉ�����Ȃ��̂�AddCard�Ȃǂ��g�p���Ă�������
	std::shared_ptr<Card> DrawCard(const std::pair<Card::Type, unsigned int>* const pair, const size_t pairSize);
	//��D�ɃJ�[�h��ǉ�
	void AddCard(std::shared_ptr<Card>& card);
	//��D�̈�ԍ��ɒǉ�
	const bool AddCardFront(const std::shared_ptr<Card>& card);
	//�R�D�ɂ��[�ǂ�ǉ�
	void AddCardReserved(const std::shared_ptr<Card>& card);
	//�J�[�h���폜
	void EraseItem(std::shared_ptr<Card>& item);
	//�����ɃJ�[�h���폜**for�����񂵂Ă���Ԃ͌Ă΂Ȃ��ł�������
	void QuickEraseItem(std::shared_ptr<Card>& item);
	//�ړ����Ă���J�[�h�����邩�ǂ���
	const bool IsMoving()const noexcept;
	//�J�[�h�u����S�ĂɃJ�[�h���u����Ă��邩
	const bool IsFillSetCards()const noexcept;

	//�J�[�h�u���ꂩ�玟�Ɏg�p����J�[�h���擾����
	const Card::Type GetUseCard()noexcept;
	//�J�[�h�u���ꂩ�玟�Ɏg�p����J�[�h���폜&�擾����
	CardComboDataBase* PopAndGetUseCard()noexcept;
	//�J�[�h�u����ɃJ�[�h���c���Ă��邩�������擾����
	const bool IsSetCardsEmpty()const noexcept;

	//�S�ẴJ�[�h���폜
	void ALLClear();
	//�J�[�h������܂ŕ�[����
	void Replenish();

	//Getter&Setter*********************************************************************************************
#if 1
	//�J�[�h�������Ƃ��ł��邩
	const bool& GetIsMoveable()const noexcept { return isMoveable; }
	//�J�[�h�������Ƃ��ł��邩
	void SetIsMoveable(const bool flag)noexcept { isMoveable = flag; }
	const std::shared_ptr<Card> GetSetCards(int index) const noexcept { index %= SET_CARD_MAX; return SetCards[index]; }
	void ResetPrevType()noexcept { PrevUseCardType = Card::Type::NONE; }
	const unsigned int GetHaveSpecial()const noexcept { return haveSpecial; }
#endif // 1
	//Getter&Setter*********************************************************************************************

private://functions
	void Erase();

	//�}�E�X�����J�[�h�̕ύX
	void ChangeHaveCard(std::shared_ptr<Card>* card);
public://static memberes
	static constexpr unsigned int CARD_MAX = 5u;
	static constexpr unsigned int SPECIAL_CARD_MAX = 1u;
	static constexpr DirectX::XMFLOAT2 CARD_SIZE = DirectX::XMFLOAT2{ 90.0f,140.0f };
	static constexpr DirectX::XMFLOAT2 CARD_SPAWM_POS = DirectX::XMFLOAT2{ .0f,.0f };
	static constexpr float CARD_DISTANCE = 20.0f;
	static constexpr unsigned int SET_CARD_MAX = 3u;

private://members
	const DirectX::XMFLOAT2 HAND_CARDS_START_POS;//��D�̓����蔻�荶��
	const DirectX::XMFLOAT2 SET_CARDS_START_POS;//�Z�b�g�̓����蔻�荶��
	std::list<std::shared_ptr<Card>> cards;//�莝���̃J�[�h
	std::vector<std::shared_ptr<Card>> eraser;//�폜�ҋ@�J�[�h
	std::stack<std::shared_ptr<Card>> reservedCards;//���Ɉ������Ɋ��荞�ރJ�[�h
	std::shared_ptr<Card> SetCards[SET_CARD_MAX];//�Z�b�g�ɒu���Ă���J�[�h
	std::weak_ptr<Card> haveCard;//�}�E�X�����삵�Ă���J�[�h
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