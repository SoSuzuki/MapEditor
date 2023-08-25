#pragma once
#include "Engine/GameObject.h"

class PlayerChild :
    public GameObject
{
private:
    int hModel_;    //ƒ‚ƒfƒ‹”Ô†

public:
    PlayerChild(GameObject* parent);
    ~PlayerChild ();

    void Initialize()   override;
    void Update()   override;
    void Draw()     override;
    void Release()  override;

    //‰½‚©‚É“–‚½‚Á‚½
    //ˆø”FpTarget “–‚½‚Á‚½‘Šè
    void OnCollision(GameObject* pTarget) override;
};

