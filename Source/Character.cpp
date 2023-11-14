#include "Common.h"
#include "Character.h"
#include <CustomMathf.h>

void Character::UpdateTransform()
{
	//�X�P�[���s��
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//��]�s����쐬
	//DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
	DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
	DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
	DirectX::XMMATRIX R = Y * X * Z;
	//�ʒu�s������
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(positionWorld.x, positionWorld.y, positionWorld.z);

	//�O�̍s����g�ݍ��킹, ���[���h�s����쐬
	DirectX::XMMATRIX W = S * R * T;

	//�v�Z�������[���h�s������o��
	DirectX::XMStoreFloat4x4(&transform, W);
}

void Character::UpdateVelocity(float elapsedTime)
{
	// �o�߃t���[��
	float elapsedFrame = 60.0f * elapsedTime;

	if (movePosX != 0)
	{
		this->position.x += movePosX;
		this->positionWorld.x += movePosX * Common::SquareWidth;
	}
	if (movePosY != 0)
	{
		this->position.y += movePosY;
		this->positionWorld.z += movePosY * Common::SquareHeight;
	}

#if 0
	//�������͍X�V����
	UpdateVerticalVelocity(elapsedFrame);
	//�������͍X�V����
	UpdateHorizontalVelocity(elapsedFrame);

	//�����ړ��X�V����
	UpdateVerticalMove(elapsedTime);
	//�����ړ��X�V����
	UpdateHorizontalMove(elapsedTime);
#endif
}

bool Character::ApplyDamage(int damage)
{
	//�_���[�W���O�̏ꍇ�͌��N��Ԃ�ύX����K�v���Ȃ�
	if (damage <= 0) return false;
	//���S���Ă���ꍇ�͌��N��Ԃ�ύX���Ȃ�
	if (health <= 0) return false;

	//�_���[�W����
	health -= damage;

	//���S�ʒm
	if (health <= 0)
	{
		OnDead();
	}
	//�_���[�W�ʒm
	else
	{
		OnDamaged();
	}

	//���N��Ԃ��ύX�����ꍇ��true��Ԃ�
	return true;
}

void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
	//���͂ɗ͂�^����
	velocity.x += impulse.x;
	velocity.y += impulse.y;
	velocity.z += impulse.z;
}

// �������͍X�V����
void Character::UpdateVerticalVelocity(float elapsedFrame)
{
	// �d�͏���
	//velocity.y += gravity * elapsedFrame;
}

//�����ړ��X�V����
void Character::UpdateVerticalMove(float elapsedTime)
{
	//�撼�����̈ړ���
	float my = velocity.y * elapsedTime;

	// �L�����N�^�[��Y�������ƂȂ�@���x�N�g��
	DirectX::XMFLOAT3 normal = { 0, 1, 0 };

	//������
	if (my < 0.0f)
	{
		//���C�̊J�n�ʒu�͑�����菭����
		DirectX::XMFLOAT3 start = { positionWorld.x, positionWorld.y, positionWorld.z };
		//���C�̏I�_�ʒu�͈ړ���̈ʒu
		DirectX::XMFLOAT3 end = { positionWorld.x, positionWorld.y + my, positionWorld.z };

	}
	//�㏸��
	else if (my > 0.0f)
	{
		positionWorld.y += my;
		//isGround = false;
	}

	//�n�ʂ̌����ɉ����悤��XZ����]
	{
		// Y�����@���x�N�g�������Ɍ����I�C���[�p��]���Z�o����
		float ax = atan2f(normal.z, normal.y);
		float az = -atan2f(normal.x, normal.y);

		//���`�⊮�Ŋ��炩�Ȃɉ�]����
		angle.x = CustomMathf::Lerp(angle.x, ax, 0.2f);
		angle.z = CustomMathf::Lerp(angle.z, az, 0.2f);
		/*angle.x = ax;
		angle.z = az;*/

		groundNormal = normal;
	}
}

//�������͍X�V����
void Character::UpdateHorizontalVelocity(float elapsedFrame)
{
}

void Character::UpdateHorizontalMove(float elapsedTime)
{
	// �ړ�����
	/*positionWorld.x += velocity.x * elapsedTime;
	positionWorld.z += velocity.z * elapsedTime;*/

	//�������͗ʌv�Z
	float velocityLengthXZ = sqrtf(velocity.z * velocity.z + velocity.x * velocity.x);
	if (velocityLengthXZ > 0.0f)
	{
		//�����ړ��l
		float mx = velocity.x * elapsedTime;
		float mz = velocity.z * elapsedTime;

		//���C�̊J�n�ʒu�ƏI�_�ʒu
		DirectX::XMFLOAT3 start = { positionWorld.x, positionWorld.y, positionWorld.z };
		DirectX::XMFLOAT3 end = { positionWorld.x + mx, positionWorld.y, positionWorld.z + mz };

		//���C�L���X�g�ɂ��ǔ���
	}

}

void Character::Move(int vx, int vy)
{
	//�ړ������x�N�g����ݒ�
	movePosX = vx;
	movePosY = vy;
}
