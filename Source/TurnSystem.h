#pragma once
#include <Character.h>

class TurnSystem
{
public: //Enum �錾
	enum class TURN_ENUM
	{
		ENEMY_TURN,
		PLAYER_TURN,
		MAX_TURN,
	};
public: //Function
	TurnSystem() :
		//who(nullptr),
		subject(TURN_ENUM::ENEMY_TURN),
		turnNum(0)
	{}
	~TurnSystem() = default;

	void ChangeTurn();

	//Setter & Getter
#if 1
	//void setWho(Character* chara) { who = chara; }
	//const Character* getWho() const { return who; }
	void setSubject(TURN_ENUM sub) { subject = sub; }
	const TURN_ENUM getSubject() const { return subject; }
	const unsigned int getTurnNum() const { return turnNum; }
#endif

private: //Function Private
	void ChangeToPlayerTurn();
	void ChangeToEnemyTurn();

private: //Member
	TURN_ENUM subject; //�G���v���C���[�^�[���̔���
	//Character* who; //�N�̃^�[��
	unsigned int turnNum; //�^�[����
};