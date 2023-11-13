#include "Graphics/Graphics.h"
#include "Input\Input.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "EffectManager.h"
#include "StageManager.h"
#include "StageMain.h"
#include "StageMoveFloor.h"

// ������
void SceneGame::Initialize()
{
	//�J�����R���g���[���[������
	cameraController = new CameraController();
	player = new Player();

	//�X�e�[�W������
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain();
	stageManager.Register(stageMain);
	StageMoveFloor* stageMoveFloor = new StageMoveFloor();
	stageMoveFloor->SetStartPoint(DirectX::XMFLOAT3(0, 1, 3));
	stageMoveFloor->SetGoalPoint(DirectX::XMFLOAT3(10, 2, 3));
	stageMoveFloor->SetTorque(DirectX::XMFLOAT3(0, 1.0f, 0));
	stageManager.Register(stageMoveFloor);

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

	//�G�l�~�[������
	EnemyManager& enemyManager = EnemyManager::Instance();
	//EnemyManager::Instance().Register(new EnemySlime());
	for (int i = 0; i < 1; ++i)
	{
		EnemySlime* slime = new EnemySlime();
		slime->SetPosition(DirectX::XMFLOAT3(-3 + i * 3.0f, 0, 5 * (i % 3)));
		enemyManager.Register(slime);
	}
	
	//�Q�[�W�X�v���C�g
	gauge = new Sprite();
}

// �I����l
void SceneGame::Finalize()
{
	//�Q�[�W�X�v���C�g�I����
	if (gauge != nullptr)
	{
		delete gauge;
		gauge = nullptr;
	}

	EnemyManager::Instance().Clear();
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
	/*if (enemyManager != nullptr)
	{
		delete enemyManager;
		enemyManager = nullptr;
	}*/
	//�X�e�[�W�I����
	StageManager::Instance().Clear();
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
	//�J�����R���g���[���[�X�V����
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 0.5f;
	cameraController->setTarget(target);
	cameraController->Update(elapsedTime);

	StageManager::Instance().Update(elapsedTime);
	player->Update(elapsedTime);
	EnemyManager::Instance().Update(elapsedTime);

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
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0�`1.0)
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
		StageManager::Instance().Render(dc, shader);

		//�v���C���[�`��
		player->Render(dc, shader);
		EnemyManager::Instance().Render(dc, shader);

		shader->End(dc);

	}

	//3D���f���`��
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3D�f�o�b�O�`��
	{
		//�v���C���[�f�o�b�O�v���~�e�B�u�`��
		graphics.GetDebugRenderer()->Render(dc, rc.view, player->GetTransform());
		player->DrawDebugPrimitive();
		//player->DrawDebugGUI();
		EnemyManager::Instance().DrawDebugPrimitive();
		//player->getProjectileManagaer().DrawDebugPrimitive();

		// ���C�������_���`����s
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// �f�o�b�O�����_���`����s
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	// 2D�X�v���C�g�`��
	{
		RenderEnemyGauge(dc, rc.view, rc.projection);
	}

	// 2D�f�o�b�OGUI�`��
	{
		//�v���C���[�f�o�b�O�`��
		//player->DrawDebugGUI();
		DrawDebugGUI();
		//DrawDebugGUI(player, cameraController);
	}
}

//void DrawDebugGUI(Player* player, CameraController* cameraController)
void SceneGame::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Window 1", nullptr, ImGuiWindowFlags_None))
	{
		//�g�����X�t�H�[��
		if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
		{
			////�ʒu
			//ImGui::InputFloat3("Position", &(player->GetPosition()).x);

			////��]
			DirectX::XMFLOAT3 a;
			DirectX::XMFLOAT3 playerAngle = player->GetAngle();
			a.x = DirectX::XMConvertToDegrees(playerAngle.x);
			a.y = DirectX::XMConvertToDegrees(playerAngle.y);
			a.z = DirectX::XMConvertToDegrees(playerAngle.z);
			ImGui::InputFloat3("Angle", &a.x);
			playerAngle.x = DirectX::XMConvertToRadians(a.x);
			playerAngle.y = DirectX::XMConvertToRadians(a.y);
			playerAngle.z = DirectX::XMConvertToRadians(a.z);

			////�X�P�[��
			//ImGui::InputFloat3("Scale", &scale.x);
		}
		if (ImGui::CollapsingHeader("Movement", ImGuiTreeNodeFlags_DefaultOpen))
		{
			float playerSpeed = player->getMoveSpeed();
			ImGui::InputFloat("MoveSpeed", &playerSpeed);
			player->setMoveSpeed(playerSpeed);

			float playerTurnSpeed = DirectX::XMConvertToDegrees(player->getTurnSpeed());
			ImGui::InputFloat("TurnSpeed", &playerTurnSpeed);
			player->setTurnSpeed(DirectX::XMConvertToRadians(playerTurnSpeed));

			DirectX::XMFLOAT3 playerVelocity = player->getVelocity();
			float lengthVel = sqrtf(playerVelocity.x * playerVelocity.x + playerVelocity.z * playerVelocity.z);
			ImGui::InputFloat("Movement Speed", &lengthVel);
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
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(player->groundNormal.x);
			a.y = DirectX::XMConvertToDegrees(player->groundNormal.y);
			a.z = DirectX::XMConvertToDegrees(player->groundNormal.z);
			ImGui::InputFloat3("Angle", &a.x);
		}
		ImGui::End();
	}
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
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	float enemyHpSizeX = 15;

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);
		//���[���h�@-> �r���[�ϊ��@-> �N���b�v���W�@-> ���K�����W�@->�@�X�N���[�����W
		DirectX::XMFLOAT3 enemyPos = enemy->GetPosition();
		DirectX::XMVECTOR EnemyPos;

		EnemyPos = DirectX::XMVectorSet(
			enemyPos.x - enemy->GetRadius(), enemyPos.y + enemy->GetHeight() + 0.1f, enemyPos.z, 0);
		DirectX::XMVECTOR ScreenPositionEnemyLeft = DirectX::XMVector3Project(
			EnemyPos,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			Projection,
			View,
			World
		);

		DirectX::XMFLOAT3 screenPosEnemyLeft;
		DirectX::XMStoreFloat3(&screenPosEnemyLeft, ScreenPositionEnemyLeft);
		if (screenPosEnemyLeft.z > 1.0f) continue;

		EnemyPos = DirectX::XMVectorSet(
			enemyPos.x + enemy->GetRadius(), enemyPos.y + enemy->GetHeight() + 0.1f, enemyPos.z, 0);
		DirectX::XMVECTOR ScreenPositionEnemyRight = DirectX::XMVector3Project(
			EnemyPos,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			Projection,
			View,
			World
		);

		DirectX::XMFLOAT3 screenPosEnemyRight;
		DirectX::XMStoreFloat3(&screenPosEnemyRight, ScreenPositionEnemyRight);

		//DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
		float enemyHealthWidth = (screenPosEnemyRight.x - screenPosEnemyLeft.x) * enemy->GetHealth() / enemy->GetMaxHealth();
		int enmHealthReduction = enemy->GetMaxHealth() - enemy->GetHealth();
		gauge->Render(
			dc, screenPosEnemyLeft.x + enemyHealthWidth * enmHealthReduction / enemy->GetMaxHealth(), screenPosEnemyLeft.y,
			enemyHealthWidth, 8, 
			0, 0, 0, 0, 0,
			1, 0, 0, 1
		);
	}

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
			mouse.GetPositionX(), 
			mouse.GetPositionY(),
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
			mouse.GetPositionX(),
			mouse.GetPositionY(),
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

		HitResult hit;
		if (StageManager::Instance().RayCast(startRay, endRay, hit))
		{
			EnemySlime* slime = new EnemySlime();
			slime->SetPosition(DirectX::XMFLOAT3(hit.position.x, hit.position.y, hit.position.z));
			enemyManager.Register(slime);
		}
	}
}
