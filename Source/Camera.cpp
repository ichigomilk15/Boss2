#include "Camera.h"
#include "Graphics\Graphics.h"

//�w�����������
void Camera::SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
	// ���_�A�����_�A���������r���[�s����쐬
	DirectX::XMVECTOR Eye = { eye.x, eye.y, eye.z };
	DirectX::XMVECTOR Focus = { focus.x, focus.y, focus.z };
	DirectX::XMVECTOR Up = { up.x, up.y, up.z };
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	DirectX::XMStoreFloat4x4(&view, View);


	//�r���[���t�s�񉻂��A���[���h�s��ɖ߂�
	DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, World);

	//�J�����̉E���������o��
	this->right.x = world._11;
	this->right.y = world._12;
	this->right.z = world._13;

	this->up.x = world._21;
	this->up.y = world._22;
	this->up.z = world._23;

	this->front.x = world._31;
	this->front.y = world._32;
	this->front.z = world._33;

	//���_, �����_��ۑ�
	this->eye = { world._41, world._42, world._43 };
	this->focus = focus;
}

void Camera::SetLookAt()
{
	// ���_�A�����_�A���������r���[�s����쐬
	DirectX::XMVECTOR Eye = { eye.x, eye.y, eye.z };
	DirectX::XMVECTOR Focus = { focus.x, focus.y, focus.z };
	DirectX::XMVECTOR Up = { up.x, up.y, up.z };
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	DirectX::XMStoreFloat4x4(&view, View);


	//�r���[���t�s�񉻂��A���[���h�s��ɖ߂�
	DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, World);

	//�J�����̉E���������o��
	this->right.x = world._11;
	this->right.y = world._12;
	this->right.z = world._13;

	this->up.x = world._21;
	this->up.y = world._22;
	this->up.z = world._23;

	this->front.x = world._31;
	this->front.y = world._32;
	this->front.z = world._33;

	//���_, �����_��ۑ�
	this->eye = { world._41, world._42, world._43 };
	this->focus = focus;
}

//�p�[�X�y�N�e�B�u�ݒ�
void Camera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
	// ��p, ��ʔ䗦, �N���b�v��������v���W�F�N�V�����s����쐬
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
