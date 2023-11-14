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

	//カメラの回転速度
	float speed = rollSpeed * elapsedTime;

	//スティックの入力値に合わせてX軸とY軸を回転
	{
		angle.y += ax * speed;
		angle.x += ay * -speed;
	}

	//X軸のカメラ回転を制限
	angle.x = min(angle.x, maxAngleX);
	angle.x = max(angle.x, minAngleX);

	if (angle.y < -DirectX::XM_PI) angle.y += DirectX::XM_2PI;
	if (angle.y > DirectX::XM_PI) angle.y -= DirectX::XM_2PI;

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
