#include "TestScene.h"
#include "Engine/SceneManager.h"
#include "Engine/Input.h"

TestScene::TestScene(GameObject* parent)
	:GameObject(parent,"TestScene")
{
}

TestScene::~TestScene()
{
}

void TestScene::Initialize()
{
}

void TestScene::Update()
{
	if (Input::IsKeyDown(DIK_ESCAPE)) {
		// (SceneManager*)�̈Ӗ��c���Ԑ��̊֌W�ŁA�e�N���X���Q�Ƃ��Ă��܂����߁A�_�E���L���X�g���Ă���
		// �{���_�E���L���X�g�͗ǂ��Ȃ�(�m��Ȃ��Ǝg���Ȃ�����)
		SceneManager* pSceneManager = (SceneManager*)FindObject("SceneManager");
		pSceneManager->ChangeScene(SCENE_ID_PLAY);
	}
}

void TestScene::Draw()
{
}

void TestScene::Release()
{
}
