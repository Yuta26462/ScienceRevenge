#include "BoxCollider.h"
#include"SphereCollider.h"
#define _USE_MATH_DEFINES
#include<math.h>


//SphereColliderとの当たり判定
bool BoxCollider::HitSphere(class SphereCollider* sphere_collider)
{
	bool ret = false;//返り値
	float rad; //2点の角度
	float sphere_x; //円の座標(x)
	float sphere_y; //円の座標(y)

	//角度の計算
	rad = atan2f(sphere_collider->GetLocation().y - location.y, sphere_collider->GetLocation().x - location.x);

	//座標の計算
	sphere_x = sphere_collider->GetLocation().x + (sphere_collider->GetRadius() * cosf(rad));
	sphere_y = sphere_collider->GetLocation().y + (sphere_collider->GetRadius() * sinf(rad));

	//自分の当たり判定の範囲
	float mx1 = location.x - (area.width / 2);
	float my1 = location.y - (area.height / 2);
	float mx2 = mx1 - area.width;
	float my2 = my1 - area.height;


	if ((mx1 > sphere_x) && (mx2 < sphere_x) && (my1 > sphere_y) && (my2 < sphere_y)) //当たり判定
	{
		ret = true;
	}

	return ret;
}

//BoxColliderとの当たり判定
bool BoxCollider::HitBox(BoxCollider* box_collider)
{
	bool ret = false; //返り値

	//自分の当たり判定の範囲
	//float mx1 = location.x - (erea.width / 2);
	//float my1 = location.y - (erea.height / 2);
	float mx1 = location.x;
	float my1 = location.y;
	float mx2 = mx1 + area.width;
	float my2 = my1 + area.height;

	//相手の当たり判定の範囲
	//float px1 = box_collider.GetLocation().x - (box_collider.GetErea().width / 2);
	//float py1 = box_collider.GetLocation().x - (box_collider.GetErea().height / 2);
	float px1 = box_collider->GetLocation().x;
	float py1 = box_collider->GetLocation().y;
	float px2 = px1 + box_collider->GetArea().width;
	float py2 = py1 + box_collider->GetArea().height;

	if ((mx1 < px2) && (px1 < mx2) && (my1 < py2) && (py1 < my2)) //当たり判定
	{
		ret = true;
	}
	return ret;
}

//中心座標の取得
Location BoxCollider::GetLocation()const
{
	return location;
}

//半径の取得
Area BoxCollider::GetArea()const
{
	return area;
}

//中心座標の設定
void BoxCollider::SetLocation(Location location)
{
	this->location = location;
}