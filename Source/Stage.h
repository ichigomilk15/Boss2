#pragma once


#include <memory>
#include <d3d11.h>
#include <DirectXMath.h>
#include <utility>

#include "Collision.h"
#include "Common.h"
#include "Square.h"

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
	std::shared_ptr<Square> GetSquare(unsigned int x, unsigned int y)const noexcept { return squares[y][x]; }
	void ClearStage()noexcept;
	void CreateStage();
	const DirectX::XMFLOAT3 GetWorldPos(const DirectX::XMINT2& pos) const;

	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc,Shader* shader);
	void DrawIMGUI();

	std::vector<Square*> GetSquares(const int& initX, const int& initY, const int& cost);
	std::vector<Square*> GetSquaresEdgeAdjacent(const int& initX, const int& initY, const int& cost);
	std::vector<Square*> GetSquaresByDirection(const int& initX, const int& initY, const int& cost, const int& direction);
	int GetTargetPosCost(const DirectX::XMINT2& posInit, const DirectX::XMINT2& posTarget);
	const bool IsAdjacent(const DirectX::XMINT2& posInit, const DirectX::XMINT2& posTarget) const;

	const bool Raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
	const bool IsInArea(int x, int y)const noexcept;

	void ResetAllSquare();
	void ResetSquaresAccessible();

private:
	void SearchSquare(const int x, const int y, const int cost, std::vector<DirectX::XMINT2>& squaresChecked);

private://static members
private://members
	std::shared_ptr<Square> squares[Common::SQUARE_NUM_Y][Common::SQUARE_NUM_X];
	std::unique_ptr<Model> model;
	std::shared_ptr<Model> squareBorder;
	std::shared_ptr<Model> squareArea;

	DirectX::XMFLOAT3 position{};
	DirectX::XMFLOAT3 scale{};
	DirectX::XMFLOAT4 rotate{};

	//bool isSquareDirty = false; //àÍÇ¬ÇÃÉ}ÉXÇ≈Ç‡ïœä∑Ç™Ç†Ç¡ÇΩèÍçátrueÇ…Ç∑ÇÈ
};