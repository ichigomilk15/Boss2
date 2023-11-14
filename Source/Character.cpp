#include "Character.h"
#include "StageManager.h"
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
		this->positionWorld.x += movePosX * 1.0f;
	}
	if (movePosY != 0)
	{
		this->position.y += movePosY;
		this->positionWorld.z += movePosY * 1.0f;
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

		//���C�L���X�g�ɂ��n�ʔ���
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			normal = hit.normal;

			//�n�ʂɐڒn���Ă���
			//positionWorld.y = hit.positionWorld.y;
			positionWorld = hit.positionWorld;

			//��]
			//this->angle.x += hit.rotation.x;
			this->angle.y += hit.rotation.y;
			//this->angle.z += hit.rotation.z;
			
			// �X�Η��̌v�Z
			float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);

			//���n����
			/*if (!isGround)
			{
				OnLanding();
			}*/
			//isGround = true;
			velocity.y = 0.0f;
		}
		else
		{
			//�󒆂ɕ����Ă���
			positionWorld.y += my;
			//isGround = false;
		}
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
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			//�ǂ܂ł̃x�N�g��
			DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(/*&start*/&hit.positionWorld);
			DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

			//�ǂ̖@��w
			DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

			//���˃x�N�g����@���Ɏˉe
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);
			Dot = DirectX::XMVectorScale(Dot, 1.1f);
			float dot = DirectX::XMVectorGetX(Dot);

			//�␳�ʒu�̌v�Z
			DirectX::XMVECTOR Pos = DirectX::XMVectorAdd(End, DirectX::XMVectorScale(Normal, dot));
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, Pos);

			HitResult hitSecondWall;
			if (StageManager::Instance().RayCast(start, pos, hitSecondWall))
			{
				DirectX::XMVECTOR Line = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&hitSecondWall.positionWorld), Start);

				auto CorrectPos = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&hit.positionWorld), DirectX::XMVectorScale(Line, 0.2f));
				DirectX::XMFLOAT3 correctPos;
				DirectX::XMStoreFloat3(&correctPos, CorrectPos);
				positionWorld.x = correctPos.x;
				positionWorld.z = correctPos.z;
			}
			else
			{
				positionWorld.x = DirectX::XMVectorGetX(Pos);
				positionWorld.z = DirectX::XMVectorGetZ(Pos);
			}
		}
		else
		{
			//�ړ�
			positionWorld.x += mx;
			positionWorld.z += mz;
		}
	}

}

void Character::Move(int vx, int vy)
{
	//�ړ������x�N�g����ݒ�
	movePosX = vx;
	movePosY = vy;
}
