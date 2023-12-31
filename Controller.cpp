#include "Controller.h"
#include "Engine/Camera.h"
#include "Engine/Input.h"


Controller::Controller(GameObject* parent)
	:GameObject(parent, "Controller"),isDegOver(false)
{
	transform_.position_ = XMFLOAT3(7.0f, 0.0f, 7.0f);
	transform_.rotate_.x = 45.0f;
}

Controller::~Controller()
{
}

void Controller::Initialize()
{
	
}

void Controller::Update()
{

	if (Input::IsKey(DIK_LEFT)) {
		transform_.rotate_.y -= 1.0f;
	}
	if (Input::IsKey(DIK_RIGHT)) {
		transform_.rotate_.y += 1.0f;
	}

	if (Input::IsKey(DIK_UP)) {
		transform_.rotate_.x += 1.0f;
		if (transform_.rotate_.x > 89)
			transform_.rotate_.x = 89;
	}
	if (Input::IsKey(DIK_DOWN)) {
		transform_.rotate_.x -= 1.0f;
		if (transform_.rotate_.x <= 0)
			transform_.rotate_.x = 0;
	}


	if (Input::IsKey(DIK_SPACE)) {
		transform_.position_.y += 0.05f;
		if (transform_.position_.y > 30)
			transform_.position_.y = 30.0f;
	}

	if (Input::IsKey(DIK_LCONTROL)) {
		transform_.position_.y -= 0.05f;
		if (transform_.position_.y <= 0)
			transform_.position_.y = 0;
	}

	if (Input::IsKeyDown(DIK_R)) {
		transform_.position_ = XMFLOAT3(7.0f, 0.0f, 7.0f);
		transform_.rotate_ = XMFLOAT3(45.0f, 0, 0);
	}


	//transform_.rotate_.x度回転させる行列を作成
	XMMATRIX mRotX = XMMatrixRotationX(XMConvertToRadians(transform_.rotate_.x));

	//transform_.rotate_.y度回転させる行列を作成
	XMMATRIX mRotY = XMMatrixRotationY(XMConvertToRadians(transform_.rotate_.y));

	//transform_.rotate_.z度回転させる行列を作成
	XMMATRIX mRotZ = XMMatrixRotationZ(XMConvertToRadians(transform_.rotate_.z));

	//現在位置をベクトル型に変換
	XMVECTOR vPos = XMLoadFloat3(&transform_.position_);

	//1フレームの移動ベクトル
	//奥に0.1m
	XMVECTOR vMoveZ = { 0.0f, 0.0f, 0.1f, 0.0f };
	vMoveZ = XMVector3TransformCoord(vMoveZ, mRotY);
	
	//横に0.1m
	XMVECTOR vMoveX = { 0.1f, 0.0f, 0.0f, 0.0f };
	vMoveX = XMVector3TransformCoord(vMoveX, mRotY);

	if (Input::IsKey(DIK_W)) {
		vPos += vMoveZ;
	}
	if (Input::IsKey(DIK_S)) {
		vPos -= vMoveZ;
	}
	if (Input::IsKey(DIK_A)) {
		vPos -= vMoveX;
	}
	if (Input::IsKey(DIK_D)) {
		vPos += vMoveX;
	}
	XMStoreFloat3(&transform_.position_, vPos);

	//カメラをControllerの角度と同期させる
	//vCam -> カメラのベクトル
	XMVECTOR vCam = { 0, 0, -10, 0 };	// yは0にしておかないと、rotateの計算がおかしくなる
	vCam = XMVector3TransformCoord(vCam, mRotX * mRotY * mRotZ);
	
	//XMFLOAT3 camPos;
	//XMStoreFloat3(&camPos, vPos + vCam);
	//Camera::SetPosition(camPos);
	//Camera::SetTarget(XMLoadFloat3(&transform_.position_));	// カメラの焦点はプレイヤーの位置

	Camera::SetPosition(vPos + vCam);           //カメラの位置は自撮り棒の先端（現在地+自撮り棒）
	Camera::SetTarget(transform_.position_);    //カメラの見る位置はこのオブジェクトの位置
}

void Controller::Draw()
{
}

void Controller::Release()
{
}
