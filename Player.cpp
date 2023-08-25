#include "Player.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "PlayerChild.h"


Player::Player(GameObject* parent):GameObject(parent,"Player"),hModel_(-1)
{
}

Player::~Player()
{
}

void Player::Initialize()
{
	hModel_ = Model::Load("Assets/Oden2.fbx");
	this->transform_.scale_.x = 0.5f;
	this->transform_.scale_.y = 0.5f;
	this->transform_.scale_.z = 0.5f;

	//GameObject* pPlayerChild = Instantiate<PlayerChild>(this);
	//pPlayerChild->SetPosition(2, 0.5f, 0);
	//GameObject* pCO2 = Instantiate<PlayerChild>(this);
	//pCO2->SetPosition(-2, 0.5f, 0);
}

void Player::Update()
{
	this->transform_.rotate_.y++;
	/*if (transform_.rotate_.y++ > 600) {
		Killme();
	}*/
	if (Input::IsKey(DIK_A)) {
		transform_.position_.x -= 0.1f;
	}
	if (Input::IsKey(DIK_D)) {
		transform_.position_.x += 0.1f;
	}

	if (Input::IsKeyDown(DIK_SPACE)) {
		GameObject* pBullet = Instantiate<PlayerChild>(pParent_);
		pBullet->SetPosition(transform_.position_);
	}
}

void Player::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
}

void Player::Release()
{
	//pFbx_->Release();
	//delete pFbx_;
}
