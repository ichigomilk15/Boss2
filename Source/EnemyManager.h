#pragma once

#include <vector>
#include <set>
#include "Common.h"
#include "Enemy.h"
#include "Character.h"

class EnemyManager
{
private:
    EnemyManager();
    ~EnemyManager() {}

public:
    //インスタンス
    static EnemyManager& Instance()
    {
        static EnemyManager instance;
        return instance;
    }

    //更新処理
    void Update(float elapsedTime, Character* player);

    //描画処理
    void Render(ID3D11DeviceContext* context, Shader* shader);
    void Render2D(ID3D11DeviceContext* dc);

    //エネミー登録
    void Register(Enemy* enemy);

    //エネミー全削除
    void Clear();

    //エネミー数取得
    int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

    //エネミー取得
    Enemy* GetEnemy(int index) { return enemies.at(index); }
    //リスト取得
    auto GetList() { return enemies; }

    void Remove(Enemy* enemy);

    /// <summary>
    /// エネミーの生成
    /// </summary>
    /// <typeparam name="T">生成するエネミーのクラス</typeparam>
    /// <param name="player">プレイヤーのポインタ</param>
    /// <returns>作成したエネミーのポインタ</returns>
    template<typename T>
    Enemy* Spawn(Character* player)
    {
        Enemy* enemy = new T(player);
        this->Register(enemy);

        return enemy;
    }

    void DrawDebugGUI();

    void ResetTurnEnemies(); //ターン終了に敵をリセット

    const bool GetIsAllActEnd() const;
    const bool GetIsAllDead() const;


    void SetStartEnemyNum()noexcept { startEnemyNum = static_cast<int>(enemies.size()); }
private:
    void UpdateEnemiesTurn(float elapsedTime); //敵のターン更新処理

private:
    std::vector<Enemy*> enemies;
    std::set<Enemy*>    removes;
    static int enemyTurnIndex; //敵の行動順番
    unsigned int startEnemyNum = 0u;
};
