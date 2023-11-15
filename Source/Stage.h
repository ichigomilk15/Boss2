#pragma once


#include <memory>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Collision.h"
#include "Common.h"
#include "Square.h"

class Model;
class Shader;

class Stage final
{
private://constructors
	Stage();
	~Stage() {};
public://functions
	static Stage* Instance()noexcept { static Stage instance; return &instance; }
	std::shared_ptr<Square> GetSquare(unsigned int x, unsigned int y)const noexcept { return squares[y][x]; }
	void ClearStage()noexcept;
	void CreateStage();

	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc,Shader* shader);
	void DrawIMGUI();

	std::vector<Square*> GetSquares(const int& initX, const int& initY, int cost);

	const bool Raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
	const bool IsInArea(int x, int y)const noexcept;

private:
	void SearchSquare(const int x, const int y, const int cost, std::vector<DirectX::XMINT2>& squaresChecked);

private://static members
private://members
	std::shared_ptr<Square> squares[Common::SQUARE_NUM_Y][Common::SQUARE_NUM_X];
	std::unique_ptr<Model> model;

	DirectX::XMFLOAT3 position{};
	DirectX::XMFLOAT3 scale{};
	DirectX::XMFLOAT4 rotate{};
};