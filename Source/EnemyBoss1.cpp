#include "EnemyBoss1.h"
#include "Input\Input.h"
#include "Stage.h"

EnemyBoss1::EnemyBoss1(Character* p) :
	Enemy(p)
{
	model = std::make_unique<Model>("Data/Model/Jammo/Jammo.mdl");

	//ƒXƒP[ƒ‹‚Ì’²®
	scale.x = scale.y = scale.z = 0.1f;

	height = 1.0f;
	enemyType = ENEMY_TYPE::BOSS1;
	actMax = 3;
	actNo = 0;
	state = State::Act_Init;
	moveMax = 2;
	attackPower = 7;
	attackAdjacentRange = 2;
	isActEnd = false;
	health = 100;
	SetDirection(CommonClass::DirectionFace::BackRight);
}

bool EnemyBoss1::ChooseTargetMove(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (IsMoving()) return true;

	bool isMove = false;
	if (gamePad.GetButtonDown() & gamePad.BTN_UP)
	{
		SetTargetMoveTranslation({ 0, -1 });
		isMove = true;
	}
	if (gamePad.GetButtonDown() & gamePad.BTN_DOWN)
	{
		SetTargetMoveTranslation({ 0, 1 });
		isMove = true;
	}
	if (gamePad.GetButtonDown() & gamePad.BTN_LEFT)
	{
		SetTargetMoveTranslation({ -1, 0 });
		isMove = true;
	}
	if (gamePad.GetButtonDown() & gamePad.BTN_RIGHT)
	{
		SetTargetMoveTranslation({ 1, 0 });
		isMove = true;
	}
	if (isMove)
	{
		Stage::Instance()->ResetAllSquare();
	}
	return isMove;
}

void EnemyBoss1::UpdateState(float elapsedTime)
{
	ChooseTargetMove(elapsedTime);
	state = State::Act_Finish;
	isActEnd = true;
}

State EnemyBoss1::ChooseAct(float elapsedTime)
{
	return State();
}

void EnemyBoss1::InitializeAttack(float elapsedTime)
{

}
