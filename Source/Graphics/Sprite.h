#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

// スプライト
class Sprite
{
public:
	Sprite();
	Sprite(const char* filename);
	~Sprite() {}

	struct Vertex
	{
		DirectX::XMFLOAT3	positionWorld;
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT2	texcoord;
	};

	// 描画実行
	DirectX::XMFLOAT2 Render(ID3D11DeviceContext *dc,
		float dx, float dy,
		float dw, float dh,
		float sx, float sy,
		float sw, float sh,
		float angle,
		float r, float g, float b, float a) const;
	//描画
	DirectX::XMFLOAT2 Render(ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT2& pos,
		const DirectX::XMFLOAT2& size,
		const DirectX::XMFLOAT2& texturepos,
		const DirectX::XMFLOAT2& texturesize,
		const float angle,
		const DirectX::XMFLOAT4& color
	)const ;

	//画像全体描画
	DirectX::XMFLOAT2 Render(ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT2& pos,
		const DirectX::XMFLOAT2& size,
		const float angle,
		const DirectX::XMFLOAT4& color
	)const ;
		
	// テクスチャ幅取得
	int GetTextureWidth() const { return textureWidth; }
	float GetTextureWidthf()const noexcept { return static_cast<float>(textureWidth); }

	// テクスチャ高さ取得
	int GetTextureHeight() const { return textureHeight; }
	float GetTextureHeightf()const { return static_cast<float>(textureHeight); }

	const DirectX::XMFLOAT2 GetTextureSize()const noexcept
		{ return { static_cast<float>(textureWidth),static_cast<float >(textureHeight) }; }

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer>				vertexBuffer;

	Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView;

	int textureWidth = 0;
	int textureHeight = 0;
};