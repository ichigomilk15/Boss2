#pragma once
#include <d3d11.h>
#include <memory>
#include "Graphics/Sprite.h"
#include "HitCollisions.h"

#define PHASE_LIST \
	X(Phase_GameStart_Init)   \
	X(Phase_GameStart)   \
	X(Phase_NextStage_Init)   \
	X(Phase_NextStage)   \
	X(Phase_Start_Init)   \
	X(Phase_Start)   \
	X(Phase_Player_Init)   \
	X(Phase_Player)   \
	X(Phase_PlayerAct_Init)   \
	X(Phase_PlayerAct)   \
	X(Phase_Enemy_Init)   \
	X(Phase_Enemy)   \
	X(Phase_EnemyAct_Init) \
	X(Phase_EnemyAct) \
	X(Phase_End_Init)   \
	X(Phase_End)   \

class PhaseManager
{
public://class
	enum class Phase : int
	{
#define X(name) name,
		PHASE_LIST
#undef X
		Phase_Max,
	};
private://constructer
	PhaseManager();
	~PhaseManager() = default;
	PhaseManager(PhaseManager&) = delete;
	PhaseManager operator=(PhaseManager&) = delete;
public:
	static PhaseManager& Instance()noexcept { static PhaseManager instance; return instance; }

	void Initialize();
	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc);
	void Reset();
	void DrawDebugGUI();

	//Getter&Setter*****************************************************************************
#if 1
	const Phase& GetFhase()const noexcept { return static_cast<Phase>(phase); }
	const unsigned int& GetTrunCount()const noexcept { return trunCount; }
#endif // 1
	//Getter&Setter*****************************************************************************

private:
	void SetGameStart();
	void UpdatePlayerAct(float elapsedTime);
	void NextPhase();

	//ˆê’èŠÔğŒ‚ğ–‚½‚µ‘±‚¯‚Ä‚¢‚½‚çtrue‚ğ•Ô‚·
	const bool IsSlowNextPhase(const bool flag);
	//ğŒ‚ğ–‚½‚µ‚½uŠÔ‚Étrue‚ğ•Ô‚·
	const bool IsQuickNextPhase(const bool flag);
public:
	static constexpr float NEXT_PHASE_WAIT_TIMER = 1.0f;
private:
	Phase phase = Phase::Phase_GameStart_Init;
	unsigned int trunCount = 0u;
	float phaseTimer = -1.0f;
	bool isNextPhase = false;

	HitBox2D okButtonCollision;
	std::unique_ptr<Sprite> okButton;
};