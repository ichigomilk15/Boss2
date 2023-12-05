#pragma once
#include <DirectXMath.h>
#include <d3d11.h>

#include <random>
#include <map>

class Square;

namespace Common
{
	//ステージ詳細
	static int STAGE_MAX = 2;
	
	//マス詳細
	static constexpr unsigned int SQUARE_NUM_X = 7u;
	static constexpr unsigned int SQUARE_NUM_Y = 7u;
	static const float SquareWidth = 5.0f;
	static const float SquareHeight = 5.0f;

	//ステージ詳細
	static const DirectX::XMFLOAT3 lefttop = { -SquareWidth * (SQUARE_NUM_X / 2.0f - 0.5f),0.0f,SquareHeight * (SQUARE_NUM_Y / 2.0f - 0.5f) };

	//プレイヤーの移動速度
	const float moveSpeed = 0.5f; //秒数

	//プレイヤーの初期値
	static const DirectX::XMINT2 PlayerPosInit = { 3, 3 };

	//マス用の情報
	/*static std::vector<int> listObstacle = {
		1, 0
	};*/
}

class CommonClass
{
public:
	static std::mt19937 random;
private:
	static std::default_random_engine random_engine;
	static std::random_device random_seed;
public:
	static DirectX::XMFLOAT3 GetWorldStartPosition(ID3D11DeviceContext* dc, const int& mouseX, const int& mouseY,
		const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
	{
		//ビューポート
		D3D11_VIEWPORT viewport;
		UINT numViewports = 1;
		dc->RSGetViewports(&numViewports, &viewport);

		DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
		DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

		//マウスカーソル座標を取得
		DirectX::XMFLOAT3 screenPosition;
		screenPosition.x = static_cast<float>(mouseX);
		screenPosition.y = static_cast<float>(mouseY);

		DirectX::XMVECTOR StartRay2D = DirectX::XMVectorSet(
			static_cast<float>(mouseX),
			static_cast<float>(mouseY),
			viewport.MinDepth, 0);

		DirectX::XMVECTOR StartRayVec = DirectX::XMVector3Unproject(
			StartRay2D,
			viewport.TopLeftX,		//ビューポート左上X位置
			viewport.TopLeftY,		//ビューポート左上Y位置
			viewport.Width,			//ビューポート幅
			viewport.Height,		//ビューポート高さ
			viewport.MinDepth,		//深度値の範囲を表す最小値（0.0でよい）
			viewport.MaxDepth,		//深度値の範囲を表す最大値（1.0でよい）
			Projection,				//プロジェクション行列
			View,					//ビュー行列
			World					//ワールド行列（単位行列でよい）
		);
		DirectX::XMFLOAT3 startPosition;
		DirectX::XMStoreFloat3(&startPosition, StartRayVec);
		return startPosition;
	}
	static DirectX::XMFLOAT3 GetWorldEndPosition(ID3D11DeviceContext* dc, const int& screenX, const int& screenY,
		const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
	{
		//ビューポート
		D3D11_VIEWPORT viewport;
		UINT numViewports = 1;
		dc->RSGetViewports(&numViewports, &viewport);

		DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
		DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

		//マウスカーソル座標を取得
		DirectX::XMFLOAT3 screenPosition;
		screenPosition.x = static_cast<float>(screenX);
		screenPosition.y = static_cast<float>(screenY);

		DirectX::XMVECTOR EndRay2D = DirectX::XMVectorSet(
			static_cast<float>(screenX),
			static_cast<float>(screenY),
			viewport.MaxDepth, 0);

		DirectX::XMVECTOR EndRayVec = DirectX::XMVector3Unproject(
			EndRay2D,
			viewport.TopLeftX,		//ビューポート左上X位置
			viewport.TopLeftY,		//ビューポート左上Y位置
			viewport.Width,			//ビューポート幅
			viewport.Height,		//ビューポート高さ
			viewport.MinDepth,		//深度値の範囲を表す最小値（0.0でよい）
			viewport.MaxDepth,		//深度値の範囲を表す最大値（1.0でよい）
			Projection,				//プロジェクション行列
			View,					//ビュー行列
			World					//ワールド行列（単位行列でよい）
		);
		DirectX::XMFLOAT3 endPosition;
		DirectX::XMStoreFloat3(&endPosition, EndRayVec);
		return endPosition;
	}
public: 
	enum DirectionFace
	{
		Front,
		FrontRight,
		Right,
		BackRight,
		Back,
		BackLeft,
		Left,
		FrontLeft,
		Max
	};
	static std::map<int, DirectX::XMFLOAT3> directionMaps;

	static const int GetDirectionTarget(const DirectX::XMINT2 initPos, const DirectX::XMINT2& targetPos);
};