#include "EnemyManager.h"
#include "Collision.h"

void EnemyManager::CollisionEnemyVsEnemies()
{
	int enemyCount = enemies.size();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemyA = enemies.at(i);
		for (int j = i + 1; j < enemyCount; ++j)
		{
			Enemy* enemyB = enemies.at(j);
			DirectX::XMFLOAT3 outPosition;

			if (Collision::IntersectCylinderVsCylinder(
				enemyA->GetPosition(),
				enemyA->GetRadius(),
				enemyA->GetHeight(),
				enemyB->GetPosition(),
				enemyB->GetRadius(),
				enemyB->GetHeight(),
				outPosition
			))
			{
				enemyB->SetPosition(outPosition);
			}
		}
	}
	//for (auto& enemy : enemies)
	//{
	//	for (auto& enemy2 : enemies)
	//	{
	//		if (enemy == enemy2)continue;
	//		// 衝突処理
	//		DirectX::XMFLOAT3 outPosition;
	//		//if (Collision::IntersectSphereVsSphere(
	//		//	enemy->GetPosition(),
	//		//	enemy->GetRadius(),
	//		//	enemy2->GetPosition(),
	//		//	enemy2->GetRadius(),
	//		//	outPosition
	//		//))
	//		//{
	//		//	//押し出し後の位置設定
	//		//	enemy2->SetPosition(outPosition);
	//		//}
	//		if (Collision::IntersectCylinderVsCylinder(
	//			enemy->GetPosition(),
	//			enemy->GetRadius(),
	//			enemy->GetHeight(),
	//			enemy2->GetPosition(),
	//			enemy2->GetRadius(),
	//			enemy2->GetHeight(),
	//			outPosition
	//		))
	//		{
	//			enemy2->SetPosition(outPosition);
	//		}
	//	}
	//}
}

void EnemyManager::Remove(Enemy* enemy)
{
	//破棄リストに増加
	removes.insert(enemy);
}

void EnemyManager::Update(float elapsedTime)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}

	//破棄処理
	for (Enemy* enemy : removes)
	{
		// std::vectorから要素を削除する場合はイテレーターで削除するしなければならない
		std::vector<Enemy*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);
		if (it != enemies.end())
		{
			enemies.erase(it);
		}

		// 弾丸の破棄処理
		delete enemy;
	}
	removes.clear();

	// 敵同士の衝突処理
	CollisionEnemyVsEnemies();
}

void EnemyManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Render(context, shader);
	}
}

void EnemyManager::Register(Enemy* enemy)
{
	enemies.emplace_back(enemy);
}

void EnemyManager::Clear()
{
	for (auto& enemy : enemies)
	{
		delete enemy;
	}
}

void EnemyManager::DrawDebugPrimitive()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugPrimitive();
	}
}
