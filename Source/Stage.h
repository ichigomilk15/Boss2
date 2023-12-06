#pragma once


#include <memory>
#include <d3d11.h>
#include <DirectXMath.h>
#include <utility>
#include <random>
#include <map>

#include "Collision.h"
#include "Common.h"
#include "Square.h"
#include "Card.h"

class Model;
class Shader;

class Stage final
{
	friend Square;
	enum class Type
	{
		Player,
		Enemy,
		Item,
	};
private://constructors
	Stage();
	~Stage() {};
public://functions
	static Stage* Instance()noexcept { static Stage instance; return &instance; }
	void ClearStage()noexcept;
	void CreateStage();
	const DirectX::XMFLOAT3 GetWorldPos(const DirectX::XMINT2& pos) const;

	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc,Shader* shader);
	void DrawIMGUI();

	std::vector<Square*> GetSquares(const int& initX, const int& initY, const int& cost, const std::vector<Square::Type>& typesExclusion = {Square::Type::MAX});
	std::vector<Square*> GetSquaresEdgeAdjacent(const int& initX, const int& initY, const int& cost, const std::vector<Square::Type>& typesExclusion = { Square::Type::MAX });
	std::vector<Square*> GetSquaresBoxRange(const int& initX, const int& initY, const int& cost, const std::vector<Square::Type>& typesExclusion = { Square::Type::MAX });
	std::vector<Square*> GetSquaresByDirection(const int& initX, const int& initY, const int& cost, const int& direction);
	int GetTargetPosCost(const DirectX::XMINT2& posInit, const DirectX::XMINT2& posTarget);
	const bool IsAdjacent(const DirectX::XMINT2& posInit, const DirectX::XMINT2& posTarget) const;
	const int IsAdjacent(const Character* chara1, const Character* chara2) const;

	const bool Raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
	const bool IsInArea(int x, int y)const noexcept;

	//キャラクター位置によりマスタイプをType::Moveにする
	void SetSquareTypeMove(const DirectX::XMINT2& pos, const int& cost, const std::vector<Square::Type>& typesExclusion = {Square::Type::MAX});
	std::vector<std::shared_ptr<Square>> GetSquareType(Square::Type type);

	//スペシャルカードの再配置
	void ReFleshCard();
	void ResetAllSquare();
	void ResetSquaresAccessible();
	void ResetAllSquareDrawType();
	void ResetAllSquareDrawType(const Square::DrawType drawType);

	//Getter&Setter***************************************************************************
#if 1
	std::shared_ptr<Square> GetSquare(unsigned int x, unsigned int y)const noexcept { return squares[y][x]; }
	const unsigned int& GetStageLevel()const noexcept { return stageLevel; }
	void SetStageLevel(const unsigned int level)noexcept { stageLevel = level; }
	const unsigned int& StageLevelStepUp()noexcept { return ++stageLevel; }
#endif // 1
	//Getter&Setter***************************************************************************

private:
	void SearchSquare(const int x, const int y, const int cost, std::vector<DirectX::XMINT2>& squaresChecked, const std::vector<Square::Type>& typesExclusion = {Square::Type::MAX});

public:
	static constexpr unsigned int STAGE_LEVEL_MAX = 3;
private://static members
	static constexpr int ADD_CARD_NUM = 3;
private://members
	unsigned int stageLevel = 0u;
	std::shared_ptr<Square> squares[Common::SQUARE_NUM_Y][Common::SQUARE_NUM_X];
	std::unique_ptr<Model> model;
	std::map<Card::Type,std::unique_ptr<Model>> cardModel;
	std::shared_ptr<Model> squareBorder;
	std::shared_ptr<Model> squareArea;

	DirectX::XMFLOAT3 position{};
	DirectX::XMFLOAT3 scale{};
	DirectX::XMFLOAT4 rotate{};

	std::uniform_int_distribution<unsigned int> random;
};