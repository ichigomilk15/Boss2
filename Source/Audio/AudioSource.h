#pragma once

#include <memory>
#include <xaudio2.h>
#include "Audio/AudioResource.h"

// �I�[�f�B�I�\�[�X
class AudioSource
{
public:
	AudioSource(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource, bool loop);
	~AudioSource();

	// �Đ�
	void Play(bool loop);

	// ��x��~���Ă���Đ�����
	void ResetPlay();

	// ��~
	void Stop();

	// �Đ����ł��邩
	bool Queuing();

	// �{�����[���ݒ�
	void SetVolume(float volume) { this->volume = volume; }

private:
	IXAudio2SourceVoice* sourceVoice = nullptr;
	XAUDIO2_BUFFER					buffer = { 0 };
	std::shared_ptr<AudioResource>	resource;
	float volume = 1.0f;
};
