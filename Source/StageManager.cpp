#include "StageManager.h"

void StageManager::Update(float elapsedTime)
{
	for (auto& stage : stages)
	{
		stage->Update(elapsedTime);
	}
}

void StageManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (auto& stage : stages)
	{
		stage->Render(dc, shader);
	}
}

void StageManager::Register(Stage* stage)
{
	stages.emplace_back(stage);
}

void StageManager::Clear()
{
	for (auto& stage : stages)
	{
		delete stage;
	}
	stages.clear();
}

bool StageManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	bool result = false;
	hit.distance = FLT_MAX;

	HitResult findClosest;
	for (auto& stage : stages)
	{
		if (stage->RayCast(start, end, findClosest))
		{
			if (findClosest.distance < hit.distance)
			{
				hit = findClosest;
				result = true;
			}
		}
	}

	return result;
}
