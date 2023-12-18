#pragma once
#include <d3d11.h>
#include <memory>
#include "Audio\AudioSource.h"
#include "Graphics/Sprite.h"
#include "UI.h"
#include "HitCollisions.h"
#include "Stage.h"

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
	void ResetTurn(); //ターンのリセット
	void Reset(); //ゲームのリセット
	void Render(ID3D11DeviceContext* dc);
	void RenderWaveChange(ID3D11DeviceContext* dc)const;
	void DrawDebugGUI();

	//Getter&Setter*****************************************************************************
#if 1
	const Phase GetFhase()const noexcept { 
		return static_cast<Phase>(phase); }
	const unsigned int GetTrunCount()const noexcept { return turnCount; }
	void SetTurnCount(const unsigned int turn) { turnCount = turn; }
	const void StepupUseCardIndex() { ++useCardIndex; }
	const unsigned int GetUseCardIndex()const noexcept { return useCardIndex; }
#endif // 1
	//Getter&Setter*****************************************************************************

private:
	void SetGameStart();
	void UpdatePlayerAct(float elapsedTime);
	void NextPhase();
	void ChangePhase(const Phase&& next)noexcept;

	//一定時間条件を満たし続けていたらtrueを返す
	const bool IsSlowNextPhase(const bool flag);
	//条件を満たした瞬間にtrueを返す
	const bool IsQuickNextPhase(const bool flag);

	void StageInit(const int level);

	void InitializeAudio();

	void StartWaveChange(float startTime = .0f);
public:
	static constexpr float NEXT_PHASE_WAIT_TIMER = 1.0f;
private:
	Phase phase = Phase::Phase_GameStart_Init;
	int turnCount = 0u;
	float phaseTimer = -1.0f;
	bool isNextPhase = false;
	unsigned int useCardIndex = 0u;
	std::unique_ptr<Sprite> waveSprites[Stage::STAGE_LEVEL_MAX];

	HitBox2D okButtonCollision;
	std::unique_ptr<Sprite> okButton;

	struct 
	{
		const float TIMER_MAX = 2.0f;
		int DepthNum = 5;
		std::unique_ptr<Sprite> sprite;
		float offsetY = .0f;
		float timer = 0.f;
		float scale = .0f;
		DirectX::XMFLOAT2 topPos = {};
		bool isChangeOk = true;
		bool isLoadOk;
		bool oldIsLoadOk;
	}waveChangeData;
};