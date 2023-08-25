#include "PlayerChild.h"
#include "Engine/Fbx.h"
#include "Engine/Input.h"
#include "Engine/Model.h"
#include "Engine/SphereCollider.h"

PlayerChild::PlayerChild(GameObject* parent) 
	:GameObject(parent, "PlayerChild"),hModel_(-1)
{
}

PlayerChild::~PlayerChild()
{
}

void PlayerChild::Initialize()
{
	hModel_ = Model::Load("Assets/Oden2.fbx");
	assert(hModel_ >= 0);

	this->transform_.scale_ = XMFLOAT3(0.2f, 0.2f, 0.2f);

	SphereCollider* col = new SphereCollider(1.0f);
	AddCollider(col);
}

void PlayerChild::Update()
{
	this->transform_.rotate_.z++;
	this->transform_.position_.z += 0.5f;
	if (transform_.position_.z > 50) {
		Killme();
	}
}

void PlayerChild::Draw()
{
	Model::SetTransform(hModel_,transform_);
	Model::Draw(hModel_);
}

void PlayerChild::Release()
{
}

//‰½‚©‚É“–‚½‚Á‚½
void PlayerChild::OnCollision(GameObject* pTarget)
{
	//“G‚É“–‚½‚Á‚½‚Æ‚«
	if (pTarget->GetObjectName() == "Enemy")
	{
		pTarget->Killme();
	}
}
