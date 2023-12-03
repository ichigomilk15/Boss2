#include "Graphics/Graphics.h"
#include "Input\Input.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EffectManager.h"
#include "Effect.h"
#include "Stage.h"
#include "AttackManager.h"
#include "Common.h"
#include "EnemyManager.h"
#include "EnemyMinion1.h"
#include "PhaseManager.h"
#include "PlayerManager.h"
#include "GameSystemManager.h"
#include "Graphics/NumberSprite.h"

std::map<int, DirectX::XMFLOAT3> CommonClass::directionMaps;

// ������
void SceneGame::Initialize()
{
	//�J�����R���g���[���[������
	cameraController = new CameraController();

	//�����}�b�v�ݒ�
	SetGlobalDirection();

	//�X�e�[�W
	Stage::Instance()->CreateStage();

	PlayerManager::Instance().Register(new Player);
	Player* player = PlayerManager::Instance().GetFirstPlayer();
	playerHP = std::make_unique<Sprite>();
	CardManager::Instance().ALLClear();

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
	cameraController->setTarget({.0f,.0f,-7.f});

	Stage::Instance()->ResetAllSquare();
	Stage::Instance()->ResetSquaresAccessible();

	effects.emplace_back(std::make_unique<Effect>("./Data/Effect/Stun0.efk"));

	PhaseManager::Instance().Initialize();
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

	PlayerManager::Instance().Clear();

	//�G�l�~�[�I����
	EnemyManager::Instance().Clear();

	GameSystemManager::Instance().SetPoused(false);
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
#ifdef _DEBUG
	//�J�����R���g���[���[�X�V����
	cameraController->Update(elapsedTime);
#endif // _DEBUG
	if (GameSystemManager::Instance().GetIsPoused())
	{
		GameSystemManager::Instance().Update(elapsedTime);
		return;
	}

	Stage::Instance()->Update(elapsedTime);

	Stage::Instance()->Update(elapsedTime);
	CardManager::Instance().Update(elapsedTime);
	PlayerManager::Instance().Update(elapsedTime);
	EnemyManager::Instance().Update(elapsedTime, PlayerManager::Instance().GetFirstPlayer());
	AttackManager::Instance().Update(elapsedTime);

	PhaseManager::Instance().Update(elapsedTime);

	//�G�t�F�N�g�X�V����
	EffectManager::Instance().Update(elapsedTime);
	GameSystemManager::Instance().Update(elapsedTime);
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
		PlayerManager::Instance().Render(dc, shader, rc);
		EnemyManager::Instance().Render(dc, shader);
		AttackManager::Instance().Render(dc, shader);

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

	//2D�\��
	{

		const DirectX::XMFLOAT2 pos = { 50.0f,50.0f };
		const DirectX::XMFLOAT2 HpBarSize = { 250.0f,50.0f };
		const DirectX::XMFLOAT2 HpBarBorderSize = { 2.0f,2.0f };
		//�O�g
		playerHP->Render(dc,
			pos.x - HpBarBorderSize.x, pos.y - HpBarBorderSize.y,
			HpBarSize.x + HpBarBorderSize.x * 2.0f, HpBarSize.y + HpBarBorderSize.y * 2.0f,
			.0f, .0f, static_cast<float>(playerHP->GetTextureWidth()), static_cast<float>(playerHP->GetTextureHeight()),
			DirectX::XMConvertToRadians(.0f), .0f, .0f, .0f, 1.0f);

		auto player = PlayerManager::Instance().GetFirstPlayer();

		//���g
		playerHP->Render(dc,
			pos.x, pos.y,
			HpBarSize.x*player->GetHealth()/static_cast<float>(player->GetMaxHealth()), HpBarSize.y,
			.0f, .0f, static_cast<float>(playerHP->GetTextureWidth()), static_cast<float>(playerHP->GetTextureHeight()),
			DirectX::XMConvertToRadians(.0f),
			1.0f, .0f, .0f, 1.0f);


		//�V�[���h�̕`��
		playerHP->Render(dc,
			pos.x, pos.y,
			HpBarSize.x * (player->Getshield()/static_cast<float>(player->GetMaxHealth())), HpBarSize.y,
			.0f, .0f, static_cast<float>(playerHP->GetTextureWidth()), static_cast<float>(playerHP->GetTextureHeight()),
			DirectX::XMConvertToRadians(.0f),
			.0f, 0.1f, 1.0f, .6f);

		CardManager::Instance().Render(dc);
		PhaseManager::Instance().Render(dc);


		GameSystemManager::Instance().Render(dc);

		NumberSprite::Instance().NumberOut("1112345678999", dc, DirectX::XMFLOAT2{ .0f,.0f }, DirectX::XMFLOAT2{ 500.0f,125 }, DirectX::XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f });
	}
	// 2D�f�o�b�OGUI�`��
	{
		//�v���C���[�f�o�b�O�`��
		PlayerManager::Instance().DrawDebugGUI();
		EnemyManager::Instance().DrawDebugGUI();
		DrawDebugGUI();
		Stage::Instance()->DrawIMGUI();
		cameraController->DrawIMGUI();

		//DrawDebugGUI(player, cameraController);

		CardManager::Instance().DrawDebugGUI();
		PhaseManager::Instance().DrawDebugGUI();
	}

	auto&& manager = EffectManager::Instance().GetEffekseerManager().Get();
	manager->GetSetting();
	manager;
}

void SceneGame::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("GameScene", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_None))
	{
		static int playEffectIndex = 0;
		if (ImGui::InputInt("playEffectIndex", &playEffectIndex))
		{
			playEffectIndex = std::clamp(playEffectIndex, 0, static_cast<int>(effects.size() - 1));
		}

		if (ImGui::Button("playEffect"))
		{
			auto& effect = effects.at(playEffectIndex);
			effect->Play(DirectX::XMFLOAT3{.0f,.0f,.0f},2.0f);
		}
		if (ImGui::Button("StopEffect"))
		{
			auto& effect = effects.at(playEffectIndex);
			effect->Stop(effect->GetHandle());
		}
		if (ImGui::Button("sendTregger"))
		{
			auto& effect = effects.at(playEffectIndex);
			EffectManager::Instance().GetEffekseerManager()->SendTrigger(effect->GetHandle(), 0);
		}
	}
	ImGui::End();
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

void SceneGame::SetGlobalDirection()
{
	CommonClass::directionMaps.insert({ (int)CommonClass::DirectionFace::Front, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f) });
	CommonClass::directionMaps.insert({ (int)CommonClass::DirectionFace::FrontRight, DirectX::XMFLOAT3(0.0f, DirectX::XMConvertToRadians(45), 0.0f) });
	CommonClass::directionMaps.insert({ (int)CommonClass::DirectionFace::Right, DirectX::XMFLOAT3(0.0f, DirectX::XMConvertToRadians(90), 0.0f) });
	CommonClass::directionMaps.insert({ (int)CommonClass::DirectionFace::BackRight, DirectX::XMFLOAT3(0.0f, DirectX::XMConvertToRadians(135), 0.0f) });
	CommonClass::directionMaps.insert({ (int)CommonClass::DirectionFace::Back, DirectX::XMFLOAT3(0.0f, DirectX::XMConvertToRadians(180), 0.0f) });
	CommonClass::directionMaps.insert({ (int)CommonClass::DirectionFace::BackLeft, DirectX::XMFLOAT3(0.0f, DirectX::XMConvertToRadians(225), 0.0f) });
	CommonClass::directionMaps.insert({ (int)CommonClass::DirectionFace::Left, DirectX::XMFLOAT3(0.0f, DirectX::XMConvertToRadians(270), 0.0f) });
	CommonClass::directionMaps.insert({ (int)CommonClass::DirectionFace::FrontLeft, DirectX::XMFLOAT3(0.0f, DirectX::XMConvertToRadians(315), 0.0f) });
}