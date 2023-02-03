#define _USE_MATH_DEFINES
#include<math.h>
#include "BoxCollider.h"
#include"SphereCollider.h"


bool SphereCollider::HitSphere(SphereCollider* sphere_collider)
{
	bool ret = false;//返り値
	float distance;	//中心座標の距離

	//中心座標の距離の計算
	distance = sqrtf(powf(sphere_collider->GetLocation().x - location.x, 2) + powf(sphere_collider->GetLocation().y - location.y, 2));

	if (distance < radius + sphere_collider->GetRadius()) //当たり判定
	{
		ret = true;
	}

	return ret;
}

bool SphereCollider::HitBox(class BoxCollider* box_collider)
{
	bool ret = false; //返り値
	float rad; //2点の角度
	float sphere_x; //円の座標(x)
	float sphere_y; //円の座標(y)

	//角度の計算
	rad = atan2f(box_collider->GetLocation().y - location.y, box_collider->GetLocation().x - location.x);

	//座標の計算
	sphere_x = location.x + (radius * cosf(rad));
	sphere_y = location.y - (radius * sinf(rad));

	//自分の当たり判定の範囲
	//float px1 = location.x - (box_collider.GetErea().width / 2);
	//float py1 = location.y - (box_collider.GetErea().height / 2);
	float px1 = box_collider->GetLocation().x;
	float py1 = box_collider->GetLocation().y;
	float px2 = px1 + box_collider->GetArea().width;
	float py2 = py1 + box_collider->GetArea().height;


	if ((px1 < sphere_x) && (sphere_x < px2) && (py1 < sphere_y) && (sphere_y < py2)) //当たり判定
	{
		ret = true;
	}

	return ret;
}
