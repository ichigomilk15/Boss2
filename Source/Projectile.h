#pragma once
#include "Graphics\Shader.h"

// 前方宣言
class ProjectileManager;

class Projectile
{
public:
	//Projectile() {}
	Projectile(ProjectileManager* manager);

	// 破棄
	void Destroy();

	virtual ~Projectile() {}

	virtual void Update(float elapsedTime) = 0;

	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;
	
	virtual void DrawDebugPrimitive();

	//判定取得
	float GetRadius() const { return radius; }

	const DirectX::XMFLOAT3& GetPosition() const { return position; }
	const DirectX::XMFLOAT3& GetDirection() const { return direction; }
	const DirectX::XMFLOAT3& GetScale() const { return scale; }

protected:
	//行列更新処理
	void UpdateTransform();

protected:
	DirectX::XMFLOAT3 position = { 0, 0, 0 };
	DirectX::XMFLOAT3 direction = { 0, 0, 1 };
	DirectX::XMFLOAT3 scale = { 1, 1, 1 };
	DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0,
									  0, 1, 0, 0,
									  0, 0, 1, 0, 
									  0, 0, 0, 1};

	ProjectileManager* manager = nullptr;
	float radius = 0.5f;
};