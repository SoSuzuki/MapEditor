#pragma once
#include "GameObject.h"

// ゲームのシーンを列挙
enum SCENE_ID
{
    SCENE_ID_TEST = 0,
};

class SceneManager :
    public GameObject
{
private:
    SCENE_ID currentSceneID_;
    SCENE_ID nextSceneID_;

public:
    SceneManager(GameObject* parent);
    ~SceneManager();

    void Initialize()   override;
    void Update()   override;
    void Draw()     override;
    void Release()  override;

    /// <summary>
    /// 次のシーンを_nextに切り替える
    /// </summary>
    /// <param name="_next">NextSceneID</param>
    void ChangeScene(SCENE_ID _next);

};

