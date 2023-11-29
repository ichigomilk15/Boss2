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
	SET_CARDS_START_POS({Graphics::Instance().GetScreenWidth()*0.1f,Graphics::Instance().GetScreenHeight()*0.2f}),isMoveable(true),
	HandsCardSprite("./Data/Sprite/HandsBackGround.png")
{

	SetCardSprites[0] = std::make_unique<Sprite>("./Data/Sprite/SetCardBack.png");
	SetCardSprites[1] = std::make_unique<Sprite>("./Data/Sprite/SetCardFront.png");

	//�J�[�h�R���{�ݒ�
	{
	const int typeNone = (int)Card::Type::NONE;
	const int typeAttack = (int)Card::Type::ATTACK;
	const int typeDefence = (int)Card::Type::DEFENCE;
	const int typeMove = (int)Card::Type::MOVE;
	const int typeSpecial = (int)Card::Type::SPECIAL;
	const int typeDebuff = (int)Card::Type::DEBUFF;

	//�U���R���{�̓o�^
	{
		CardComboAttack data;
		data.type = Card::Type::ATTACK;
		//�U���P��
		data.Attackcost = 1u;
		data.AttackDamage = 9;
		data.AreaAttackCost = false;
		data.VAreaAttackCost = false;
		data.UseShield = false;
		auto Data = std::make_shared<CardComboAttack>(data);
		CardComboDatas[typeNone][typeAttack] = Data;
		CardComboDatas[typeDebuff][typeAttack] = Data;		

		//	�U�����U��
		data.Attackcost = 1u;
		data.AttackDamage = 12;
		data.AreaAttackCost = true;
		data.VAreaAttackCost = false;
		data.UseShield = false;
		CardComboDatas[typeAttack][typeAttack] = std::make_shared<CardComboAttack>(data);

		//�ړ����U��
		data.Attackcost = 1u;
		data.AttackDamage = 8;
		data.AreaAttackCost = false;
		data.VAreaAttackCost = 1u;
		data.UseShield = false;
		CardComboDatas[typeMove][typeAttack] = std::make_shared<CardComboAttack>(data);
		
		//�h�䁖�U��
		data.Attackcost = 1u;
		data.AttackDamage = 9;
		data.AreaAttackCost = false;
		data.VAreaAttackCost = false;
		data.UseShield = true;
		CardComboDatas[typeDefence][typeAttack] = std::make_shared<CardComboAttack>(data);

		//�X�y�V�������U��
		data.Attackcost = 1u;
		data.AttackDamage = 30;
		data.AreaAttackCost = false;
		data.VAreaAttackCost = false;
		data.UseShield = false;
		CardComboDatas[typeSpecial][typeAttack] = std::make_shared<CardComboAttack>(data);
	}

	//�ړ��R���{�̓o�^
	{
		CardComboMove data;
		data.type = Card::Type::MOVE;

		//�ړ��P��
		data.moveCost = 1;
		data.attackDamege = 0u;
		data.knockbackCost = 0u;
		data.knockbackDamege = 0u;
		data.knockbackTakeDamege = 0u;
		auto Data = std::make_shared<CardComboMove>(data);
		CardComboDatas[typeNone][typeMove] = Data;
		CardComboDatas[typeDebuff][typeMove] = Data;
		
		//�U��*�ړ�
		data.moveCost = 1u;
		data.attackDamege = 3;
		data.knockbackCost = 0;
		data.knockbackDamege = 0;
		data.knockbackTakeDamege = 0;
		CardComboDatas[typeAttack][typeMove] = std::make_shared<CardComboMove>(data);

		//�ړ�*�ړ�
		data.moveCost = 2u;
		data.attackDamege = 0;
		data.knockbackCost = 0;
		data.knockbackDamege = 0;
		data.knockbackTakeDamege = 0;
		CardComboDatas[typeMove][typeMove] = std::make_shared<CardComboMove>(data);

		//�h��*�ړ�
		data.moveCost = 1u;
		data.attackDamege = 0;
		data.knockbackCost = 2;
		data.knockbackDamege = 1;
		data.knockbackTakeDamege = 2;
		CardComboDatas[typeDefence][typeMove] = std::make_shared<CardComboMove>(data);

		//�X�y�V����*�ړ�
		data.moveCost = 5u;
		data.attackDamege = 0;
		data.knockbackCost = 0;
		data.knockbackDamege = 0;
		data.knockbackTakeDamege = 0;
		CardComboDatas[typeSpecial][typeMove] = std::make_shared<CardComboMove>(data);
	}

	//�h��R���{�̓o�^
	{
		CardComboDefence data;
		data.type = Card::Type::DEFENCE;

		//�h��P��
		data.getShield = 4;
		data.GetBlock = 0;
		data.heal = 0;
		data.movecostGetShield = false;
		auto Data = std::make_shared<CardComboDefence>(data);
		CardComboDatas[typeNone][typeDefence] = Data;
		CardComboDatas[typeDebuff][typeDefence] = Data;

		//�U��*�h��
		data.getShield = 0;
		data.GetBlock = -4;
		data.heal = 0;
		data.movecostGetShield = false;
		CardComboDatas[typeAttack][typeDefence] = std::make_shared<CardComboDefence>(data);

		//�ړ�*�h��
		data.getShield = 3;
		data.GetBlock = 0;
		data.heal = 0;
		data.movecostGetShield = true;
		CardComboDatas[typeMove][typeDefence] = std::make_shared<CardComboDefence>(data);

		//�h��*�h��
		data.getShield = 6;
		data.GetBlock = 0;
		data.heal = 5;
		data.movecostGetShield = false;
		CardComboDatas[typeDefence][typeDefence] = std::make_shared<CardComboDefence>(data);

		//�X�y�V����*�h��
		data.getShield = 10;
		data.GetBlock = 0;
		data.heal = 10;
		data.movecostGetShield = false;
		CardComboDatas[typeSpecial][typeDefence] = std::make_shared<CardComboDefence>(data);
	}

	//�f�o�t�R���{�̓o�^
	{
		CardComboDebuff data;
		data.type = Card::Type::DEBUFF;

		//�f�o�t�P��
		data.heal = 0;
		data.takeDamage = 5;
		auto Data = std::make_shared<CardComboDebuff>(data);
		CardComboDatas[typeNone][typeDebuff] = Data;
		CardComboDatas[typeAttack][typeDebuff] = Data;
		CardComboDatas[typeMove][typeDebuff] = Data;
		CardComboDatas[typeDefence][typeDebuff] = Data;

		//�f�o�t*�f�o�t
		data.heal = 5;
		data.takeDamage = 0;
		CardComboDatas[typeDebuff][typeDebuff] = std::make_shared<CardComboDebuff>(data);

		//�o�t*�f�o�t
		data.heal = 10;
		data.takeDamage = 10;
		CardComboDatas[typeSpecial][typeDebuff] = std::make_shared<CardComboDebuff>(data);
	}

	//�R���{�����̓o�^
	{
		CardComboNoUseing data;
		data.type = Card::Type::NONE;
		//�R���{����
		auto Data = std::make_shared<CardComboNoUseing>(data);
		CardComboDatas[typeNone][typeNone] = Data;
		CardComboDatas[typeNone][typeSpecial] = Data;
		CardComboDatas[typeAttack][typeSpecial] = Data;
		CardComboDatas[typeMove][typeSpecial] = Data;
		CardComboDatas[typeDefence][typeSpecial] = Data;
		CardComboDatas[typeDebuff][typeSpecial] = Data;
		CardComboDatas[typeAttack][typeNone] = Data;
		CardComboDatas[typeMove][typeNone] = Data;
		CardComboDatas[typeDefence][typeNone] = Data;
		CardComboDatas[typeDebuff][typeNone] = Data;
		CardComboDatas[typeSpecial][typeNone] = Data;
		CardComboDatas[typeSpecial][typeSpecial] = Data;
	}
	}
}

void CardManager::Update(float elapsedTime)
{
	Mouse& mouse = Input::Instance().GetMouse();
	const DirectX::XMFLOAT2 ScreenSize = { Graphics::Instance().GetScreenWidth(),Graphics::Instance().GetScreenHeight() };

	//�S�ẴX�y�V�����J�[�h�̃J�E���g
	haveSpecial = 0u;

	//��D�J�[�h�̍X�V	
	if (cards.empty())return;
	DirectX::XMFLOAT2 pos = HAND_CARDS_START_POS;
	for (auto& card : cards)
	{
		if (card->GetType() == Card::Type::SPECIAL)++haveSpecial;
		if (isMoveable&& mouse.GetButtonDown() & Mouse::BTN_LEFT && card->HitCheck(mouse.GetPosition()))
		{
			ChangeHaveCard(&card);
		}

		card->SetPosition(pos);
		pos.x += card->GetSize().x + CARD_DISTANCE;
		card->Update(elapsedTime);
		if (card->GetPosition().x < 0.0f)//todo : debug�p
			eraser.emplace_back(card);
	}

	//�Z�b�g�J�[�h�̍X�V
	pos = DirectX::XMFLOAT2{ ScreenSize.x * 0.1f,ScreenSize.y * 0.2f };
	for (size_t i = 0; i < SET_CARD_MAX; ++i, pos.y += CARD_SIZE.y + CARD_DISTANCE)
	{
		auto& card = SetCards[i];
		if (card.get() == nullptr)continue;
		if (card->GetType() == Card::Type::SPECIAL)++haveSpecial;

		if (isMoveable&& mouse.GetButtonDown() & Mouse::BTN_LEFT && card->HitCheck(mouse.GetPosition()))
			ChangeHaveCard(&card);

		card->SetPosition(pos);
		card->Update(elapsedTime);
	}

	//�}�E�X�������Ă���J�[�h���X�V	
	if (isMoveable&&!haveCard.expired())
	{
		//�}�E�X���J�[�h�𗣂�����
		if (mouse.GetButtonUp() & Mouse::BTN_LEFT)
		{
			std::shared_ptr<Card> card = haveCard.lock();
			
			//��D�Ƃ̔���
			DirectX::XMFLOAT2 boxSize = DirectX::XMFLOAT2{
				(CARD_SIZE.x + CARD_DISTANCE) * std::max(cards.size(),static_cast<size_t>(CARD_MAX)),CARD_SIZE.y };
			if (Collision2D::BoxVsPos(HAND_CARDS_START_POS, boxSize, mouse.GetPosition()))
			{
				auto& it = std::find(cards.begin(), cards.end(), card);
				if (it == cards.end())
				{
					QuickEraseItem(card);
					AddCard(card);
				}
			}
			//�Z�b�g�Ƃ̔���
			else 
			{
				boxSize = CARD_SIZE;
				for (size_t i = 0,end = (card->GetType()==Card::Type::SPECIAL?SET_CARD_MAX-1:SET_CARD_MAX); i < end; i++)
				{
					if (SetCards[i] == card)continue;
					DirectX::XMFLOAT2 pos = SET_CARDS_START_POS;
					pos.y += (CARD_SIZE.y + CARD_DISTANCE) * i;
					if (Collision2D::BoxVsPos(pos, boxSize, mouse.GetPosition()))
					{
						if (SetCards[i] == nullptr)
						{
							QuickEraseItem(card);
							SetCards[i] = card;
						}
						else
						{
							SetCards[i]->Swap(card.get());
						}
					}
				}
			}
			ChangeHaveCard(nullptr);
		}
		//�}�E�X��ǔ�
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
	const DirectX::XMFLOAT2 ScreenSize = Graphics::Instance().GetScreenSize();
	
	DirectX::XMFLOAT2 renderpos;
	DirectX::XMFLOAT2 renderSize = { ScreenSize.x,ScreenSize.y * 0.2f };

	//��D�̔w�i
	HandsCardSprite.Render(dc,
		0.0f, ScreenSize.y - renderSize.y,
		renderSize.x,renderSize.y,
		.0f, .0f,
		HandsCardSprite.GetTextureWidthf(), HandsCardSprite.GetTextureHeightf(),
		.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	);

	renderpos = { ScreenSize.x * 0.1f, ScreenSize.y * 0.2f };
	renderpos = { testdatas[1]};
	renderSize = { ScreenSize.x * 0.2f, ScreenSize.y * 0.8f };
	renderSize = { testdatas[0] };
	//�Z�b�g�J�[�h�̕`��
	SetCardSprites[0]->Render(dc,
		renderpos.x,renderpos.y,
		renderSize.x,renderSize.y,
		.0f, .0f,
		SetCardSprites[0]->GetTextureWidthf(), SetCardSprites[0]->GetTextureHeightf(),
		.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	);

	for (auto& card : cards)
	{
		card->Render(dc);
	}

	for (auto& card : SetCards)
	{
		if (card.get() == nullptr)continue;
		card->Render(dc);
	}

	//�Z�b�g�J�[�h�̕`��
	SetCardSprites[1]->Render(dc,
		renderpos.x, renderpos.y,
		renderSize.x, renderSize.y,
		.0f, .0f,
		SetCardSprites[1]->GetTextureWidthf(), SetCardSprites[1]->GetTextureHeightf(),
		.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	);


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
		if (ImGui::CollapsingHeader("HaveCard",ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (!haveCard.expired())
			{
				haveCard.lock()->DrawIMGUI();
			}
		}
		if (ImGui::CollapsingHeader("OnMouseCard",ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (auto& card = this->HitCheck(Input::Instance().GetMouse().GetPosition()))
			{
				card->DrawIMGUI();
			}
		}
		if (ImGui::Button("DrawCard"))
		{
			Replenish();
		}
		if (ImGui::Button("DrawSpecial"))
		{
			AddCardFront(std::make_shared<Card>(DirectX::XMFLOAT2{ .0f,.0f }, CARD_SIZE, Card::Type::SPECIAL));
		}
		if (ImGui::Button("GetDebuff"))
		{
			AddCardReserved(std::make_shared<Card>(DirectX::XMFLOAT2{ .0f,.0f }, CARD_SIZE, Card::Type::DEBUFF));
		}

		if (ImGui::SliderFloat2("renderpos", &testdatas[1].x,.0f,500.0f)) {};
		if (ImGui::SliderFloat2("rendersize", &testdatas[0].x,.0f,1000.0f)) {};
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

std::shared_ptr<Card> CardManager::DrawCard(const std::pair<Card::Type, unsigned int>* const pair, const size_t pairSize)
{
	unsigned int sumPercent = 0;
	for (size_t i = 0; i < pairSize; i++)
	{
		sumPercent += pair[i].second;
	}
	std::uniform_int_distribution<unsigned int> random(0, sumPercent);//�����_������
	int result = random(CommonClass::random);
	for (size_t i = 0; i < pairSize; i++)
	{
		result -= pair[i].second;
		if (result <= 0)return std::make_shared<Card>(CARD_SPAWM_POS, CARD_SIZE,pair[i].first);
	}

	return std::make_shared<Card>(CARD_SPAWM_POS, CARD_SIZE,pair[pairSize-1].first);
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

const bool CardManager::IsMoving() const noexcept
{
	for (auto& card : cards)
	{
		if (card->IsMoving())return true;
	}
	for (auto& card : SetCards)
	{
		if (card == nullptr)continue;
		if (card->IsMoving())return true;
	}
	return false;
}

const bool CardManager::IsFillSetCards() const noexcept
{
	for (auto& card : SetCards)
	{
		if (card == nullptr)return false;
	}
	return true;
}

const Card::Type CardManager::GetUseCard() noexcept
{
	for (auto& card : SetCards)
	{
		if (card == nullptr)continue;
		return card->GetType();
	}
	return Card::Type::NONE;
}

const CardComboDataBase* CardManager::PopAndGetUseCard() noexcept
{

	for (size_t i = 0; i < std::size(SetCards); i++)
	{
		auto card = SetCards[i];
		if (card == nullptr)continue;
		const Card::Type leftType = (i == 0 ? Card::Type::NONE : PrevUseCardType);
		const Card::Type RightType = card->GetType();
		EraseItem(card);
		auto& data = CardComboDatas[static_cast<int>(leftType)][static_cast<int>(RightType)];
		data->type = RightType;
		PrevUseCardType = RightType;
		return data.get();
	}
	return CardComboDatas[static_cast<int>(Card::Type::NONE)][static_cast<int>(Card::Type::NONE)].get();
}

const bool CardManager::IsSetCardsEmpty() const noexcept
{
	for (auto& card : SetCards)
	{
		if (card != nullptr)return false;
	}
	return true;
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
	while(cards.size() < (CARD_MAX + haveSpecial) % std::numeric_limits<unsigned int>::max())
	{
		if (!reservedCards.empty())//�����J�[�h���m�肵�Ă���Ȃ��
		{
			AddCard(reservedCards.top());
			reservedCards.pop();
		}
		else//�����\��̃J�[�h���Ȃ��Ȃ��
		{
			//�m����ݒ�
			const std::pair<Card::Type, unsigned int> param[] =
			{
				{Card::Type::ATTACK,100},
				{Card::Type::MOVE,120},
				{Card::Type::DEFENCE,100},
			};
			size_t paramSize = std::size(param);
			std::shared_ptr<Card> draw = DrawCard(param, paramSize);
			AddCard(draw);
		}
	}
}

void CardManager::Erase()
{
	for (auto& erase : eraser)
	{
		cards.remove_if([&](std::shared_ptr<Card> src) {return src == erase; });


		for (size_t i = 0; i < SET_CARD_MAX; i++)
		{
			if (SetCards[i] == erase)SetCards[i].reset();
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
