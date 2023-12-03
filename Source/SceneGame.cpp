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

// 初期化
void SceneGame::Initialize()
{
	//カメラコントローラー初期化
	cameraController = new CameraController();

	//方向マップ設定
	SetGlobalDirection();

	//ステージ
	Stage::Instance()->CreateStage();

	PlayerManager::Instance().Register(new Player);
	Player* player = PlayerManager::Instance().GetFirstPlayer();
	playerHP = std::make_unique<Sprite>();
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
	cameraController->setTarget({.0f,.0f,-7.f});

	Stage::Instance()->ResetAllSquare();
	Stage::Instance()->ResetSquaresAccessible();

	effects.emplace_back(std::make_unique<Effect>("./Data/Effect/Stun0.efk"));

	PhaseManager::Instance().Initialize();
}

// 終了化l
void SceneGame::Finalize()
{
	//カメラコントローラー終了化
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}

	PlayerManager::Instance().Clear();

	//エネミー終了化
	EnemyManager::Instance().Clear();

	GameSystemManager::Instance().SetPoused(false);
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
#ifdef _DEBUG
	//カメラコントローラー更新処理
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

	//エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);
	GameSystemManager::Instance().Update(elapsedTime);
}

// 描画処理
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 92.0f/255.0f,163/255.0f,188/255.0f };	// RGBA(0.0～1.0)
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
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);
		//ステージ描画
		Stage::Instance()->Render(dc, shader);
		PlayerManager::Instance().Render(dc, shader, rc);
		EnemyManager::Instance().Render(dc, shader);
		AttackManager::Instance().Render(dc, shader);

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

		const DirectX::XMFLOAT2 pos = { 50.0f,50.0f };
		const DirectX::XMFLOAT2 HpBarSize = { 250.0f,50.0f };
		const DirectX::XMFLOAT2 HpBarBorderSize = { 2.0f,2.0f };
		//外枠
		playerHP->Render(dc,
			pos.x - HpBarBorderSize.x, pos.y - HpBarBorderSize.y,
			HpBarSize.x + HpBarBorderSize.x * 2.0f, HpBarSize.y + HpBarBorderSize.y * 2.0f,
			.0f, .0f, static_cast<float>(playerHP->GetTextureWidth()), static_cast<float>(playerHP->GetTextureHeight()),
			DirectX::XMConvertToRadians(.0f), .0f, .0f, .0f, 1.0f);

		auto player = PlayerManager::Instance().GetFirstPlayer();

		//中身
		playerHP->Render(dc,
			pos.x, pos.y,
			HpBarSize.x*player->GetHealth()/static_cast<float>(player->GetMaxHealth()), HpBarSize.y,
			.0f, .0f, static_cast<float>(playerHP->GetTextureWidth()), static_cast<float>(playerHP->GetTextureHeight()),
			DirectX::XMConvertToRadians(.0f),
			1.0f, .0f, .0f, 1.0f);


		//シールドの描画
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
	// 2DデバッグGUI描画
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