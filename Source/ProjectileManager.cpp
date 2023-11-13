#include "ProjectileManager.h"

ProjectileManager::ProjectileManager()
{
}

ProjectileManager::~ProjectileManager()
{
	Clear();
}

// 更新処理
void ProjectileManager::Update(float elapsedTime)
{
	for (Projectile* projectile : projectiles)
	{
		projectile->Update(elapsedTime);
	}

	// 破棄処理
	// ㌽ projectiles の範囲for文中でerase()すると不具合が発生してしまうため、
	//	　更新処理が終わった後に破棄リストに積まれたオブジェクトを削除する
	for (Projectile* projectile : removes)
	{
		// std::vectorから要素を削除する場合はイテレーターで削除するしなければならない
		std::vector<Projectile*>::iterator it = std::find(projectiles.begin(), projectiles.end(), projectile);
		if (it != projectiles.end())
		{
			projectiles.erase(it);
		}

		// 弾丸の破棄処理
		delete projectile;
	}
	removes.clear();
}

// 描画処理
void ProjectileManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (Projectile* projectile : projectiles)
	{
		projectile->Render(dc, shader);
	}
}

// デバッグプリミティブ描画
void ProjectileManager::DrawDebugPrimitive()
{
	for (Projectile* projectile : projectiles)
	{
		projectile->DrawDebugPrimitive();
	}
}

// 弾丸登録
void ProjectileManager::Register(Projectile* projectile)
{
	projectiles.emplace_back(projectile);
}

// 弾丸全
void ProjectileManager::Clear()
{
	for (Projectile* projectile : projectiles)
	{
		delete projectile;
	}
}

void ProjectileManager::Remove(Projectile* projectile)
{
	// 破棄リストに追加
	removes.insert(projectile);
}
