#pragma once
#include <DirectXMath.h>
#include "Common.h"

class CameraController
{
public:
	CameraController() {};
	~CameraController() {};

	//唯一インスタンス取得
	static CameraController& Instance()
	{
		static CameraController cameraController;
		return cameraController;
	}

	void Update(float elapsedTime);
	//ターゲット位置設定
	void setTarget(const DirectX::XMFLOAT3& target) { this->target = target; }

	void DrawIMGUI();

	const DirectX::XMFLOAT3 getAngle() const { return angle; }

	//Camera Shake処理
	void ShakeCamera(const float timer, const int level = 3);

	bool IsCameraShaking() { return cameraShakeDetail.isShaking; }

private:
	void UpdateCameraShake(float elapsedTime);
private:
	DirectX::XMFLOAT3 target = { 0, 0, 0 };
	DirectX::XMFLOAT3 angle = { DirectX::XMConvertToRadians(50), 0, 0 };
	float rollSpeed = DirectX::XMConvertToRadians(90);
	float range = 60.0f;

	float maxAngleX = DirectX::XMConvertToRadians(80);
	float minAngleX = DirectX::XMConvertToRadians(-80);
	
private:
	struct CameraShakeDetail
	{
		bool isShaking = false;
		float shakeTimer = 0.0f;
		float shakeTimerMax = 0.0f;
		int shakeLevel = 0;

		DirectX::XMFLOAT3 angleInit{ 0.0f, 0.0f, 0.0f };

	}cameraShakeDetail;
	std::uniform_real_distribution<float> cameraRandom;
};