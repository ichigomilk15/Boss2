#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "Graphics/RenderContext.h"
#include "Graphics/Model.h"

class Shader
{
public:
	Shader() {}
	virtual ~Shader() {}

	// ï`âÊäJén
	virtual void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) = 0;

	// ï`âÊ
	//virtual void Draw(ID3D11DeviceContext* dc, const Model* model) = 0;
	virtual void Draw(ID3D11DeviceContext* dc, const Model* model, const RenderContext& rc = {}) = 0;

	// ï`âÊèIóπ
	virtual void End(ID3D11DeviceContext* context) = 0;
};
