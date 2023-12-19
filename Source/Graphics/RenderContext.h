#pragma once

#include <DirectXMath.h>

// レンダーコンテキスト
struct RenderContext
{
	//マスクデータ
	struct MaskData
	{
		ID3D11ShaderResourceView*   maskTexture;
		float						dissolveThreshold;
		float						edgeThreshold;		//縁の閾値
		DirectX::XMFLOAT4			edgeColor;			//縁の色
	};

public:
	DirectX::XMFLOAT4X4		view;
	DirectX::XMFLOAT4X4		projection;
	DirectX::XMFLOAT4		lightDirection;

	//マスクデータ
	MaskData maskData;
};
