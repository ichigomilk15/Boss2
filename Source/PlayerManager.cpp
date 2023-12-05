#include "PlayerManager.h"
#include "Stage.h"

void PlayerManager::Update(float elapsedTime)
{
	// プレイヤーの更新
	for (Player* player : players)
	{
		player->Update(elapsedTime);
	}
}

void PlayerManager::Render(ID3D11DeviceContext* context, Shader* shader, RenderContext& rc)
{
	for (Player* player : players)
	{
		player->Render(context, shader);
	}
}

void PlayerManager::Render2D(ID3D11DeviceContext* dc)
{
	
}

void PlayerManager::Register(Player* player)
{
	players.emplace_back(player);
}


void PlayerManager::Clear()
{
	for (Player* player : players)
	{
		delete player;
	}
	players.clear();
}

void PlayerManager::DrawDebugGUI()
{
	if (players.size() == 0u)return;
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Players", nullptr, ImGuiWindowFlags_None))
	{
		//プレイヤー数
		int playerNum = static_cast<int>(players.size());
		ImGui::InputInt("playerNum", &playerNum, 0);

		//各プレイヤーのデバック呼び出し
		for (auto& player : players)
		{
			player->DrawDebugGUI();
		}
	}
	ImGui::End();
}

void PlayerManager::Remove(Player* player)
{
	// 破棄リストに追加
	removes.emplace_back(player);
}