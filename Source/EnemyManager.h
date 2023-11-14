#pragma once

#include <vector>
#include <set>
#include "Enemy.h"

class EnemyManager
{
private:
	EnemyManager() {}
	~EnemyManager() {}


public:
	static EnemyManager& Instance()
	{
		static EnemyManager instance;
		return instance;
	}

	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc, Shader* shader);
	void Register(Enemy* enemy);
	void Clear();
	void DrawDebugPrimitive();
	
	// �G�l�~�[���擾
	int GetEnemyCount() const { return static_cast<int> (enemies.size()); }

	// �G�l�~�[�擾
	Enemy* GetEnemy(int index) { return enemies.at(index); }

	void Remove(Enemy* enemy);

private:
	std::vector<Enemy*> enemies;
	std::set<Enemy*> removes;
};