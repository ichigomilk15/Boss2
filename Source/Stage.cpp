
#include "Stage.h"
#include "Graphics/Model.h"
#include "Graphics/Shader.h"
#ifdef _DEBUG
#include "Graphics/ImGuiRenderer.h"
#endif // _DEBUG

#include "Square.h"
#include "Graphics\DebugRenderer.h"
#include "Graphics\Graphics.h"
#include "EnemyManager.h"
#include "PlayerManager.h"
#include "Card.h"
#include <Input\Input.h>

Stage::Stage() :
	//squares(),
	model(std::make_unique<Model>("./Data/Model/Stage/BackGround.mdl")),
	squareBorder(std::make_shared<Model>("./Data/Model/Stage/SquareBorder.mdl")),
	squareArea(std::make_shared<Model>("./Data/Model/Stage/SquareArea.mdl")),
	cardModel()
{
	cardModel[Card::Type::SPECIAL] = std::make_unique<Model>("./Data/Model/Stage/card_buff.mdl");
	cardModel[Card::Type::DEBUFF] = std::make_unique<Model>("./Data/Model/Stage/card_debuff.mdl");
	this->scale = { 3.0f,0.01f,3.0f };
	this->position = { 0.0f, -1.0f, 0.0f };
	random = std::uniform_int_distribution<unsigned int>(0u,Common::SQUARE_NUM_X-1);
}

void Stage::ClearStage() noexcept
{
	for (auto& y : squares)
	{
		for (auto& x : y)
		{
			x.reset();
		}
	}
}

void Stage::CreateStage()
{
	using namespace Common;
	ClearStage();
	for (unsigned int y = 0; y < SQUARE_NUM_Y; y++)
	{
		for (unsigned int x = 0; x < SQUARE_NUM_X; x++)
		{
			squares[y][x] = std::make_shared<Square>(DirectX::XMINT2(x, y));
		}
	}
}

const DirectX::XMFLOAT3 Stage::GetWorldPos(const DirectX::XMINT2& pos) const
{
	using namespace Common;
	return DirectX::XMFLOAT3{ lefttop.x + (SquareWidth * pos.x), lefttop.y, lefttop.z - (SquareHeight * pos.y) };
}

void Stage::Update(float elapsedTime)
{
	DirectX::XMMATRIX Transform =
		DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
		DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rotate)) *
		DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMStoreFloat4x4(&transform, Transform);
	model->UpdateTransform(transform);

	for (auto& y : squares)
	{
		for (auto& x : y)
		{
			x->Update(elapsedTime);
		}
	}
}

void Stage::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (auto& y : squares)
	{
		for (auto& x : y)
		{
			x->Render(dc, shader);
		}
	}

	shader->Draw(dc, model.get());
}

void Stage::DrawIMGUI()
{
#ifdef _DEBUG
	if (ImGui::Begin("Stage", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_None))
	{
		ImGui::InputFloat3("position", &position.x);
		ImGui::InputFloat3("scale", &scale.x);
		ImGui::InputFloat4("rotate", &rotate.x);
		int level = stageLevel;
		if (ImGui::InputInt("StageLevel", &level))
		{
			stageLevel = level;
		}


		for (auto& sq : squares)
		{
			for (auto& x : sq)
			{
				auto chara = x->GetCharacter();
				if( chara != nullptr)
				{
					auto sqPos = x->GetPos();
					ImGui::InputInt2("sq pos", &sqPos.x);
				}
			}
		}
		if (ImGui::Button("refleshCard"))
		{
			ReFleshCard();
		}

	}

	ImGui::End();
#endif // _DEBUG
}


void Stage::SearchSquare(const int x, const int y, const int cost, std::vector<DirectX::XMINT2>& squaresChecked, const std::vector<Square::Type>& typesExclusion)
{
	if (!IsInArea(x, y)) return;
	if (cost < 0) return;
	//if (!GetSquare(x, y)->GetIsaccessible()) return;

	bool isChecked = false;
	for (auto& sq : squaresChecked)
	{
		if (x == sq.x && y == sq.y)
			isChecked = true;
	}

	if (!isChecked && !GetSquare(x, y)->CheckSameType(typesExclusion))
		squaresChecked.emplace_back(DirectX::XMINT2(x, y));

	SearchSquare(x, y - 1, cost - 1, squaresChecked, typesExclusion);      // è„

	SearchSquare(x, y + 1, cost - 1, squaresChecked, typesExclusion);      // â∫

	SearchSquare(x - 1, y, cost - 1, squaresChecked, typesExclusion);      // ç∂

	SearchSquare(x + 1, y, cost - 1, squaresChecked, typesExclusion);      // âE
}

std::vector<Square*> Stage::GetSquares(const int& initX, const int& initY, const int& cost, const std::vector<Square::Type>& typesExclusion)
{
	std::vector<DirectX::XMINT2> squaresValid;

	SearchSquare(initX, initY, cost, squaresValid, typesExclusion);
	if (!squaresValid.empty())
	{
		if (squaresValid.at(0).x == initX && squaresValid.at(0).y == initY)
			squaresValid.erase(squaresValid.begin());
	}

	std::vector<Square*> foundSquares;
	for (auto& square : squaresValid)
	{
		auto sq = GetSquare(square.x, square.y);
		foundSquares.emplace_back(sq.get());
	}

	return foundSquares;
}

std::vector<Square*> Stage::GetSquaresEdgeAdjacent(const int& initX, const int& initY, const int& cost, const std::vector<Square::Type>& typesExclusion)
{
	std::vector<Square*> foundSq = GetSquares(initX, initY, cost, typesExclusion);
	std::vector<Square*> removeSq;
	for (auto& sq : foundSq)
	{
		DirectX::XMINT2 sqPos = sq->GetPos();
		if (sqPos.x != initX && sqPos.y != initY)
		{
			removeSq.emplace_back(sq);
		}
	}
	for (auto& sq : removeSq)
	{
		std::vector<Square*>::iterator it = std::find(foundSq.begin(), foundSq.end(), sq);
		if (it != foundSq.end())
		{
			foundSq.erase(it);
		}
	}
	removeSq.clear();
	return foundSq;
}

std::vector<Square*> Stage::GetSquaresByDirection(const int& initX, const int& initY, const int& cost, const int& direction)
{
	std::vector<Square*> foundSq = GetSquares(initX, initY, cost);
	std::vector<Square*> removeSq;
	for (auto& sq : foundSq)
	{
		DirectX::XMINT2 sqPos = sq->GetPos();
		switch (direction)
		{
		case CommonClass::DirectionFace::Front:
			if (sqPos.x != initX || sqPos.y > initY)
			{
				removeSq.emplace_back(sq);
			}
			break;
		case CommonClass::DirectionFace::Back:
			if (sqPos.x != initX || sqPos.y < initY)
			{
				removeSq.emplace_back(sq);
			}
			break;
		case CommonClass::DirectionFace::Right:
			if (sqPos.x < initX || sqPos.y != initY)
			{
				removeSq.emplace_back(sq);
			}
			break;
		case CommonClass::DirectionFace::Left:
			if (sqPos.x > initX || sqPos.y != initY)
			{
				removeSq.emplace_back(sq);
			}
			break;
		default:
			removeSq.emplace_back(sq);
			break;
		}
		if (sqPos.x != initX && sqPos.y != initY)
		{
			removeSq.emplace_back(sq);
		}
	}
	for (auto& sq : removeSq)
	{
		std::vector<Square*>::iterator it = std::find(foundSq.begin(), foundSq.end(), sq);
		if (it != foundSq.end())
		{
			foundSq.erase(it);
		}
	}
	removeSq.clear();
	return foundSq;
}

const bool Stage::IsAdjacent(const DirectX::XMINT2& posInit, const DirectX::XMINT2& posTarget) const
{
	if (!IsInArea(posInit.x, posInit.y)) return false;
	if (!IsInArea(posTarget.x, posTarget.y)) return false;

	if (posTarget.x != posInit.x && posTarget.y != posInit.y) return false;
	if (abs(posTarget.x - posInit.x) > 1) return false;
	if (abs(posTarget.y - posInit.y) > 1) return false;

	return true;
}

const int Stage::IsAdjacent(const Character* chara1, const Character* chara2) const
{
	for (int y = chara1->GetPosition().y; y < chara1->GetPosition().y + chara1->GetSize().y; y++)
	{
		for (int x = chara1->GetPosition().x; x < chara1->GetPosition().x + chara1->GetSize().x; x++)
		{
			for (int y2 = chara2->GetPosition().y; y2 < chara2->GetPosition().y + chara2->GetSize().y; y2++)
			{
				for (int x2 = chara2->GetPosition().x; x2 < chara2->GetPosition().x + chara2->GetSize().x; x2++)
				{
					if (Stage::Instance()->IsAdjacent(DirectX::XMINT2{ x, y }, DirectX::XMINT2{x2, y2}))
					{
						int dir = CommonClass::GetDirectionTarget({ x, y }, { x2, y2 });
						return dir;
					}
				}
			}
		}
	}
	return -1;
}

int Stage::GetTargetPosCost(const DirectX::XMINT2& posInit, const DirectX::XMINT2& posTarget)
{
	int cost = 1;

	/*struct CheckSquare
	{
		int x;
		int y;
		int cost;
	};
	std::vector<CheckSquare> checkSquares;
	checkSquares.reserve(Common::SquareHeight * Common::SquareWidth);*/
	std::vector<DirectX::XMINT2> checkSquares;

	for (; cost < Common::SquareHeight * Common::SquareWidth; ++cost)
	{
		SearchSquare(posInit.x, posInit.y, cost, checkSquares);
		for (auto& sq : checkSquares)
		{
			if (sq.x == posTarget.x && sq.y == posTarget.y)
				return cost;
		}
	}

	return -1;
	/*for (; cost < Common::SquareHeight * Common::SquareWidth; ++cost)
	{
		bool isFound = false;
		if (getSquare.empty())
		{
			SearchSquare(posInit.x, posInit.y, 1, getSquare);
		}
		else
		{
			std::vector<DirectX::XMINT2> getSquare2;
			for (int i = 0; i < getSquare.size(); ++i)
			{
				SearchSquare(pos)
			}
		}

	}*/

	return 0;
}

void Stage::ResetSquaresAccessible()
{
	for (auto& enemy : EnemyManager::Instance().GetList())
	{
		for (auto& sq : enemy->GetSquaresPosition())
		{
			squares[sq.y][sq.x]->SetIsaccessible(false);
			squares[sq.y][sq.x]->SetCharacter(enemy);
		}
	}
	for (auto& sq : PlayerManager::Instance().GetFirstPlayer()->GetSquaresPosition())
	{
		squares[sq.y][sq.x]->SetIsaccessible(false);
		squares[sq.y][sq.x]->SetCharacter(PlayerManager::Instance().GetFirstPlayer());
	}
}

void Stage::ReFleshCard()
{
	std::vector<std::shared_ptr<Card>> cards;
	for (auto& y : squares)
	{
		for (auto& x : y)
		{
			auto card = x->GetSharedCard();
			if (card == nullptr||card->GetType()!=Card::Type::SPECIAL)continue;
			cards.push_back(card);
			x->ResetCard();
		}
	}

	if (std::size(cards) <= 1)
	{
		for (size_t i = 0; i <ADD_CARD_NUM; i++)
		{
			cards.emplace_back(std::make_shared<Card>(DirectX::XMFLOAT2{ .0f,.0f }, CardManager::CARD_SIZE, Card::Type::SPECIAL));
		}
	}

	for (size_t i = 0; i < cards.size();)
	{
		auto x = random(CommonClass::random);
		auto y = random(CommonClass::random);
		auto&& square = GetSquare(x,y);
		if (square->GetCard() != nullptr)continue;
		if (square->GetCharacter() != nullptr)continue;
		square->SetCard(cards[i]);
		++i;
	}
}

void Stage::ResetAllSquare()
{
	for (auto& y : squares)
	{
		for (auto& x : y)
		{
			x->ResetSquare();
		}
	}
	ResetSquaresAccessible();
}

const bool Stage::Raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	for (auto& y : squares)
	{
		for (auto& x : y)
		{
			if (x->Raycast(start, end, hit))
				return true;
		}
	}
	return false;
}

const bool Stage::IsInArea(int x, int y) const noexcept
{
	using namespace Common;
	return (x >= 0 &&
		y >= 0 &&
		x < Common::SQUARE_NUM_X&&
		y < SQUARE_NUM_Y);
}

void Stage::SetSquareTypeMove(const DirectX::XMINT2& pos, const int& cost, const std::vector<Square::Type>& typesExclusion)
{
	std::vector<Square*> squares = GetSquares(pos.x, pos.y, cost, typesExclusion);

	for (auto& square : squares)
	{
		square->SetType(Square::Type::MoveArea);
	}
}

std::vector<std::shared_ptr<Square>> Stage::GetSquareType(Square::Type type)
{
	std::vector<std::shared_ptr<Square>> moveSquares;
	for (auto& y : squares)
	{
		for (auto& x : y)
		{
			if (x->GetType() == type)
				moveSquares.emplace_back(x);
		}
	}
	return moveSquares;
}
