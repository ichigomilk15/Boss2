#include "Audio.h"
#include "AudioLoader.h"

const std::vector<AudioLoader::AUDIO_INFO> AudioLoader::audioInfo
{
    // BGM
    /*{AUDIO::BGM_GAME,       "Data/Audio/BGM/game.wav"   , 0.25f ,true},
    {AUDIO::BGM_GAMEOVER,      "Data/Audio/BGM/gameover.wav"  , 0.2f  ,true},
    {AUDIO::BGM_GAMECLEAR,      "Data/Audio/BGM/gameclear.wav"  , 0.2f  ,true},*/

    // SE
    {AUDIO::SE_PLAYER_ATTACK,  "Data/Audio/SE/Player/attack.wav"   , 0.2f},
    {AUDIO::SE_PLAYER_DAMAGE,  "Data/Audio/SE/Player/damage.wav"   , 0.2f},
    {AUDIO::SE_PLAYER_DEAD,  "Data/Audio/SE/Player/dead.wav"   , 0.2f},
    {AUDIO::SE_PLAYER_SHIELD,  "Data/Audio/SE/Player/shield.wav"   , 0.2f},
    {AUDIO::SE_PLAYER_MOVE,  "Data/Audio/SE/Player/move.wav"   , 0.2f},

    {AUDIO::SE_MINION1_ATTACK,  "Data/Audio/SE/EnemyMinion1/attack.wav"   , 0.2f},
    {AUDIO::SE_MINION1_DAMAGE,  "Data/Audio/SE/EnemyMinion1/damage.wav"   , 0.2f},
    {AUDIO::SE_MINION1_DEATH,  "Data/Audio/SE/EnemyMinion1/death.wav"   , 0.2f},
    {AUDIO::SE_MINION1_WALK,  "Data/Audio/SE/EnemyMinion1/walk.wav"   , 0.2f},

    {AUDIO::SE_BOSS1_START,  "Data/Audio/SE/Boss1/start.wav"   , 0.2f},
    {AUDIO::SE_BOSS1_ATTACKLINE,  "Data/Audio/SE/Boss1/Attack/line.wav"   , 0.2f},
    {AUDIO::SE_BOSS1_BUMPATTACK,  "Data/Audio/SE/Boss1/Attack/bumpAttack.wav"   , 0.2f},
    {AUDIO::SE_BOSS1_JUMPATTACK,  "Data/Audio/SE/Boss1/Attack/jumpAttack.wav"   , 0.2f},
    {AUDIO::SE_BOSS1_DAMAGE,  "Data/Audio/SE/Boss1/damage.wav"   , 0.2f},
    {AUDIO::SE_BOSS1_DEATH,  "Data/Audio/SE/Boss1/death.wav"   , 0.2f},
    {AUDIO::SE_BOSS1_ANGRY,  "Data/Audio/SE/Boss1/angry.wav"   , 0.2f},
    {AUDIO::SE_BOSS1_PANIC,  "Data/Audio/SE/Boss1/panic.wav"   , 0.2f},
    {AUDIO::SE_BOSS1_WALLHIT,  "Data/Audio/SE/Boss1/wallHit.wav"   , 0.2f},

    //{AUDIO::SE_SELECT,  "Data/Audio/SE/select.wav"   , 0.4f},
};


void AudioLoader::Load(int trackNo, std::unique_ptr<AudioSource>& source)
{
    source = Audio::Instance().LoadAudioSource(audioInfo.at(trackNo).AUDIO_NAME, audioInfo.at(trackNo).LOOP);
    source.get()->SetVolume(audioInfo.at(trackNo).VOLUME);
}

AudioManager::AudioManager()
{
    //AudioLoader::Load(AUDIO::SE_CHOICE, clickSe);
}
