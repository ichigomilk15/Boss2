#pragma once

#include <memory>
#include <xaudio2.h>
#include "Audio/AudioResource.h"

// オーディオソース
class AudioSource
{
public:
	AudioSource(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource, bool loop);
	~AudioSource();

	// 再生
	void Play(bool loop);

	// 一度停止してから再生する
	void ResetPlay();

	// 停止
	void Stop();

	// 再生中であるか
	bool Queuing();

	// ボリューム設定
	void SetVolume(float volume) { this->volume = volume; }

private:
	IXAudio2SourceVoice* sourceVoice = nullptr;
	XAUDIO2_BUFFER					buffer = { 0 };
	std::shared_ptr<AudioResource>	resource;
	float volume = 1.0f;
};
