#include "Graphics\Graphics.h"
#include "Effect.h"
#include "EffectManager.h"

//コンストラクタ
Effect::Effect(const char* filename)
{
	//エフェクトを読み込みする前にロックする
	//*マルチスレッドでEffectを作成するとDeviceContextを同時アクセスして
	//	フリーズする可能性があるので排他制御する
	std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());

	//Effekseerのシリーズを読み込む
	//EffekseerはUTF-16のファイルパス以外は対応していないため文字コード変換が必要
	char16_t utf16Filename[256];
	Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filename);

	//Effekseer::Managerを取得
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	//Effekseerエフェクトを読み込み
	effekseerEffect = Effekseer::Effect::Create(effekseerManager, (EFK_CHAR*)utf16Filename);
}

//再生
Effekseer::Handle Effect::Play(const DirectX::XMFLOAT3& positionWorld, float scale)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	Effekseer::Handle handle = effekseerManager->Play(effekseerEffect, positionWorld.x, positionWorld.y, positionWorld.z);
	effekseerManager->SetScale(handle, scale, scale, scale);
	return handle;
}

//停止
void Effect::Stop(Effekseer::Handle handle)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->StopEffect(handle);
}

//座標設定
void Effect::SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& positionWorld)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->SetLocation(handle, positionWorld.x, positionWorld.y, positionWorld.z);
}

//スケール設定
void Effect::SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
}
