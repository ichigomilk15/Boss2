#pragma once

#include <memory>
#include <wrl.h>
#include "Graphics/Shader.h"

class LambertMaskShader : public Shader
{
public:
	LambertMaskShader(ID3D11Device* device);
	~LambertMaskShader() override {}

	void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) override;
	void Draw(ID3D11DeviceContext* dc, const Model* model, const RenderContext& rc = {}) override;
	void End(ID3D11DeviceContext* dc) override;

private:
	static const int MaxBones = 128;

	struct CbScene
	{
		DirectX::XMFLOAT4X4	viewProjection;
		DirectX::XMFLOAT4	lightDirection;
	};

	struct CbMesh
	{
		DirectX::XMFLOAT4X4	boneTransforms[MaxBones];
	};

	struct CbSubset
	{
		DirectX::XMFLOAT4	materialColor;
	};

	struct CbMask
	{
		float dissolveThreshold;		//ディゾルブ量
		float edgeThreshold;			//縁の閾値
		DirectX::XMFLOAT2 dummy;
		DirectX::XMFLOAT4 edgeColor;	//縁の色
	};


	Microsoft::WRL::ComPtr<ID3D11Buffer>			sceneConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			meshConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			subsetConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			maskConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerState;
};
