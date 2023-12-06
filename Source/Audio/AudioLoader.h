#pragma once
#include <memory>
#include <vector>
#include "AudioSource.h"

enum AUDIO
{
	// BGM
	/*BGM_GAME = 0,
	BGM_GAMEOVER,
	BGM_GAMECLEAR,*/

	// SE
	SE_PLAYER_ATTACK,
	SE_PLAYER_DAMAGE,
	SE_PLAYER_DEAD,
	SE_PLAYER_SHIELD,
	SE_PLAYER_MOVE,

	SE_MINION1_ATTACK,
	SE_MINION1_DAMAGE,
	SE_MINION1_DEATH,
	SE_MINION1_WALK,

	SE_BOSS1_START,
	SE_BOSS1_ATTACKLINE,
	SE_BOSS1_BUMPATTACK,
	SE_BOSS1_JUMPATTACK,
	SE_BOSS1_DAMAGE,
	SE_BOSS1_DEATH,
	SE_BOSS1_ANGRY,
	SE_BOSS1_PANIC,
	SE_BOSS1_WALLHIT,

	//SE_SELECT,

	MAX
};

class AudioLoader
{
public:
	struct AUDIO_INFO
	{
		int AUDIO_NO;
		const char* AUDIO_NAME;
		float VOLUME;
		bool LOOP = false;
	};
public:
	/// <summary>
	/// �I�[�f�B�I�ǂݍ���
	/// </summary>
	/// <param name="trackNo">�I�[�f�B�I�ԍ�</param>
	/// <param name="source">�ۑ�����|�C���^</param>
	static void Load(int trackNo, std::unique_ptr<AudioSource>& source);

private:
	static const std::vector<AUDIO_INFO> audioInfo;
};

// ���ʂŎg��SE�������Ă�
class AudioManager
{
	AudioManager();
	~AudioManager() {}
public:
	static AudioManager& Instance()
	{
		static AudioManager instance;
		return instance;
	}

	void PlayClickSe() { clickSe->ResetPlay(); }

private:
	std::unique_ptr<AudioSource> clickSe;			//�N���b�N��SE
};