#pragma once
#include <d3d11.h>

#define PhaseCreate(name)	Phase_##name##_Init,\
							Phase_##name

class PhaseManager
{
public://class
	enum class Phase : int
	{
		PhaseCreate(GameStart),
		PhaseCreate(NextStage),
		PhaseCreate(Start),
		PhaseCreate(Player),
		PhaseCreate(Enemy),
		PhaseCreate(End),
		Phase_Max,
	};
private://constructer
	PhaseManager() = default;
	~PhaseManager() = default;
public:
	static PhaseManager& Instance()noexcept { static PhaseManager instance; return instance; }

	void Initialize();
	void Update(float elapsedTime);
	void Reset();
	void DrawDebugGUI();

//Getter&Setter*****************************************************************************
#if 1
	const Phase& GetFhase()const noexcept { return static_cast<Phase>(phase); }
	const unsigned int& GetTrunCount()const noexcept { return trunCount; }
#endif // 1
//Getter&Setter*****************************************************************************
private:
	Phase phase = Phase::Phase_GameStart_Init;
	int StageLevel = 0;
	unsigned int trunCount = 0u;
	float phaseTimer;
};

#undef PhaseCreate