#include "Controller.h"
#include "Engine/Camera.h"
#include "Engine/Input.h"


Controller::Controller(GameObject* parent)
	:GameObject(parent, "Controller")
{
}

Controller::~Controller()
{
}

void Controller::Initialize()
{
	transform_.position_ = XMFLOAT3(7.2f, 0.0f, 7.5f);
}

void Controller::Update()
{
	if (Input::IsKey(DIK_LEFTARROW)) {
		transform_.rotate_.y -= 1.0f;
	}
	if (Input::IsKey(DIK_RIGHTARROW)) {
		transform_.rotate_.y += 1.0f;
	}

	//���݈ʒu���x�N�g���^�ɕϊ�
	XMVECTOR vPos = XMLoadFloat3(&transform_.position_);

	//1�t���[���̈ړ��x�N�g��
	//����0.1m
	XMVECTOR vMove = { 0.0f, 0.0f, 0.1f, 0.0f };
	//����0.1m
	XMVECTOR vMoveX = { 0.1f, 0.0f, 0.0f, 0.0f };

	//transform_.rotate_.y�x��]������s����쐬
	XMMATRIX mRotY = XMMatrixRotationY(XMConvertToRadians(transform_.rotate_.y));

	if (Input::IsKey(DIK_W)) {
		vPos += vMove;
		XMStoreFloat3(&transform_.position_, vPos);
	}
	if (Input::IsKey(DIK_S)) {
		vPos -= vMove;
		XMStoreFloat3(&transform_.position_, vPos);
	}
	if (Input::IsKey(DIK_A)) {
		vPos -= vMoveX;
		XMStoreFloat3(&transform_.position_, vPos);
	}
	if (Input::IsKey(DIK_D)) {
		vPos += vMoveX;
		XMStoreFloat3(&transform_.position_, vPos);
	}

	Camera::SetTarget(transform_.position_);

	//�J������Controller�̊p�x�Ɠ���������
	//vCam -> �J�����̃x�N�g��
	XMVECTOR vCam = { 0,5,-10,0 };
	vCam = XMVector3TransformCoord(vCam, mRotY);
	XMFLOAT3 camPos;
	XMFLOAT3 camTag;
	XMStoreFloat3(&camPos, vPos + vCam);
	Camera::SetPosition(camPos);
}

void Controller::Draw()
{
}

void Controller::Release()
{
}
