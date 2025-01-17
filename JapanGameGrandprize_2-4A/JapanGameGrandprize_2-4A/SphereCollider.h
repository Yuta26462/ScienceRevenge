#pragma once
#include "define.h"
#include "ColliderBase.h"


class SphereCollider :public ColliderBase
{
protected:

	int radius;			//半径
public:

	SphereCollider():ColliderBase()
	{
		collider_type = (int)COLLIDER::SPHERE;;
		radius = 5;
	}

	SphereCollider(Location location) :ColliderBase(location)
	{
		collider_type = (int)COLLIDER::SPHERE;
		radius = 5;
	}

	//描画
	virtual void Draw()const;

	//SphereColliderとの当たり判定
	bool HitSphere(const class SphereCollider* sphere_collider) const override;

	//BoxColliderとの当たり判定
	bool HitBox(const class BoxCollider* box_collider) const override;

	//LineColliderとの当たり判定
	bool HitLine(const class LineCollider* line_collider) const override;

	//PolyLineとの当たり判定
	virtual bool HitPolyLine(const class PolyLine* poly_line)const;

	ColliderBase* Copy()const override { return new SphereCollider(*this); }

	bool HitCheck(ColliderBase* collider)const override;

	//中心座標の取得
	Location GetLocation() const
	{
		return location;
	}

	//半径の取得
	int GetRadius() const
	{
		return radius;
	}

	//中心座標の設定
	void SetLocation(Location location)
	{
		this->location = location;
	}
};