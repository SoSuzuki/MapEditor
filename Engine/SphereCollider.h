#pragma once
#include "Direct3D.h"

class SphereCollider
{
private:
	float radius_;	// ”¼Œa

public:
	SphereCollider(/*XMFLOAT3 _size,*/float _radius);
	~SphereCollider();

	float GetRadius() { return radius_; }
};

