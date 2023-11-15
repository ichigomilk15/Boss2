#include "Graphics/Graphics.h"
#include "Input\Input.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EffectManager.h"
#include "Stage.h"

// ������
void SceneGame::Initialize()
{
	//�J�����R���g���[���[������
	cameraController = new CameraController();

	player = new Player();

	//�J���������ݒ�
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f
	);
	cameraController->setTarget(player->GetPosition());

	//�G�l�~�[������
	
	//�Q�[�W�X�v���C�g
	Stage::Instance()->CreateStage();
}

// �I����l
void SceneGame::Finalize()
{
	//�J�����R���g���[���[�I����
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}

	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
	//�J�����R���g���[���[�X�V����
	cameraController->Update(elapsedTime);

	Stage::Instance()->Update(elapsedTime);
	player->Update(elapsedTime);

	//�G�t�F�N�g�X�V����
	EffectManager::Instance().Update(elapsedTime);
}

// �`�揈��
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 92.0f/255.0f,163/255.0f,188/255.0f };	// RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// �`�揈��
	RenderContext rc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ���C�g�����i�������j

	//�J�����p�����[�^�ݒ�
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	// 3D���f���`��
	{
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);
		//�X�e�[�W�`��
		Stage::Instance()->Render(dc, shader);
		player->Render(dc, shader);

		shader->End(dc);

	}

	//�G�t�F�N�g�`��
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3D�f�o�b�O�`��
	{
		//�v���C���[�f�o�b�O�v���~�e�B�u�`��

		// ���C�������_���`����s
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// �f�o�b�O�����_���`����s
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	// 2D�f�o�b�OGUI�`��
	{
		//�v���C���[�f�o�b�O�`��
		player->DrawDebugGUI();
		DrawDebugGUI();
		Stage::Instance()->DrawIMGUI();
		cameraController->DrawIMGUI();
		//DrawDebugGUI(player, cameraController);
	}
}

void SceneGame::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

#if 0
	if (ImGui::Begin("Window 1", nullptr, ImGuiWindowFlags_None))
	{
		//�g�����X�t�H�[��
		if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
		{
			////�ʒu
			//ImGui::InputFloat3("Position", &(player->GetPosition()).x);

			////��]

			////�X�P�[��
			//ImGui::InputFloat3("Scale", &scale.x);
		}
		if (ImGui::CollapsingHeader("Movement", ImGuiTreeNodeFlags_DefaultOpen))
		{
		}

		if (ImGui::CollapsingHeader("CameraController", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//�ʒu
			//DirectX::XMFLOAT3 tempAngle = CameraController::Instance().getAngle();
			DirectX::XMFLOAT3 tempAngle = cameraController->getAngle();
			//ImGui::InputFloat3("Angle", &tempAngle.x);

			//��]
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(tempAngle.x);
			a.y = DirectX::XMConvertToDegrees(tempAngle.y);
			a.z = DirectX::XMConvertToDegrees(tempAngle.z);
			ImGui::InputFloat3("Angle", &a.x);
		}

		if (ImGui::CollapsingHeader("Ground", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//��]
		}
		ImGui::End();
	}
#endif
}

void SceneGame::RenderEnemyGauge(ID3D11DeviceContext* dc, 
	const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	//�r���[�|�[�g
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	//�ϊ��s��
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	//�S�Ă̓G�̓����HP�Q�[�W��\��

	float enemyHpSizeX = 15;

	//�G�l�~�[�z�u����
	Mouse& mouse = Input::Instance().GetMouse();
	if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
	{
		//�}�E�X�J�[�\�����W���擾
		DirectX::XMFLOAT3 screenPosition;
		screenPosition.x = static_cast<float>(mouse.GetPositionX());
		screenPosition.y = static_cast<float>(mouse.GetPositionY());

		DirectX::XMVECTOR ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);

		DirectX::XMVECTOR StartRay2D = DirectX::XMVectorSet(
			static_cast<float>(mouse.GetPositionX()), 
			static_cast<float>(mouse.GetPositionY()),
			viewport.MinDepth, 0);
		DirectX::XMVECTOR StartRayVec = DirectX::XMVector3Unproject(
			StartRay2D,
			viewport.TopLeftX,		//�r���[�|�[�g����X�ʒu
			viewport.TopLeftY,		//�r���[�|�[�g����Y�ʒu
			viewport.Width,			//�r���[�|�[�g��
			viewport.Height,		//�r���[�|�[�g����
			viewport.MinDepth,		//�[�x�l�͈̔͂�\���ŏ��l�i0.0�ł悢�j
			viewport.MaxDepth,		//�[�x�l�͈̔͂�\���ő�l�i1.0�ł悢�j
			Projection,				//�v���W�F�N�V�����s��
			View,					//�r���[�s��
			World					//���[���h�s��i�P�ʍs��ł悢�j
		);
		DirectX::XMVECTOR EndRay2D = DirectX::XMVectorSet(
			static_cast<float>(mouse.GetPositionX()),
			static_cast<float>(mouse.GetPositionY()),
			viewport.MaxDepth, 0);
		DirectX::XMVECTOR EndRayVec = DirectX::XMVector3Unproject(
			EndRay2D,
			viewport.TopLeftX,		//�r���[�|�[�g����X�ʒu
			viewport.TopLeftY,		//�r���[�|�[�g����Y�ʒu
			viewport.Width,			//�r���[�|�[�g��
			viewport.Height,		//�r���[�|�[�g����
			viewport.MinDepth,		//�[�x�l�͈̔͂�\���ŏ��l�i0.0�ł悢�j
			viewport.MaxDepth,		//�[�x�l�͈̔͂�\���ő�l�i1.0�ł悢�j
			Projection,				//�v���W�F�N�V�����s��
			View,					//�r���[�s��
			World					//���[���h�s��i�P�ʍs��ł悢�j
		);

		DirectX::XMFLOAT3 startRay, endRay;
		DirectX::XMStoreFloat3(&startRay, StartRayVec);
		DirectX::XMStoreFloat3(&endRay, EndRayVec);
	}
}
