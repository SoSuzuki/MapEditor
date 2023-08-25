#pragma once

#include <list>
#include <string>
#include "Transform.h"

#define SAFE_DELETE(p) if(p != nullptr){ delete p; p = nullptr;}	 //引数：ポインタ変数 自分で作ったクラスはこっち
#define SAFE_RELEASE(p) if(p != nullptr){ p->Release(); p = nullptr;}//引数：ポインタ変数 DirectX固有の型はこっち

class SphereCollider;

class GameObject
{
private:
	bool isDead_;	//削除フラグ
protected:
	std::list<GameObject*>	childList_;
	Transform				transform_;
	GameObject*				pParent_;
	std::string				objectName_;
	SphereCollider*			pCollider_;
public:
	GameObject();
	GameObject(GameObject* parent, const std::string& name);
	~GameObject();

	virtual void Initialize() = 0;
	virtual void Update()	  = 0;
	virtual void Draw()		  = 0;
	virtual void Release()	  = 0;
	void UpdateSub();
	void DrawSub();
	void ReleaseSub();
	void Killme();

	void SetPosition(XMFLOAT3 _p){ transform_.position_ = _p; }
	void SetPosition(float _x, float _y, float _z) { SetPosition(XMFLOAT3(_x, _y, _z)); }
	void SetRotate(XMFLOAT3 _r) { transform_.rotate_ = _r; }
	void SetRotate(float _x, float _y, float _z) { SetRotate(XMFLOAT3(_x, _y, _z)); }
	void SetScale(XMFLOAT3 _s) { transform_.scale_ = _s; }
	void SetScale(float _x, float _y, float _z) { SetScale(XMFLOAT3(_x, _y, _z)); }
	XMMATRIX GetWolrdMatrix(void);


	GameObject* FindChildObject(std::string &_objName);
	
	/// <summary>
	/// 再帰呼び出しでRootJobを探してそのアドレスを返す関数
	/// </summary>
	/// <returns>RootJobのアドレス(GameObject * 型)</returns>
	GameObject* GetRootJob();
	
	GameObject* FindObject(std::string _objName);

	//オブジェクトの名前を取得
	//戻値：名前
	const std::string& GetObjectName(void) const;

	/*****Collider関連*****/
	void AddCollider(SphereCollider* _pCollider);

	void Collision(GameObject* _pTarget);

	virtual void OnCollision(GameObject* _pTarget);

	void RoundRobin(GameObject* _pTarget);
	/**********************/

	template <class T>
	GameObject* Instantiate(GameObject* parent)
	{
		T* pObject;
		pObject = new T(parent);
		pObject->Initialize();
		parent->childList_.push_back(pObject);

		return pObject;
	}
};
