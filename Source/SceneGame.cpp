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

// 初期化
void SceneGame::Initialize()
{
	//カメラコントローラー初期化
	//cameraController = new CameraController();

	//方向マップ設定
	SetGlobalDirection();

	//ステージ
	Stage::Instance()->CreateStage();

	PlayerManager::Instance().Register(new Player);
	Player* player = PlayerManager::Instance().GetFirstPlayer();
	playerHP[0] = std::make_unique<Sprite>("./Data/Sprite/life_waku.png");
	playerHP[1] = std::make_unique<Sprite>("./Data/Sprite/life_1.png");
	playerHP[2] = std::make_unique<Sprite>("./Data/Sprite/life_2.png");
	CardManager::Instance().ALLClear();

	//カメラ初期設定
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

	// BGM再生
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

// 終了化
void SceneGame::Finalize()
{
	//カメラコントローラー終了化
	/*if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}*/

	PlayerManager::Instance().Clear();

	//エネミー終了化
	EnemyManager::Instance().Clear();

	GameSystemManager::Instance().SetPoused(false);

	EffectManager::Instance().GetEffekseerManager()->StopAllEffects();

	// BGM停止
	gameSe->Stop();
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	//カメラコントローラー更新処理
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
	//ポーズ時の処理
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

	//エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);
	GameSystemManager::Instance().Update(elapsedTime);
	DamageEffector::Instance().Update(elapsedTime);
}

// 描画処理
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	const DirectX::XMFLOAT2 ScreenSize = graphics.GetScreenSize();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 92.0f / 255.0f,163 / 255.0f,188 / 255.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 描画処理
	RenderContext rc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ライト方向（下方向）

	//カメラパラメータ設定
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	// 3Dモデル描画
	{
		Shader* shader = graphics.GetShader(Shader3D::Lambert);
		shader->Begin(dc, rc);
		//ステージ描画
		Stage::Instance()->Render(dc, shader);
		AttackManager::Instance().Render(dc, shader);

		shader->End(dc);

		shader = graphics.GetShader(Shader3D::LambertMask);
		shader->Begin(dc, rc);
		PlayerManager::Instance().Render(dc, shader, rc);
		EnemyManager::Instance().Render(dc, shader, rc);
		shader->End(dc);
	}

	//エフェクト描画
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3Dデバッグ描画
	{
		//プレイヤーデバッグプリミティブ描画

		// ラインレンダラ描画実行
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// デバッグレンダラ描画実行
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	//2D表示
	{

		CardManager::Instance().Render(dc);
		PhaseManager::Instance().Render(dc);

		DamageEffector::Instance().Render(dc);

		//体力バー描画
		{
			Player* pl = PlayerManager::Instance().GetFirstPlayer();
			pl->Render2D(dc, HitBox2D::CreateBoxFromTopLeft(DirectX::XMFLOAT2{ .0f,.0f, }, DirectX::XMFLOAT2{ ScreenSize.x * 0.2f,ScreenSize.y * 0.1f }));

			EnemyManager::Instance().Render2D(dc);
		}

		PhaseManager::Instance().RenderWaveChange(dc);

		GameSystemManager::Instance().Render(dc);//一番最後に描画すること
	}
	// 2DデバッグGUI描画
#ifdef _DEBUG
	{
		//プレイヤーデバッグ描画
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
	//ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	//変換行列
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	//全ての敵の頭上にHPゲージを表示

	float enemyHpSizeX = 15;

	//エネミー配置処理
	Mouse& mouse = Input::Instance().GetMouse();
	if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
	{
		//マウスカーソル座標を取得
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
			viewport.TopLeftX,		//ビューポート左上X位置
			viewport.TopLeftY,		//ビューポート左上Y位置
			viewport.Width,			//ビューポート幅
			viewport.Height,		//ビューポート高さ
			viewport.MinDepth,		//深度値の範囲を表す最小値（0.0でよい）
			viewport.MaxDepth,		//深度値の範囲を表す最大値（1.0でよい）
			Projection,				//プロジェクション行列
			View,					//ビュー行列
			World					//ワールド行列（単位行列でよい）
		);
		DirectX::XMVECTOR EndRay2D = DirectX::XMVectorSet(
			static_cast<float>(mouse.GetPositionX()),
			static_cast<float>(mouse.GetPositionY()),
			viewport.MaxDepth, 0);
		DirectX::XMVECTOR EndRayVec = DirectX::XMVector3Unproject(
			EndRay2D,
			viewport.TopLeftX,		//ビューポート左上X位置
			viewport.TopLeftY,		//ビューポート左上Y位置
			viewport.Width,			//ビューポート幅
			viewport.Height,		//ビューポート高さ
			viewport.MinDepth,		//深度値の範囲を表す最小値（0.0でよい）
			viewport.MaxDepth,		//深度値の範囲を表す最大値（1.0でよい）
			Projection,				//プロジェクション行列
			View,					//ビュー行列
			World					//ワールド行列（単位行列でよい）
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

	this->StageLevel = 1;//1からスタート
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
