#pragma once
#include "Player.h"

class PlayerManager
{
private:
	PlayerManager() {}
	~PlayerManager() {}
public:

	// 唯一のインスタンス取得
	inline static PlayerManager& Instance()noexcept
	{
		static PlayerManager instance;
		return instance;
	}

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(ID3D11DeviceContext* context, Shader* shader, RenderContext& rc);

	// プレイヤー登録
	void Register(Player* player);

	// 初期化
	void Clear();

	// デバッグ情報表示
	void DrawDebugGUI();

	// プレイヤー数取得
	inline const int GetPlayerCount()const noexcept { return static_cast<int>(players.size()); }

	// プレイヤー取得
	inline Player* GetPlayer(int index)const noexcept { return players.at(index); }
	//プレイヤーリスト取得
	inline std::vector<Player*>* GetPlayerVector()noexcept { return &players; }

	inline Player* GetFirstPlayer()const noexcept { return players.at(0); }

	// プレイヤー削除
	void Remove(Player* player);

private:
	std::vector<Player*> players;		//playerの格納		0番は常に操作用プレイヤー
	std::vector<Player*> removes;		//削除待ちplaeyrの格納
};

