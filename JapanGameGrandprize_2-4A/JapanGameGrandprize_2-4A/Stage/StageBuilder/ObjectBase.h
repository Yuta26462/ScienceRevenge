#pragma once
#include "../../Define.h"
#include "../../ColliderBase.h"
#include "../../TextureContainer.h"

 //保存 読み込みに使うキー--------------------------------------
#define OBJECT_NAME_DEFAULT "object"
//--------------------------------------------------------------


class ObjectBase
{
private:
	const char* class_name = OBJECT_NAME_DEFAULT;
	const char* texture_name = "yuka_1";
	int image;
	ColliderBase* collider;
	SphereCollider* pivot;
	Location vector;
	Location old_location;
	bool is_reverse;

	void ColliderCast(ColliderBase* p_collider);
public:
	ObjectBase();
	ObjectBase(Location pivot, ColliderBase* p_collider ,const char* texture_name);
	~ObjectBase();

	void Draw()const;

	void UpdateColliderPos();

	bool HitSphere(SphereCollider* sphere)
	{
		return collider->HitSphere(sphere);
	}

	bool HitBox(BoxCollider* box)
	{
		return collider->HitBox(box);
	}

	bool HitLine(LineCollider* line)
	{
		return collider->HitLine(line);
	}

	ColliderBase* GetColllider()const { return collider; }
	SphereCollider* GetPivot()const { return pivot; }
	const char* GetObjectName() { return class_name; }
	const char* GetTextureName() { return texture_name; }

};