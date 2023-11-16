#include "Camera.h"
#include "Graphics\Graphics.h"

//指定方向を向く
void Camera::SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
	// 視点、注視点、上方向からビュー行列を作成
	DirectX::XMVECTOR Eye = { eye.x, eye.y, eye.z };
	DirectX::XMVECTOR Focus = { focus.x, focus.y, focus.z };
	DirectX::XMVECTOR Up = { up.x, up.y, up.z };
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	DirectX::XMStoreFloat4x4(&view, View);


	//ビューを逆行列化し、ワールド行列に戻す
	DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, World);

	//カメラの右方向を取り出す
	this->right.x = world._11;
	this->right.y = world._12;
	this->right.z = world._13;

	this->up.x = world._21;
	this->up.y = world._22;
	this->up.z = world._23;

	this->front.x = world._31;
	this->front.y = world._32;
	this->front.z = world._33;

	//視点, 注視点を保存
	this->eye = { world._41, world._42, world._43 };
	this->focus = focus;
}

void Camera::SetLookAt()
{
	// 視点、注視点、上方向からビュー行列を作成
	DirectX::XMVECTOR Eye = { eye.x, eye.y, eye.z };
	DirectX::XMVECTOR Focus = { focus.x, focus.y, focus.z };
	DirectX::XMVECTOR Up = { up.x, up.y, up.z };
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	DirectX::XMStoreFloat4x4(&view, View);


	//ビューを逆行列化し、ワールド行列に戻す
	DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, World);

	//カメラの右方向を取り出す
	this->right.x = world._11;
	this->right.y = world._12;
	this->right.z = world._13;

	this->up.x = world._21;
	this->up.y = world._22;
	this->up.z = world._23;

	this->front.x = world._31;
	this->front.y = world._32;
	this->front.z = world._33;

	//視点, 注視点を保存
	this->eye = { world._41, world._42, world._43 };
	this->focus = focus;
}

//パースペクティブ設定
void Camera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
	// 画角, 画面比率, クリップ距離からプロジェクション行列を作成
	DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
	DirectX::XMStoreFloat4x4(&projection, Projection);
}

void Camera::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Eye");
		ImGui::SliderFloat3("eyeX", &eye.x, -20.0f, 20.0f, "%.02f");
		ImGui::Text("Focus");
		ImGui::SliderFloat3("focusX", &focus.x, -20.0f, 20.0f, "%.02f");
		ImGui::Text("Up");
		ImGui::SliderFloat3("upX", &up.x, -20.0f, 20.0f, "%.02f");
		/*if (ImGui::Button("Reset"))
		{
			Reset();
		}*/
	}
	ImGui::End();
}
