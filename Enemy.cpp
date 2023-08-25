#include "Enemy.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/SphereCollider.h"

Enemy::Enemy(GameObject* parent)
	:GameObject(parent,"Enemy"),hModel_(-1)
{
}

Enemy::~Enemy()
{
}

void Enemy::Initialize()
{
	hModel_ = Model::Load("Assets/Oden2.fbx");
	assert(hModel_ >= 0);
	this->transform_.rotate_.x = 180.0f;
	this->transform_.position_ = XMFLOAT3(0, 0, 10.0f);

	SphereCollider* col = new SphereCollider(3.0f);
	AddCollider(col);
}

void Enemy::Update()
{
	this->transform_.rotate_.y--;
	if (Input::IsKey(DIK_UPARROW)) {
		transform_.position_.z++;
	}
	if (Input::IsKey(DIK_DOWNARROW)) {
		transform_.position_.z--;
	}
}

void Enemy::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
}

void Enemy::Release()
{
}

//‰½‚©‚É“–‚½‚Á‚½
void Enemy::OnCollision(GameObject* pTarget)
{
	//“G‚É“–‚½‚Á‚½‚Æ‚«
	if (pTarget->GetObjectName() == "Enemy")
	{
		Killme();
	}
}
