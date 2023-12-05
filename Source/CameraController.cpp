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

	//�J�����̉�]���x
	float speed = rollSpeed * elapsedTime;

	//�X�e�B�b�N�̓��͒l�ɍ��킹��X����Y������]
	if (!cameraShakeDetail.isShaking)
	{
		angle.y += ax * speed;
		angle.x += ay * -speed;
	}

	//X���̃J������]�𐧌�
	angle.x = std::min(angle.x, maxAngleX);
	angle.x = std::max(angle.x, minAngleX);

	if (angle.y < -DirectX::XM_PI) angle.y += DirectX::XM_2PI;
	if (angle.y > DirectX::XM_PI) angle.y -= DirectX::XM_2PI;
#endif // _DEBUG


	//�J������]�l����]�s��ɕϊ�
	//DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationX(angle.x);
	//DirectX::XMMATRIX Transform = DirectX::XMMatrixTranslationFromVector({ angle.x, angle.y, angle.z });
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	//��]�s�񂩂�O�����x�N�g�������o��
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);

	//�����_������x�N�g�������Ɉ�苗�����ꂽ�J�������_�����߂�
	DirectX::XMFLOAT3 eye;
	eye.x = target.x - front.x * range;
	eye.y = target.y - front.y * range;
	eye.z = target.z - front.z * range;

	//�J�������_�ƒ����_��ݒ�
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