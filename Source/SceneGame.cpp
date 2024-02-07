#include "Graphics/Graphics.h"
#include "Input\Input.h"
#include "Audio\AudioLoader.h"
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
#include "DamageEffector.h"

std::map<int, DirectX::XMFLOAT3> CommonClass::directionMaps;
bool SceneGame::isTutrialfinished = false;

// ������
void SceneGame::Initialize()
{
	//�J�����R���g���[���[������
	//cameraController = new CameraController();

	//�����}�b�v�ݒ�
	SetGlobalDirection();

	//�X�e�[�W
	Stage::Instance()->CreateStage();

	PlayerManager::Instance().Register(new Player);
	Player* player = PlayerManager::Instance().GetFirstPlayer();
	playerHP[0] = std::make_unique<Sprite>("./Data/Sprite/life_waku.png");
	playerHP[1] = std::make_unique<Sprite>("./Data/Sprite/life_1.png");
	playerHP[2] = std::make_unique<Sprite>("./Data/Sprite/life_2.png");
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
	cameraController->setTarget({ .0f,.0f,-7.f });

	Stage::Instance()->ResetAllSquare();
	Stage::Instance()->ResetSquaresAccessible();

	PhaseManager::Instance().Initialize();

	AudioLoader::Load(AUDIO::BGM_GAME, gameSe);

	// BGM�Đ�
	gameSe->Play(true);

	SaveData::Instance().Load();
	if (!isTutrialfinished)
	{
		GameSystemManager::Instance().CollTutorial();
		isTutrialfinished = true;
	}

#ifdef _DEBUG
	sprite = Sprite(nullptr);
	effec = std::make_unique<Effect>("./Data/Effect/dizzy.efk");
#endif // _DEBUG

}

// �I����
void SceneGame::Finalize()
{
	//�J�����R���g���[���[�I����
	/*if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}*/

	PlayerManager::Instance().Clear();

	//�G�l�~�[�I����
	EnemyManager::Instance().Clear();

	GameSystemManager::Instance().SetPoused(false);

	EffectManager::Instance().GetEffekseerManager()->StopAllEffects();

	// BGM��~
	gameSe->Stop();
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
	//�J�����R���g���[���[�X�V����
	cameraController->Update(elapsedTime);
#if _DEBUG
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & gamePad.BTN_A)
	{
		cameraController->ShakeCamera(0.5f);
	}
	if (gamePad.GetButtonDown() & gamePad.BTN_B)
	{
		auto e = EnemyManager::Instance().GetEnemy(0);
		e->AddImpulse({ 0.0f, 0.8f, 0.0f });
	}
	if (gamePad.GetButtonDown() & gamePad.BTN_X)
	{
		auto e = EnemyManager::Instance().GetEnemy(0);
		e->AddImpulse({ 0.0f, -1.8f, 0.0f });
	}
#endif // _DEBUG
	//�|�[�Y���̏���
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
	DamageEffector::Instance().Update(elapsedTime);
}

// �`�揈��
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	const DirectX::XMFLOAT2 ScreenSize = graphics.GetScreenSize();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 92.0f / 255.0f,163 / 255.0f,188 / 255.0f };	// RGBA(0.0�`1.0)
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
		Shader* shader = graphics.GetShader(Shader3D::Lambert);
		shader->Begin(dc, rc);
		//�X�e�[�W�`��
		Stage::Instance()->Render(dc, shader);
		AttackManager::Instance().Render(dc, shader);

		shader->End(dc);

		shader = graphics.GetShader(Shader3D::LambertMask);
		shader->Begin(dc, rc);
		PlayerManager::Instance().Render(dc, shader, rc);
		EnemyManager::Instance().Render(dc, shader, rc);
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

		CardManager::Instance().Render(dc);
		PhaseManager::Instance().Render(dc);

		DamageEffector::Instance().Render(dc);

		//�̗̓o�[�`��
		{
			Player* pl = PlayerManager::Instance().GetFirstPlayer();
			pl->Render2D(dc, HitBox2D::CreateBoxFromTopLeft(DirectX::XMFLOAT2{ .0f,.0f, }, DirectX::XMFLOAT2{ ScreenSize.x * 0.2f,ScreenSize.y * 0.1f }));

			EnemyManager::Instance().Render2D(dc);
		}

		PhaseManager::Instance().RenderWaveChange(dc);

		GameSystemManager::Instance().Render(dc);//��ԍŌ�ɕ`�悷�邱��
	}
	// 2D�f�o�b�OGUI�`��
#ifdef _DEBUG
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


	sprite.Render(dc, Input::Instance().GetMouse().GetPosition(), { 5.0f,5.0f }, .0f, { 1.0f,.0f,.0f,1.0f });
#endif // _DEBUG
}

void SceneGame::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("GameScene", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_None))
	{
		if (ImGui::Button("saveData"))
		{
			SaveData::Instance().Save();
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

//********************************************************************************************************
// SaveData Class
//********************************************************************************************************

const bool SaveData::Save()
{
	bool ok = true;
	ReSet();

	this->StageLevel = Stage::Instance()->GetStageLevel();
	this->PhaseTurn = PhaseManager::Instance().GetTrunCount();
	if (auto player = PlayerManager::Instance().GetFirstPlayer())
	{
		playerHp = player->GetHealth();
		playerpos = { player->GetPosition().x,player->GetPosition().y };
	}
	else
	{
		ok = false;
	}


	return ok;
}

const bool SaveData::ReSet()
{
	bool ok;
	ok = true;

	this->StageLevel = 1;//1����X�^�[�g
	this->PhaseTurn = 0;
	this->playerHp = -1;
	this->playerpos = { -1,-1 };
	return ok;
}

const bool SaveData::Load()
{
	bool ok = true;
	Stage::Instance()->SetStageLevel(this->StageLevel - 1);
	PhaseManager::Instance().SetTurnCount(this->PhaseTurn);
	if (auto player = PlayerManager::Instance().GetFirstPlayer())
	{
		//player->SetHealth((this->playerHp > 0 ? this->playerHp : player->GetMaxHealth()));
		if (this->playerpos.first < 0 || this->playerpos.second < 0) { ok = false; }
		else player->SetTurnPosInit({ playerpos.first,playerpos.second });
	}

	return ok;
}
