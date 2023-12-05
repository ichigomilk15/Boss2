#define NOMINMAX

#include <imgui.h>
#include "CameraController.h"
#include "Camera.h"
#include "Input/Input.h"
#ifdef _DEBUG
#include "Graphics/ImGuiRenderer.h"
#endif // _DEBUG
#include <algorithm>

#undef NOMINMAX


void CameraController::Update(float elapsedTime)
{
#ifdef _DEBUG
	UpdateCameraShake(elapsedTime);
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisRX();
	float ay = gamePad.GetAxisRY();

	//カメラの回転速度
	float speed = rollSpeed * elapsedTime;

	//スティックの入力値に合わせてX軸とY軸を回転
	if (!cameraShakeDetail.isShaking)
	{
		angle.y += ax * speed;
		angle.x += ay * -speed;
	}

	//X軸のカメラ回転を制限
	angle.x = std::min(angle.x, maxAngleX);
	angle.x = std::max(angle.x, minAngleX);

	if (angle.y < -DirectX::XM_PI) angle.y += DirectX::XM_2PI;
	if (angle.y > DirectX::XM_PI) angle.y -= DirectX::XM_2PI;
#endif // _DEBUG


	//カメラ回転値を回転行列に変換
	//DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationX(angle.x);
	//DirectX::XMMATRIX Transform = DirectX::XMMatrixTranslationFromVector({ angle.x, angle.y, angle.z });
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	//回転行列から前方向ベクトルを取り出す
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);

	//注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
	DirectX::XMFLOAT3 eye;
	eye.x = target.x - front.x * range;
	eye.y = target.y - front.y * range;
	eye.z = target.z - front.z * range;

	//カメラ視点と注視点を設定
	Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));

}

void CameraController::DrawIMGUI()
{
#ifdef _DEBUG
	if (ImGui::Begin("CameraController", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_None))
	{
		ImGui::SliderFloat("distance", &range, .01f, 100.0f);
		ImGui::SliderFloat3("forcasPos", &target.x, -50.0f, 50.0f, "%.3f", 1.0f);
		ImGui::SliderFloat3("angle", &angle.x, -DirectX::XM_PI, DirectX::XM_PI);
	}
	ImGui::End();
#endif // _DEBUG

}

void CameraController::UpdateCameraShake(float elapsedTime)
{
	if (!cameraShakeDetail.isShaking)
		return;

	Camera& camera = Camera::Instance();
	const float angleMax = std::max(0.0f, (3.0f + cameraShakeDetail.shakeLevel) * (cameraShakeDetail.shakeTimer / cameraShakeDetail.shakeTimerMax));
	cameraRandom = std::uniform_real_distribution<float>(-angleMax, angleMax);

	angle.x = cameraShakeDetail.angleInit.x + DirectX::XMConvertToRadians(cameraRandom(CommonClass::random));
	angle.y = cameraShakeDetail.angleInit.y + DirectX::XMConvertToRadians(cameraRandom(CommonClass::random));

	cameraShakeDetail.shakeTimer -= elapsedTime;
	if (cameraShakeDetail.shakeTimer <= 0.0f)
	{
		cameraShakeDetail.isShaking = false;
		cameraShakeDetail.shakeTimer = 0.0f;
		angle = cameraShakeDetail.angleInit;
	}
}

void CameraController::ShakeCamera(const float timer, const int level)
{
	if (!cameraShakeDetail.isShaking)
	{
		cameraShakeDetail.angleInit = angle;
	}
	cameraShakeDetail.isShaking = true;
	cameraShakeDetail.shakeTimer = timer;
	cameraShakeDetail.shakeTimerMax = timer;
	cameraShakeDetail.shakeLevel = level;
}