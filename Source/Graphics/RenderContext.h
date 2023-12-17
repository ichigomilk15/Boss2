#pragma once

#include <DirectXMath.h>

// �����_�[�R���e�L�X�g
struct RenderContext
{
	//�}�X�N�f�[�^
	struct MaskData
	{
		ID3D11ShaderResourceView*   maskTexture;
		float						dissolveThreshold;
		float						edgeThreshold;		//����臒l
		DirectX::XMFLOAT4			edgeColor;			//���̐F
	};

public:
	DirectX::XMFLOAT4X4		view;
	DirectX::XMFLOAT4X4		projection;
	DirectX::XMFLOAT4		lightDirection;

	//�}�X�N�f�[�^
	MaskData maskData;
};
