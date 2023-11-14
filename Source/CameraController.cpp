#include <imgui.h>
#include "CameraController.h"
#include "Camera.h"
#include "Input/Input.h"
#ifdef _DEBUG
#include "Graphics/ImGuiRenderer.h"
#endif // _DEBUG


void CameraController::Update(float elapsedTime)
{
#if 0
	Camera::Instance().SetLookAt();
#else
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisRX();
	float ay = gamePad.GetAxisRY();

	//�J�����̉�]���x
	float speed = rollSpeed * elapsedTime;

	//�X�e�B�b�N�̓��͒l�ɍ��킹��X����Y������]
	{
		angle.y += ax * speed;
		angle.x += ay * -speed;
	}

	//X���̃J������]�𐧌�
	angle.x = min(angle.x, maxAngleX);
	angle.x = max(angle.x, minAngleX);

	if (angle.y < -DirectX::XM_PI) angle.y += DirectX::XM_2PI;
	if (angle.y > DirectX::XM_PI) angle.y -= DirectX::XM_2PI;

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
#endif
}

void CameraController::DrawIMGUI()
{
#ifdef _DEBUG
	if(ImGui::Begin("CameraController", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_None))
	{
		ImGui::SliderFloat("distance", &range, .0f, 100.0f);
		ImGui::SliderFloat3("forcasPos", &target.x, -50.0f, 50.0f, "%.3f", 1.0f);
		ImGui::SliderFloat3("angle", &angle.x, -DirectX::XM_PI, DirectX::XM_PI);
	}
	ImGui::End();
#endif // _DEBUG

}
