#include "EnemyManager.h"


EnemyManager::EnemyManager()
{
}

void EnemyManager::Update(float elapsedTime, Character* player)
{
	for (auto&& enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}

	//”jŠüˆ—
	for (auto&& erase : this->removes)
	{
		auto&& it = std::find(enemies.begin(), enemies.end(), erase);

		if (it != enemies.end())
		{
			enemies.erase(it);
		}
		delete erase;
	}
	this->removes.clear();
}

void EnemyManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (auto&& enemy : enemies)
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
	for (auto&& enemy : this->enemies)
	{
		delete enemy;
		enemy = nullptr;
	}
	enemies.clear();
}

void EnemyManager::Remove(Enemy* enemy)
{
	//”jŠüƒŠƒXƒg‚É’Ç‰Á
	removes.insert(enemy);
}

void EnemyManager::DrawDebugGUI()
{
}
