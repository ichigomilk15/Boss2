#pragma once
#include <DirectXMath.h>
#include <d3d11.h>

#include <random>
#include <map>

class Square;

namespace Common
{
	//�X�e�[�W�ڍ�
	static int STAGE_MAX = 2;
	
	//�}�X�ڍ�
	static constexpr unsigned int SQUARE_NUM_X = 7u;
	static constexpr unsigned int SQUARE_NUM_Y = 7u;
	static const float SquareWidth = 5.0f;
	static const float SquareHeight = 5.0f;

	//�X�e�[�W�ڍ�
	static const DirectX::XMFLOAT3 lefttop = { -SquareWidth * (SQUARE_NUM_X / 2.0f - 0.5f),0.0f,SquareHeight * (SQUARE_NUM_Y / 2.0f - 0.5f) };

	//�v���C���[�̈ړ����x
	const float moveSpeed = 0.5f; //�b��

	//�v���C���[�̏����l
	static const DirectX::XMINT2 PlayerPosInit = { 3, 3 };

	//�}�X�p�̏��
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
		//�r���[�|�[�g
		D3D11_VIEWPORT viewport;
		UINT numViewports = 1;
		dc->RSGetViewports(&numViewports, &viewport);

		DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
		DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

		//�}�E�X�J�[�\�����W���擾
		DirectX::XMFLOAT3 screenPosition;
		screenPosition.x = static_cast<float>(mouseX);
		screenPosition.y = static_cast<float>(mouseY);

		DirectX::XMVECTOR StartRay2D = DirectX::XMVectorSet(
			static_cast<float>(mouseX),
			static_cast<float>(mouseY),
			viewport.MinDepth, 0);

		DirectX::XMVECTOR StartRayVec = DirectX::XMVector3Unproject(
			StartRay2D,
			viewport.TopLeftX,		//�r���[�|�[�g����X�ʒu
			viewport.TopLeftY,		//�r���[�|�[�g����Y�ʒu
			viewport.Width,			//�r���[�|�[�g��
			viewport.Height,		//�r���[�|�[�g����
			viewport.MinDepth,		//�[�x�l�͈̔͂�\���ŏ��l�i0.0�ł悢�j
			viewport.MaxDepth,		//�[�x�l�͈̔͂�\���ő�l�i1.0�ł悢�j
			Projection,				//�v���W�F�N�V�����s��
			View,					//�r���[�s��
			World					//���[���h�s��i�P�ʍs��ł悢�j
		);
		DirectX::XMFLOAT3 startPosition;
		DirectX::XMStoreFloat3(&startPosition, StartRayVec);
		return startPosition;
	}
	static DirectX::XMFLOAT3 GetWorldEndPosition(ID3D11DeviceContext* dc, const int& screenX, const int& screenY,
		const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
	{
		//�r���[�|�[�g
		D3D11_VIEWPORT viewport;
		UINT numViewports = 1;
		dc->RSGetViewports(&numViewports, &viewport);

		DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
		DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

		//�}�E�X�J�[�\�����W���擾
		DirectX::XMFLOAT3 screenPosition;
		screenPosition.x = static_cast<float>(screenX);
		screenPosition.y = static_cast<float>(screenY);

		DirectX::XMVECTOR EndRay2D = DirectX::XMVectorSet(
			static_cast<float>(screenX),
			static_cast<float>(screenY),
			viewport.MaxDepth, 0);

		DirectX::XMVECTOR EndRayVec = DirectX::XMVector3Unproject(
			EndRay2D,
			viewport.TopLeftX,		//�r���[�|�[�g����X�ʒu
			viewport.TopLeftY,		//�r���[�|�[�g����Y�ʒu
			viewport.Width,			//�r���[�|�[�g��
			viewport.Height,		//�r���[�|�[�g����
			viewport.MinDepth,		//�[�x�l�͈̔͂�\���ŏ��l�i0.0�ł悢�j
			viewport.MaxDepth,		//�[�x�l�͈̔͂�\���ő�l�i1.0�ł悢�j
			Projection,				//�v���W�F�N�V�����s��
			View,					//�r���[�s��
			World					//���[���h�s��i�P�ʍs��ł悢�j
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