#include "AttackParent.h"

AttackParent::~AttackParent()
{
}

void AttackParent::Render(ID3D11DeviceContext* dc, Shader* shader)
{
}

void AttackParent::Destroy()
{
	isDestroy = true;
}

void AttackParent::Initialize()
{
}
