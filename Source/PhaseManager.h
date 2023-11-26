#pragma once
#include <d3d11.h>

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
	PhaseManager() = default;
	~PhaseManager() = default;
public:
	static PhaseManager& Instance()noexcept { static PhaseManager instance; return instance; }

	void Initialize();
	void Update(float elapsedTime);
	void ResetTurn(); //ターンのリセット
	void Reset(); //ゲームのリセット
	void DrawDebugGUI();

	//Getter&Setter*****************************************************************************
#if 1
	const Phase& GetFhase()const noexcept { return static_cast<Phase>(phase); }
	const unsigned int& GetTrunCount()const noexcept { return turnCount; }
	const void StepupUseCardIndex() { ++useCardIndex; }
	const unsigned int GetUseCardIndex()const noexcept { return useCardIndex; }
#endif // 1
	//Getter&Setter*****************************************************************************

private:
	void SetGameStart();
	void UpdatePlayerAct(float elapsedTime);
private:
	Phase phase = Phase::Phase_GameStart_Init;
	int StageLevel = 0;
	unsigned int turnCount = 0u;
	float phaseTimer;
	unsigned int useCardIndex = 0u; //プレイヤーが今使われているカード順番
};
