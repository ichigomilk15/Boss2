#pragma once
#include "Player.h"

class PlayerManager
{
private:
	PlayerManager() {}
	~PlayerManager() {}
public:

	// �B��̃C���X�^���X�擾
	inline static PlayerManager& Instance()noexcept
	{
		static PlayerManager instance;
		return instance;
	}

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Render(ID3D11DeviceContext* context, Shader* shader, RenderContext& rc);

	// �v���C���[�o�^
	void Register(Player* player);

	// ������
	void Clear();

	// �f�o�b�O���\��
	void DrawDebugGUI();

	// �v���C���[���擾
	inline const int GetPlayerCount()const noexcept { return static_cast<int>(players.size()); }

	// �v���C���[�擾
	inline Player* GetPlayer(int index)const noexcept { return players.at(index); }
	//�v���C���[���X�g�擾
	inline std::vector<Player*>* GetPlayerVector()noexcept { return &players; }

	inline Player* GetFirstPlayer()const noexcept { return players.at(0); }

	// �v���C���[�폜
	void Remove(Player* player);

private:
	std::vector<Player*> players;		//player�̊i�[		0�Ԃ͏�ɑ���p�v���C���[
	std::vector<Player*> removes;		//�폜�҂�plaeyr�̊i�[
};

