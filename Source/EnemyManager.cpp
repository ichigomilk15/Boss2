#include "EnemyManager.h"
#include "PhaseManager.h"

int EnemyManager::enemyTurnIndex = -1;

EnemyManager::EnemyManager()
{
}

void EnemyManager::Update(float elapsedTime, Character* player)
{
	UpdateEnemiesTurn(elapsedTime);

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
	ImGui::SetNextWindowPos(ImVec2(10, 360), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Enemy", nullptr, ImGuiWindowFlags_None))
	{
		for (auto& e : enemies)
		{
			e->DrawDebugGUI();
		}
	}
	ImGui::End();
}

void EnemyManager::ResetTurnEnemies()
{
	enemyTurnIndex = -1;
	for (auto& e : enemies)
	{
		e->ResetStatus();
	}
}

const bool EnemyManager::GetIsAllActEnd() const
{
	for (auto& enemy : enemies)
	{
		if (!enemy->GetIsActEnd())
			return false;
	}
	enemyTurnIndex = -1;
	return true;
}

const bool EnemyManager::GetIsAllDead() const
{
	for (auto& enemy : enemies)
	{
		if (!enemy->GetIsDestroyed())
			return false;
	}
	return true;
}

void EnemyManager::UpdateEnemiesTurn(float elapsedTime)
{
	if (PhaseManager::Instance().GetFhase() != PhaseManager::Phase::Phase_EnemyAct)
		return;

	if (enemyTurnIndex < 0)
	{
		enemyTurnIndex = 0;
		enemies.at(enemyTurnIndex)->SetState(State::Act_Init);
	}

	if (enemies.at(enemyTurnIndex)->GetIsActEnd())
	{
		if (++enemyTurnIndex < enemies.size())
			enemies.at(enemyTurnIndex)->SetState(State::Act_Init);
	}
}
