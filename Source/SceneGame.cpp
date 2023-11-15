#include "Graphics/Graphics.h"
#include "Input\Input.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EffectManager.h"
#include "Stage.h"

// 初期化
void SceneGame::Initialize()
{
	//カメラコントローラー初期化
	cameraController = new CameraController();

	player = new Player();

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
	cameraController->setTarget(player->GetPosition());

	//エネミー初期化
	
	//ゲージスプライト
	Stage::Instance()->CreateStage();
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

	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	//カメラコントローラー更新処理
	cameraController->Update(elapsedTime);

	Stage::Instance()->Update(elapsedTime);
	player->Update(elapsedTime);

	//エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);
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
		player->Render(dc, shader);

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

	// 2DデバッグGUI描画
	{
		//プレイヤーデバッグ描画
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
		//トランスフォーム
		if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
		{
			////位置
			//ImGui::InputFloat3("Position", &(player->GetPosition()).x);

			////回転

			////スケール
			//ImGui::InputFloat3("Scale", &scale.x);
		}
		if (ImGui::CollapsingHeader("Movement", ImGuiTreeNodeFlags_DefaultOpen))
		{
		}

		if (ImGui::CollapsingHeader("CameraController", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//位置
			//DirectX::XMFLOAT3 tempAngle = CameraController::Instance().getAngle();
			DirectX::XMFLOAT3 tempAngle = cameraController->getAngle();
			//ImGui::InputFloat3("Angle", &tempAngle.x);

			//回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(tempAngle.x);
			a.y = DirectX::XMConvertToDegrees(tempAngle.y);
			a.z = DirectX::XMConvertToDegrees(tempAngle.z);
			ImGui::InputFloat3("Angle", &a.x);
		}

		if (ImGui::CollapsingHeader("Ground", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//回転
		}
		ImGui::End();
	}
#endif
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
