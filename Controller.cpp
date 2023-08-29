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

	//transform_.rotate_.x�x��]������s����쐬
	XMMATRIX mRotX = XMMatrixRotationX(XMConvertToRadians(transform_.rotate_.x));

	//transform_.rotate_.y�x��]������s����쐬
	XMMATRIX mRotY = XMMatrixRotationY(XMConvertToRadians(transform_.rotate_.y));

	//���݈ʒu���x�N�g���^�ɕϊ�
	XMVECTOR vPos = XMLoadFloat3(&transform_.position_);

	//1�t���[���̈ړ��x�N�g��
	//����0.1m
	XMVECTOR vMoveZ = { 0.0f, 0.0f, 0.1f, 0.0f };
	vMoveZ = XMVector3TransformCoord(vMoveZ, mRotY);
	
	//����0.1m
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

	//�J������Controller�̊p�x�Ɠ���������
	//vCam -> �J�����̃x�N�g��
	XMVECTOR vCam = { 0, 0, -10, 0 };	// y��0�ɂ��Ă����Ȃ��ƁArotate�̌v�Z�����������Ȃ�
	vCam = XMVector3TransformCoord(vCam, mRotX * mRotY);
	XMFLOAT3 camPos;
	XMStoreFloat3(&camPos, vPos + vCam);
	Camera::SetPosition(camPos);
	Camera::SetTarget(XMLoadFloat3(&transform_.position_));	// �J�����̏œ_�̓v���C���[�̈ʒu
}

void Controller::Draw()
{
}

void Controller::Release()
{
}
