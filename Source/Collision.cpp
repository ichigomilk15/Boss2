#include "Collision.h"

bool Collision::IntersectSphereVsSphere(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	DirectX::XMFLOAT3& outPositionB)
{
	// A��B�̒P�ʃx�N�g�����Z�o
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec); //XMVector3Length itu berat krn ngitung akar
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	//��������
	//DirectX::XMVECTOR rangeVec = DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(PositionB, PositionA));
	float range = radiusA + radiusB;
	//DirectX::XMStoreFloat(&range, rangeVec);
	if (lengthSq >= range * range)
	{
		return false;
	}

	//A��B�������o��
	//float push = range - lengthSq;
	Vec = DirectX::XMVector3NormalizeEst(Vec);
	Vec = DirectX::XMVectorScale(Vec, range);
	PositionB = DirectX::XMVectorAdd(PositionA, Vec);
	DirectX::XMStoreFloat3(&outPositionB, PositionB);

	return true;
}

// �~���Ɖ~���̌�������
bool Collision::IntersectCylinderVsCylinder(
	const DirectX::XMFLOAT3& positionA,
	float radiusA, float heightA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB, float heightB,
	DirectX::XMFLOAT3& outPositionB)
{
	// A�̑�����B�̓�����Ȃ瓖���Ă��Ȃ�
	if (positionA.y > positionB.y + heightB)
		return false;

	// A�̓���B�̑�����艺�Ȃ瓖���Ă��Ȃ�
	if (positionA.y + heightA < positionB.y)
		return false;


	// XZ���ʂł͈̔̓`�F�b�N
	DirectX::XMVECTOR distVec;
	DirectX::XMFLOAT3 distXZ = { positionB.x - positionA.x, 0, positionB.z - positionA.z };
	distVec = DirectX::XMLoadFloat3(&distXZ);
	DirectX::XMVECTOR lengthVecSq = DirectX::XMVector3LengthSq(distVec);
	float distLSq = DirectX::XMVectorGetX(lengthVecSq);
	float distL = sqrtf(distLSq);

	float range = radiusA + radiusB;
	//float diffL = range - distL;

	/*if (distLSq > range * range)
		return false;*/
	if (distL > range)
		return false;

	// A��B�������o��
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
	distVec = DirectX::XMVector3Normalize(distVec);
	//distVec = DirectX::XMVectorScale(distVec, range);
	distVec = DirectX::XMVectorScale(distVec, (range - distL));

	//PositionB = DirectX::XMVectorAdd(PositionA, distVec);
	PositionB = DirectX::XMVectorAdd(PositionB, distVec);
	DirectX::XMStoreFloat3(&outPositionB, PositionB);

	return true;
}

bool Collision::IntersectSphereVsCylinder(
	const DirectX::XMFLOAT3& spherePosition,
	float sphereRadius,
	const DirectX::XMFLOAT3& cylinderPosition,
	float cylinderRadius,
	float cylinderHeight,
	DirectX::XMFLOAT3& outCylinderPosition)
{
	if (spherePosition.y - sphereRadius > cylinderPosition.y + cylinderHeight)
		return false;
	if (spherePosition.y + sphereRadius < cylinderPosition.y)
		return false;

	float sphereRadiusTemp = sphereRadius;
	////if (spherePosition.y > cylinderPosition.y + cylinderRadius)
	//if (spherePosition.y > cylinderPosition.y + cylinderHeight)
	//{
	//	sphereRadiusTemp = spherePosition.y - (cylinderPosition.y + cylinderRadius);
	//}
	//if (spherePosition.y < cylinderPosition.y)
	//{
	//	sphereRadiusTemp = cylinderPosition.y - spherePosition.y;
	//}

	if (sphereRadiusTemp > sphereRadius)
		return false;




	DirectX::XMVECTOR distVec;
	DirectX::XMFLOAT3 distXZ = { cylinderPosition.x - spherePosition.x, 0, cylinderPosition.z - spherePosition.z };
	distVec = DirectX::XMLoadFloat3(&distXZ);
	DirectX::XMVECTOR lengthVecSq = DirectX::XMVector3LengthSq(distVec);
	float distLSq = DirectX::XMVectorGetX(lengthVecSq);
	float distL = sqrtf(distLSq);

	float range = sphereRadiusTemp + cylinderRadius;
	//float diffL = range - distL;

	/*if (distLSq > range * range)
		return false;*/
	if (distL > range)
		return false;


	// A��B�������o��
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&spherePosition);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&cylinderPosition);
	distVec = DirectX::XMVector3Normalize(distVec);
	//distVec = DirectX::XMVectorScale(distVec, range);
	distVec = DirectX::XMVectorScale(distVec, (range - distL));

	//PositionB = DirectX::XMVectorAdd(PositionA, distVec);
	PositionB = DirectX::XMVectorAdd(PositionB, distVec);
	DirectX::XMStoreFloat3(&outCylinderPosition, PositionB);

	return true;
}

//���C�ƃ��f���̌�������
bool Collision::IntersectRayVsModel(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const Model* model, HitResult& result)
{
#if 1
	DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
	DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

	//���[���h��Ԃ̃��C�̒���
	DirectX::XMStoreFloat(&result.distance, WorldRayLength);

	bool hit = false;
	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		//���b�V���m�[�h�擾
		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

		//���C�����[���h��Ԃ��烍�[�J����Ԃ֕ϊ�
		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

		DirectX::XMVECTOR S = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
		DirectX::XMVECTOR E = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
		DirectX::XMVECTOR SE = DirectX::XMVectorSubtract(E, S);
		DirectX::XMVECTOR V = DirectX::XMVector3Normalize(SE);
		DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE);

		//���C�̒���
		float neart;
		DirectX::XMStoreFloat(&neart, Length);

		//�O�p�`�i�ʁj�Ƃ̌�������
		const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
		const std::vector<UINT> indices = mesh.indices;

		int materialIndex = -1;
		DirectX::XMVECTOR HitPosition;
		DirectX::XMVECTOR HitNormal;
		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			for (UINT i = 0; i < subset.indexCount; i += 3)
			{
				UINT index = subset.startIndex + i;
				//�O�p�`�̒��_�𒊏o
				const ModelResource::Vertex& a = vertices.at(indices.at(index));
				/*const ModelResource::Vertex& b = vertices.at(indices.at((index + 1)%UINT8_MAX));
				const ModelResource::Vertex& c = vertices.at(indices.at((index + 2)%UINT8_MAX));*/
				const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
				const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

				DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
				DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
				DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

				//�O�p�`�̎O�Ӄx�N�g�����Z�o
				DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
				DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
				DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

				//�O�p�`�̖@���x�N�g�����Z�o
				DirectX::XMVECTOR N = DirectX::XMVector3Cross(AB, BC);

				//���ς̌��ʂ��v���X�Ȃ�Η�����
				DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(V, N);
				float dot = DirectX::XMVectorGetX(Dot);
				if (dot >= 0) continue;

				//���C�ƕ��ʂ̌�_���Z�o
				DirectX::XMVECTOR SA = DirectX::XMVectorSubtract(A, S);
				DirectX::XMVECTOR d1 = DirectX::XMVector3Dot(SA, N);
				DirectX::XMVECTOR X = DirectX::XMVectorDivide(d1, Dot);
				float x;
				DirectX::XMStoreFloat(&x, X);
				if (x < .0f || x > neart) continue; //��_�܂ł̋��������܂łɌv�Z�����ŋߋ������傫�����̓X�L�b�v

				DirectX::XMVECTOR P = DirectX::XMVectorAdd(S, DirectX::XMVectorScale(V, x));

				//��_���O�p�`�̓����ɂ��邩����
				//1��
				DirectX::XMVECTOR PA = DirectX::XMVectorSubtract(A, P);
				DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(PA, AB);
				DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, N/*DirectX::XMLoadFloat3(&a.normal)*/);
				float dot1;
				DirectX::XMStoreFloat(&dot1, Dot1);
				if (dot1 < 0) continue;

				//2��
				DirectX::XMVECTOR PB = DirectX::XMVectorSubtract(B, P);
				DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(PB, BC);
				DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, N/*DirectX::XMLoadFloat3(&b.normal)*/);
				float dot2;
				DirectX::XMStoreFloat(&dot2, Dot2);
				if (dot2 < 0) continue;

				//3��
				DirectX::XMVECTOR PC = DirectX::XMVectorSubtract(C, P);
				DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(PC, CA);
				DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, N/*DirectX::XMLoadFloat3(&c.normal)*/);
				float dot3;
				DirectX::XMStoreFloat(&dot3, Dot3);
				if (dot3 < 0) continue;

				//�ŋߋ������X�V
				neart = x;

				//��_�Ɩ@�����X�V
				HitPosition = P;
				HitNormal = N;
				materialIndex = subset.materialIndex;
			}
		}

		if (materialIndex >= 0)
		{
			//���[�J����Ԃ��烏�[���h��ԋ�Ԃ֕ϊ�
			DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(HitPosition, WorldTransform); //transformCOORD -> w = 1
			DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
			DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
			float distance;
			DirectX::XMStoreFloat(&distance, WorldCrossLength);

			//�q�b�g���ۑ�
			if (result.distance > distance)
			{
				DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal, WorldTransform);//transformNORMAL -> w = 0

				result.distance = distance;
				result.materialIndex = materialIndex;
				DirectX::XMStoreFloat3(&result.position, WorldPosition);
				DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
				hit = true;
			}
		}
	}

	return hit;

	//�ȑO�̏����������������悤�ɉ��̎���
	/*if (end.y < 0.0f)
	{
		result.position.x = end.x;
		result.position.y = 0.0f;
		result.position.z = end.z;
		result.normal.x = 0.0f;
		result.normal.y = 1.0f;
		result.normal.z = 0.0f;
		return true;
	}*/
	//return false;
#else
	DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
	DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

	// ���[���h��Ԃ̃��C�̒���
	DirectX::XMStoreFloat(&result.distance, WorldRayLength);

	bool hit = false;
	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		const Model::Node node = model->GetNodes().at(mesh.nodeIndex);

		// ���C�����[���h��Ԃ��烍�[�J����Ԃ֕ϊ�
		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

		DirectX::XMVECTOR Start = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
		DirectX::XMVECTOR End = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
		DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);
		DirectX::XMVECTOR Dir = DirectX::XMVector3Normalize(Vec);
		DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);

		// ���C�̒���
		float neart;
		DirectX::XMStoreFloat(&neart, Length);

		// �O�p�`(��)�Ƃ̌�������
		const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
		const std::vector<UINT> indices = mesh.indices;

		int materialIndex = -1;
		DirectX::XMVECTOR HitPosition;
		DirectX::XMVECTOR HitNormal;
		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			for (UINT i = 0; i < subset.indexCount; i += 3)
			{
				UINT index = subset.startIndex + i;
				// �O�p�`�̒��_�𒊏o
				const ModelResource::Vertex& a = vertices.at(indices.at(index));
				const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
				const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

				DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
				DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
				DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

				// �O�p�`�̎O�Ӄx�N�g�����Z�o
				DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
				DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
				DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

				//�O�p�`�̖@���x�N�g�����Z�o
				DirectX::XMVECTOR Normal = DirectX::XMVector3Cross(AB, BC);

				// ���ς̌��ʂ��v���X�Ȃ�Η�����
				DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Dir, Normal);
				float dot;
				DirectX::XMStoreFloat(&dot, Dot);
				if (dot >= 0)continue;

				// ���C�Ƃ̕��ʂ̌�_���Z�o
				DirectX::XMVECTOR V = DirectX::XMVectorSubtract(A, Start);
				DirectX::XMVECTOR T = DirectX::XMVectorDivide(DirectX::XMVector3Dot(Normal, V), Dot);
				float t;
				DirectX::XMStoreFloat(&t, T);
				// ��_�܂ł̋��������܂łɌv�Z�����ŋߋ������傫�����̓X�L�b�v
				if (t<0.0f || t>neart)continue;

				DirectX::XMVECTOR Position;
				Position = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(Dir, T), Start);

				// ��_���O�p�`�̓����ɂ��邩����
				// �P��
				DirectX::XMVECTOR V1 = DirectX::XMVectorSubtract(A, Position);
				DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(V1, AB);
				DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, Normal);
				float dot1;
				DirectX::XMStoreFloat(&dot1, Dot1);
				if (dot1 < 0.0f)continue;

				// �Q��
				DirectX::XMVECTOR V2 = DirectX::XMVectorSubtract(B, Position);
				DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(V2, BC);
				DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, Normal);
				float dot2;
				DirectX::XMStoreFloat(&dot2, Dot2);
				if (dot2 < 0.0f)continue;

				// �R��
				DirectX::XMVECTOR V3 = DirectX::XMVectorSubtract(C, Position);
				DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(V3, CA);
				DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, Normal);
				float dot3;
				DirectX::XMStoreFloat(&dot3, Dot3);
				if (dot3 < 0.0f)continue;

				// �ŋߋ������X�V
				neart = t;

				// ��_�Ɩ@�����X�V
				HitPosition = Position;
				HitNormal = Normal;
				materialIndex = subset.materialIndex;
			}
		}
		if (materialIndex >= 0)
		{
			// ���[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
			DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
			DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
			DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
			float distance;
			DirectX::XMStoreFloat(&distance, WorldCrossLength);

			// �q�b�g���ۑ�
			if (result.distance > distance)
			{
				DirectX::XMVECTOR WorldNormal = DirectX::XMVector3Transform(HitNormal, WorldTransform);
				result.distance = distance;
				result.materialIndex = materialIndex;
				DirectX::XMStoreFloat3(&result.position, WorldPosition);
				DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
				hit = true;
			}
		}
	}
	return hit;
#endif
}

