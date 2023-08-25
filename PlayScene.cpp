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
	//pPlayer = new Player(this);		// プレイヤーの親は、自分(PlayScene)
	//pPlayer->Initialize();
	//childList_.push_back(pPlayer);	// PlaySceneの子として、プレイヤーを登録

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
