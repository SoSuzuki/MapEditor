#include "GameObject.h"
#include "SphereCollider.h"

GameObject::GameObject():pParent_(nullptr)
{
}

GameObject::GameObject(GameObject* parent, const std::string& name)
	:pParent_(parent), objectName_(name), pCollider_(nullptr), isDead_(false)
{
	if (parent/* != nullptr*/)
		this->transform_.pParent_ = &(parent->transform_);
}

GameObject::~GameObject()
{
}

void GameObject::UpdateSub()
{
	Update();

	RoundRobin(GetRootJob());
	for (auto itr = childList_.begin(); itr != childList_.end(); itr++)
		(*itr)->UpdateSub();	// 子どもの子どもを子どもが呼び出してあーだこーだ…(ややこしい)

	for (auto itr = childList_.begin(); itr != childList_.end();) {
		if ((*itr)->isDead_) {
			(*itr)->ReleaseSub();
			itr = childList_.erase(itr);
		}
		else {
			itr++;
		}
	}
}

void GameObject::DrawSub()
{
	Draw();
	for (auto itr = childList_.begin(); itr != childList_.end(); itr++)
		(*itr)->DrawSub();
}

void GameObject::ReleaseSub()
{	//ここでオブジェクトを解放してしまうのでdeleteしなくて済む
	for (auto itr = childList_.begin(); itr != childList_.end(); itr++) {
		(*itr)->ReleaseSub();
		SAFE_DELETE(*itr);
	}
	Release();
}

void GameObject::Killme()
{
	isDead_ = true;
}

XMMATRIX GameObject::GetWolrdMatrix(void)
{
	return transform_.GetWorldMatrix();
}

GameObject* GameObject::FindChildObject(std::string& _objName)
{
	if (_objName == this->objectName_) {
		return(this);	// 自分が_objNameのオブジェクトだったら、thisをNULLに
	}
	else {
		for (auto &itr:childList_) {	// auto &e: childlistでも同じ
			GameObject* obj = itr->FindChildObject(_objName);
			if (obj != nullptr)
				return obj;
		}
	}
	return nullptr;
}

GameObject* GameObject::GetRootJob()
{	//再起処理
	if (pParent_ == nullptr)
		return this;
	return pParent_->GetRootJob();
}

GameObject* GameObject::FindObject(std::string _objName)
{	// 自分から見たRootJobを探し、そこから全ての子を辿って、_objNameを探す
	return GetRootJob()->FindChildObject(_objName);
}

//オブジェクトの名前を取得
const std::string& GameObject::GetObjectName(void) const
{
	return objectName_;
}

void GameObject::AddCollider(SphereCollider* _pCollider)
{
	pCollider_ = _pCollider;
}

void GameObject::Collision(GameObject* _pTarget)
{
	if (_pTarget == this || _pTarget->pCollider_ == nullptr)
		return;	// 自分自身 or ターゲットにコライダーがアタッチされてない
	
	//XMVECTOR v{ transform_.position_.x - _pTarget->transform_.position_.x,
	//			transform_.position_.y - _pTarget->transform_.position_.y,
	//			transform_.position_.z - _pTarget->transform_.position_.z,
	//			0 };
	//XMVECTOR dist = XMVector3Dot(v, v);

	// pow関数よりも直で2乗した方が早い
	float dist = (transform_.position_.x - _pTarget->transform_.position_.x) * (transform_.position_.x - _pTarget->transform_.position_.x)
		+ (transform_.position_.y - _pTarget->transform_.position_.y) * (transform_.position_.y - _pTarget->transform_.position_.y)
		+ (transform_.position_.z - _pTarget->transform_.position_.z) * (transform_.position_.z - _pTarget->transform_.position_.z);

	float rDist = (this->pCollider_->GetRadius() + _pTarget->pCollider_->GetRadius()) * (this->pCollider_->GetRadius() + _pTarget->pCollider_->GetRadius());

	// 自分とターゲットの距離 <= R1 + R2なら (もし、自分のコライダーとターゲットがぶつかっていたら
	// onCollision関数を呼び出す
	if (dist <= rDist) {
		//onCollision(_pTarget);
		double p = 0;	// デバッグ用
	}

}

void GameObject::OnCollision(GameObject* _pTarget)
{
	_pTarget->Killme();
}

void GameObject::RoundRobin(GameObject* _pTarget)
{
	if (pCollider_ == nullptr)
		return;
	if(_pTarget->pCollider_!=nullptr)
		Collision(_pTarget);

	// ターゲットの子リストから総当たり
	for (auto& itr : _pTarget->childList_)
	{
		RoundRobin(itr);
	}

}









