#include "PlayScene.h"
//#include "Engine/Fbx.h"
#include "Engine/SceneManager.h"
#include "Engine/Input.h"
#include "Player.h"
#include "Enemy.h"

const int FPS = 60;

PlayScene::PlayScene(GameObject* parent)
	:GameObject(parent, "PlayScene")
{
}

PlayScene::~PlayScene()
{
}

void PlayScene::Initialize()
{
	//Player* pPlayer;
	//pPlayer = new Player(this);		// �v���C���[�̐e�́A����(PlayScene)
	//pPlayer->Initialize();
	//childList_.push_back(pPlayer);	// PlayScene�̎q�Ƃ��āA�v���C���[��o�^

	GameObject* pPlayer = Instantiate<Player>(this);
	GameObject* pEnemy = Instantiate<Enemy>(this);
}

void PlayScene::Update()
{
	/*static int timer = 0;
	if (timer >= 5 * FPS) {
		Killme();
	}
	timer++;*/

	if (Input::IsKeyDown(DIK_ESCAPE)) {
		SceneManager* pSceneManager = (SceneManager*)FindObject("SceneManager");
		pSceneManager->ChangeScene(SCENE_ID_TEST);
	}
}

void PlayScene::Draw()
{
}

void PlayScene::Release()
{
}
