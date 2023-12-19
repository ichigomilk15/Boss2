#include "EnemyManager.h"
#include "PhaseManager.h"
#include "PlayerManager.h"

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

void EnemyManager::Render(ID3D11DeviceContext* context, Shader* shader, RenderContext& rc)
{
	for (auto&& enemy : enemies)
	{
		enemy->Render(context, shader, rc);
	}
}

void EnemyManager::Render2D(ID3D11DeviceContext* dc)
{
	const DirectX::XMFLOAT2 ScreenSize = Graphics::Instance().GetScreenSize();
	const DirectX::XMFLOAT2 AreaTopLeft = { ScreenSize.x * 0.3f,.0f };
	DirectX::XMFLOAT2 AreaSize = { ScreenSize.x * 0.5f,ScreenSize.y * 0.1f };
	DirectX::XMFLOAT2 pos = AreaTopLeft;
	
	float SumScale = 0;
	for (auto& enemy : enemies)
		SumScale += enemy->GetHpBarUseScale();

	for (auto& enemy : enemies)
	{
		DirectX::XMFLOAT2 size = { AreaSize.x * 
			(std::min)(enemy->GetHpBarUseScale() / SumScale,enemy->GetHpBarUseScale()),AreaSize.y };
		enemy->Render2D(dc, HitBox2D::CreateBoxFromTopLeft(pos, size));
		
		pos.x += size.x;
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
	startEnemyNum = 0u;
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
		int i = 0;
		for (auto& e : enemies)
		{
			std::string str = "enemy";
			str += '0' + i++;
			if (ImGui::CollapsingHeader(str.c_str(), 0))
			{
				ImGui::BeginGroup();
				e->DrawDebugGUI();
				ImGui::EndGroup();
			}

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
		if (!enemy->GetIsDead())
			return false;
	}
	return true;
}

const bool EnemyManager::GetIsAllDestroyed() const
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
	if (PhaseManager::Instance().GetFhase() != PhaseManager::Phase::Phase_EnemyAct || PlayerManager::Instance().GetFirstPlayer()->GetIsDead())
		return;

	if (enemyTurnIndex == 1)
		int a = 1;

	if (enemyTurnIndex < 0)
	{
		enemyTurnIndex = 0;
		if (enemies.at(enemyTurnIndex)->GetState() == State::Idle)
		{
			enemies.at(enemyTurnIndex)->SetState(State::Act_Init);
		}
		else
		{
			enemies.at(enemyTurnIndex)->SetState((State)((int)enemies.at(enemyTurnIndex)->GetState() - 1));
		}
	}

	if (enemies.at(enemyTurnIndex)->GetIsActEnd())
	{
		if (++enemyTurnIndex < enemies.size())
		{
			if (enemies.at(enemyTurnIndex)->GetState() == State::Idle)
			{
				enemies.at(enemyTurnIndex)->SetState(State::Act_Init);
			}
			else
			{
				enemies.at(enemyTurnIndex)->SetState((State)((int)enemies.at(enemyTurnIndex)->GetState() - 1));
			}
		}
	}
}
