#pragma once
#include <DirectXMath.h>

class CameraController
{
public:
	CameraController() {};
	~CameraController() {};

	void Update(float elapsedTime);
	//ターゲット位置設定
	void setTarget(const DirectX::XMFLOAT3& target) { this->target = target; }

	void DrawIMGUI();

	const DirectX::XMFLOAT3 getAngle() const { return angle; }
private:
	DirectX::XMFLOAT3 target = { 0, 0, 0 };
	DirectX::XMFLOAT3 angle = { DirectX::XMConvertToRadians(45), 0, 0 };
	float rollSpeed = DirectX::XMConvertToRadians(90);
	float range = 100.0f;

	float maxAngleX = DirectX::XMConvertToRadians(45);
	float minAngleX = DirectX::XMConvertToRadians(-45);
};