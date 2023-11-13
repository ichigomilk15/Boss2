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
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//�O�̍s����g�ݍ��킹, ���[���h�s����쐬
	DirectX::XMMATRIX W = S * R * T;

	//�v�Z�������[���h�s������o��
	DirectX::XMStoreFloat4x4(&transform, W);
}

void Character::Jump(float speed)
{
	// ������̗͂�ݒ�
	velocity.y = speed;
}

void Character::UpdateVelocity(float elapsedTime)
{
	// �o�߃t���[��
	float elapsedFrame = 60.0f * elapsedTime;

	//�������͍X�V����
	UpdateVerticalVelocity(elapsedFrame);
	//�������͍X�V����
	UpdateHorizontalVelocity(elapsedFrame);

	//�����ړ��X�V����
	UpdateVerticalMove(elapsedTime);
	//�����ړ��X�V����
	UpdateHorizontalMove(elapsedTime);
}

bool Character::ApplyDamage(int damage, float invicibleTime)
{
	//�_���[�W���O�̏ꍇ�͌��N��Ԃ�ύX����K�v���Ȃ�
	if (damage <= 0) return false;
	//���S���Ă���ꍇ�͌��N��Ԃ�ύX���Ȃ�
	if (health <= 0) return false;

	if (invisibleTimer > 0) return false;

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
		invisibleTimer = invicibleTime;
		//UpdateInvisibleTimer(elapsedTime);
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
	velocity.y += gravity * elapsedFrame;
}

//�����ړ��X�V����
void Character::UpdateVerticalMove(float elapsedTime)
{
	//�撼�����̈ړ���
	float my = velocity.y * elapsedTime;
	slopeRate = 0.0f;

	// �L�����N�^�[��Y�������ƂȂ�@���x�N�g��
	DirectX::XMFLOAT3 normal = { 0, 1, 0 };

	//������
	if (my < 0.0f)
	{
		//���C�̊J�n�ʒu�͑�����菭����
		DirectX::XMFLOAT3 start = { position.x, position.y + stepOffSet, position.z };
		//���C�̏I�_�ʒu�͈ړ���̈ʒu
		DirectX::XMFLOAT3 end = { position.x, position.y + my, position.z };

		//���C�L���X�g�ɂ��n�ʔ���
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			normal = hit.normal;

			//�n�ʂɐڒn���Ă���
			//position.y = hit.position.y;
			position = hit.position;

			//��]
			//this->angle.x += hit.rotation.x;
			this->angle.y += hit.rotation.y;
			//this->angle.z += hit.rotation.z;
			
			// �X�Η��̌v�Z
			float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
			slopeRate = 1.0f - hit.normal.y / (normalLengthXZ + hit.normal.y);

			//���n����
			if (!isGround)
			{
				OnLanding();
			}
			isGround = true;
			velocity.y = 0.0f;
		}
		else
		{
			//�󒆂ɕ����Ă���
			position.y += my;
			isGround = false;
		}
	}
	//�㏸��
	else if (my > 0.0f)
	{
		position.y += my;
		isGround = false;
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
	// XZ���ʂ̑��͂���������
	float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (length != 0.0f)
		int i = 0;
	if (length > 0.0f)
	{
		float friction = this->friction * elapsedFrame;

		//�󒆂ɂ���Ƃ��͖��C�͂����炷
		//friction -= (isGround) ? 0 : airControl;
		if (isGround) friction *= airControl;

		//���C�ɂ�鉡�����̌�������
		if (length > friction)
		{
#if 0
			float length2 = length - friction;
			velocity.x *= length2 / length;
			velocity.z *= length2 / length;
			length = length2;
#else
			float vx = velocity.x / length;
			float vz = velocity.z / length;
			velocity.x -= vx * friction;
			velocity.z -= vz * friction;
#endif
		}
		//�������̑��͂����C�͈ȉ��ɂȂ����̂ő��͂𖳌���
		else
		{
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}

	//XZ���ʂ̑��͂���������
	if (length <= maxMoveSpeed)
	{
		//�ړ��x�N�g�����[���x�N�g���łȂ��Ȃ��������
		float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
		if (moveVecLength > 0.0f)
		{
			//������
			float acceleration = this->acceleration * elapsedFrame;

			//�󒆂ɂ���Ƃ��͉����͂����炷
			acceleration -= (isGround) ? 0 : airControl;

			//�ړ��x�N�g���ɂ���������
			//moveVecLength += acceleration;
			velocity.x += acceleration * moveVecX;
			velocity.z += acceleration * moveVecZ;

			//�ő呬�x����
			length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
			if (length > maxMoveSpeed)
			{
				//length = maxMoveSpeed;
				velocity.x /= length;
				velocity.z /= length;
				velocity.x *= maxMoveSpeed;
				velocity.z *= maxMoveSpeed;
			}

			//�����ŃK�^�K�^���Ȃ��悤�ɂ���
			if (isGround && slopeRate > 0.0f)
			{
				velocity.y -= length * slopeRate * elapsedFrame;
			}
		}
	}
	//�ړ��x�N�g�������Z�b�g
	moveVecX = 0.0f;
	moveVecZ = 0.0f;
}

void Character::UpdateHorizontalMove(float elapsedTime)
{
	// �ړ�����
	/*position.x += velocity.x * elapsedTime;
	position.z += velocity.z * elapsedTime;*/

	//�������͗ʌv�Z
	float velocityLengthXZ = sqrtf(velocity.z * velocity.z + velocity.x * velocity.x);
	if (velocityLengthXZ > 0.0f)
	{
		//�����ړ��l
		float mx = velocity.x * elapsedTime;
		float mz = velocity.z * elapsedTime;

		//���C�̊J�n�ʒu�ƏI�_�ʒu
		DirectX::XMFLOAT3 start = { position.x, position.y + stepOffSet, position.z };
		DirectX::XMFLOAT3 end = { position.x + mx, position.y + stepOffSet, position.z + mz };

		//���C�L���X�g�ɂ��ǔ���
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			//�ǂ܂ł̃x�N�g��
			DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(/*&start*/&hit.position);
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
				DirectX::XMVECTOR Line = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&hitSecondWall.position), Start);

				auto CorrectPos = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&hit.position), DirectX::XMVectorScale(Line, 0.2f));
				DirectX::XMFLOAT3 correctPos;
				DirectX::XMStoreFloat3(&correctPos, CorrectPos);
				position.x = correctPos.x;
				position.z = correctPos.z;
			}
			else
			{
				position.x = DirectX::XMVectorGetX(Pos);
				position.z = DirectX::XMVectorGetZ(Pos);
			}
		}
		else
		{
			//�ړ�
			position.x += mx;
			position.z += mz;
		}
	}

}

void Character::Move(float vx, float vz, float speed)
{
	/*speed *= elapsedTime;
	position.x += vx * speed;
	position.z += vz * speed;*/

	//�ړ������x�N�g����ݒ�
	moveVecX = vx;
	moveVecZ = vz;
	//�ő呬�x�ݒ�
	maxMoveSpeed = speed;
}

void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;

	//�i�s�x�N�g�����[���x�N�g���̏ꍇ�͏�������K�v�Ȃ�
	//if (vz <= 0.000000001f || vz >= -0.000000001f) return;
	//float temp = sqrtf(vx * vx + vz * vz);
	float length = sqrtf(vx * vx + vz * vz);
	if (length < 0.001f) return;

	//�i�s�x�N�g����P�ʃx�N�g����
	vx /= length;
	vz /= length;

	//���g�̉�]�l����O���������߂�
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	//��]�p�����߂邽�߁A�Q�̒P�ʃx�N�g���̓��ς��v�Z����
	float dot = vx * frontX + vz * frontZ;
	//float rot = cosf(dot);
	float rot = 1.0f - dot;

	//���ϒl��-1.0~1.0�ŕ\�������Ă���A�Q�̒P�ʃx�N�g���̊p�x��
	//�������ق�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
	//speed *= rot;
	if (rot > speed) rot = speed;

	//���E������s�����߂ɂQ�̒P�ʃx�N�g���̊O�ς��v�Z����
	float cross = ((vx * frontZ) - (vz * frontX));

	//2D�̊O�ϒl�����̏ꍇ�����̏ꍇ�ɂ���č��E���肪�s����
	//���E������s�����Ƃɂ���č��E��]��I������
	if (cross < 0.0f)
	{
		angle.y -= (rot);
	}
	else
	{
		angle.y += (rot);
	}

	/*
	{
		//DirectX::XMFLOAT3 frontF = { frontX, 0, frontZ };
		//DirectX::XMVECTOR Front = DirectX::XMLoadFloat3(&frontF);
		//DirectX::XMVECTOR Vec = DirectX::XMVectorSet(vx, 0, vz, 0);
		//DirectX::XMVECTOR Axis;
		//Axis = DirectX::XMVector3Cross(Front, Vec);

		////��]���Ɖ�]�ʂ����]�s����Z�o
		//DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(Axis, rot);

		////���݂̍s�����]������
		//DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
		//Transform = DirectX::XMMatrixMultiply(Transform, Rotation);
		//DirectX::XMStoreFloat4x4(&transform, Transform);

		////��]��̑O���������o���A�P�ʃx�N�g��������
		//Front = DirectX::XMVectorSet(transform._31, transform._32, transform._33, 0.0f);
		//DirectX::XMStoreFloat3(&direction, Direction);
	}
	*/
}

void Character::UpdateInvisibleTimer(float elapsedTime)
{
	if (invisibleTimer > 0.0f)
		invisibleTimer -= elapsedTime;
}
