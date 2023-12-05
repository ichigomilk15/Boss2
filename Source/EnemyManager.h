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
    //�C���X�^���X
    static EnemyManager& Instance()
    {
        static EnemyManager instance;
        return instance;
    }

    //�X�V����
    void Update(float elapsedTime, Character* player);

    //�`�揈��
    void Render(ID3D11DeviceContext* context, Shader* shader);
    void Render2D(ID3D11DeviceContext* dc);

    //�G�l�~�[�o�^
    void Register(Enemy* enemy);

    //�G�l�~�[�S�폜
    void Clear();

    //�G�l�~�[���擾
    int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

    //�G�l�~�[�擾
    Enemy* GetEnemy(int index) { return enemies.at(index); }
    //���X�g�擾
    auto GetList() { return enemies; }

    void Remove(Enemy* enemy);

    /// <summary>
    /// �G�l�~�[�̐���
    /// </summary>
    /// <typeparam name="T">��������G�l�~�[�̃N���X</typeparam>
    /// <param name="player">�v���C���[�̃|�C���^</param>
    /// <returns>�쐬�����G�l�~�[�̃|�C���^</returns>
    template<typename T>
    Enemy* Spawn(Character* player)
    {
        Enemy* enemy = new T(player);
        this->Register(enemy);

        return enemy;
    }

    void DrawDebugGUI();

    void ResetTurnEnemies(); //�^�[���I���ɓG�����Z�b�g

    const bool GetIsAllActEnd() const;
    const bool GetIsAllDead() const;


    void SetStartEnemyNum()noexcept { startEnemyNum = static_cast<int>(enemies.size()); }
private:
    void UpdateEnemiesTurn(float elapsedTime); //�G�̃^�[���X�V����

private:
    std::vector<Enemy*> enemies;
    std::set<Enemy*>    removes;
    static int enemyTurnIndex; //�G�̍s������
    unsigned int startEnemyNum = 0u;
};
