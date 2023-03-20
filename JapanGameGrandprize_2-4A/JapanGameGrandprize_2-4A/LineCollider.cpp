#include "LineCollider.h"
#include "CameraWork.h"
#include "BoxCollider.h"
#include "SphereCollider.h"

Location t = {};
float nearpos = 0;

//------------------------------------
// コンストラクタ
//------------------------------------
LineCollider_t::LineCollider_t()
{
	//絶対座標
	vector[LINE_START] = { 0,0 };
	vector[LINE_END] = { SCREEN_WIDTH, SCREEN_HEIGHT };
	
	location = GetMiddlePoint();
	Relativize();
}

//------------------------------------
// コンストラクタ
//------------------------------------
LineCollider_t::LineCollider_t(Location point1, Location point2)
{
	//点じゃないことを保証する
	if (point1 == point2)
	{
		point2.x += 1;
		point2.y += 1;
	}
	//絶対座標
	vector[LINE_START] = point1;
	vector[LINE_END] = point2;

	location = GetMiddlePoint();
	Relativize();
}

//--------------------------------------
// デストラクタ
//--------------------------------------
LineCollider_t::~LineCollider_t()
{

}

//--------------------------------------
// 描画
//--------------------------------------
void LineCollider_t::Draw()const
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawLineAA(
		GetLocation(LINE_START).x - CameraWork::GetCamera().x, 
		GetLocation(LINE_START).y - CameraWork::GetCamera().y,
		GetLocation(LINE_END).x - CameraWork::GetCamera().x,
		GetLocation(LINE_END).y - CameraWork::GetCamera().y,
		0xE9FF00, 3);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);


	DrawBoxAA(GetMin().x - CameraWork::GetCamera().x
		, GetMin().y - CameraWork::GetCamera().y
		, GetMax().x - CameraWork::GetCamera().x
		, GetMax().y - CameraWork::GetCamera().y
		, 0x000000, FALSE);

	float x = GetLocation(LINE_START).x + t.x - CameraWork::GetCamera().x;
	float y = GetLocation(LINE_START).y + t.y - CameraWork::GetCamera().y;

	DrawCircle(GetLocation(LINE_START).x + t.x - CameraWork::GetCamera().x,
		GetLocation(LINE_START).y + t.y - CameraWork::GetCamera().y,
		3, 0xFF0000, TRUE);


	DrawFormatString(GetLocation(LINE_START).x + t.x - CameraWork::GetCamera().x,
		GetLocation(LINE_START).y + t.y - CameraWork::GetCamera().y - 30,
		 0xFF00FF, "%lf", nearpos);
}

//-----------------------------------
// 点との当たり判定
//-----------------------------------
bool LineCollider_t::HitDot(Location point)const
{
	bool is_hit = false;
	float closs_product;

	Location vector1 =
		GetLocation(LINE_START) - GetLocation(LINE_END);
	Location vector2 =
		point - GetLocation(LINE_END);

	closs_product = (vector1.x * vector2.y) - (vector1.y * vector2.x);
	//線分の範囲内かどうか
	if ((GetMin() <= point) && point <= GetMax())
	{
		if (closs_product == 0)
		{
			is_hit = true;
		}
	}


	return is_hit;
}

//-----------------------------------
// SphereColliderとの当たり判定
//-----------------------------------
bool LineCollider_t::HitSphere(const SphereCollider* sphere)const
{
	//
	bool is_hit = false;
	Location vector1 =
		GetLocation(LINE_END) - GetLocation(LINE_START);
	Location vector2 =
		sphere->GetLocation() - GetLocation(LINE_START);

	float len = powf(vector1.x * vector1.x + vector1.y * vector1.y, 0.5);

	Location unit_vector;
	unit_vector.x = vector1.x / len;
	unit_vector.y = vector1.y / len;

	float distance_near_pos = unit_vector.x * vector2.x
		+ unit_vector.y * vector2.y;

	Location near_pos;

	near_pos.x = unit_vector.x * distance_near_pos;
	near_pos.y = unit_vector.y * distance_near_pos;

	t = near_pos;

	float distance_sphere_near_pos;

	distance_sphere_near_pos =
		powf(powf(near_pos.x - vector2.x, 2.0) + powf(near_pos.y - vector2.y, 2.0), 0.5);

	nearpos = distance_sphere_near_pos;

	if (distance_sphere_near_pos > sphere->GetRadius())
	{
		return false;
	}

	bool is_start_acute = CheckIsAcute(GetLocation(LINE_END)
		- GetLocation(LINE_START), vector2);
	bool is_end_acute 
		= CheckIsAcute(GetLocation(LINE_END) - GetLocation(LINE_START)
			,sphere->GetLocation() - GetLocation(LINE_END) );

	if (is_start_acute ^ is_end_acute)
	{
		is_hit = true;
	}

	float distance_tips_sphere
		= MakeScalar(sphere->GetLocation() - GetLocation(LINE_START));
	if (distance_tips_sphere <= sphere->GetRadius())
	{
		is_hit = true;
	}
	distance_tips_sphere
		= MakeScalar(sphere->GetLocation() - GetLocation(LINE_END));
	if (distance_tips_sphere <= sphere->GetRadius())
	{
		is_hit = true;
	}

	return is_hit;
}

//-----------------------------------
// BoxColliderとの当たり判定
//-----------------------------------
bool LineCollider_t::HitBox(const BoxCollider* box)const
{
	bool is_hit = false;
	bool box_ishit = false;
	float sign = 0;

	Location vertex[4] =
	{
		//四隅
		{box->GetLocation().x - box->GetArea().width / 2,
		box->GetLocation().y - box->GetArea().height / 2 },

		{box->GetLocation().x + box->GetArea().width / 2,
		box->GetLocation().y - box->GetArea().height / 2 },

		{box->GetLocation().x - box->GetArea().width / 2,
		box->GetLocation().y + box->GetArea().height / 2 },

		{box->GetLocation().x + box->GetArea().width / 2,
		box->GetLocation().y + box->GetArea().height / 2},

	};

	for (int i = 0; i < 4; i++)
	{
		if ((GetMin() <= vertex[i]) && vertex[i] <= GetMax())
		{
			box_ishit = true;
		}
	}
	if (box_ishit == false)return false; //線分を囲う四角形にすら当たってない

	
	for (int i = 0; i < 4; i++)
	{
		float closs_product = 0;

		Location vector1 =
			GetLocation(LINE_START) - GetLocation(LINE_END);
		Location vector2 =
			vertex[i] - GetLocation(LINE_END);

		closs_product = (vector1.x * vector2.y) - (vector1.y * vector2.x);
		if (closs_product == 0)return true;	//0なら線と頂点が重なっているので当たり

		if (i == 0)
		{
			sign = closs_product;
		}
		else
		{
			if ((sign * closs_product) < 0)
			{
				is_hit = true;
				break;
			}
		}
	}

	return is_hit;
}

//-----------------------------------
// LineColliderとの当たり判定
//-----------------------------------
bool LineCollider_t::HitLine(const LineCollider_t* line)const
{
	bool is_hit = false;
	return is_hit;
}

//-----------------------------------------------------
// 線の端の座標の取得 始点か終点返す デフォルトは始点
//-----------------------------------------------------
Location LineCollider_t::GetLocation(int index)const
{
	if (index < 2)
	{
		return MakeTip(index);
	}
	else
	{
		return MakeTip(LINE_START);
	}
}

//--------------------------------------------------
// 始点、終点座標の設定 引数：座標、始点：0 終点：1
//--------------------------------------------------
void LineCollider_t::SetLocation(Location location, int index)
{
	if (index < 2)
	{
		vector[LINE_START] = MakeTip(LINE_START);
		vector[LINE_END] = MakeTip(LINE_END);
		vector[index] = location;
		this->location = GetMiddlePoint();
		Relativize();
	}
}

//-----------------------------------
// SphereColliderとの当たり判定
//-----------------------------------
bool LineCollider::HitSphere(class SphereCollider* sphere_collider)
{

	bool ret = false; //返り値

	float vector_x[3]; //X座標のベクトル
	float vector_y[3]; //Y座標のベクトル

	float unit_vector;	//単位ベクトル
	float shortest_distance; //線分と円の最短の距離

	float inner_product[2]; //内積
	float center_distance[2]; //円の中心との距離(0:線分の始点　1:線分の終点)

	//自分のLineColliderの始点と終点とのベクトルの計算
	vector_x[0] = location[1].x - location[0].x;
	vector_y[0] = location[1].y - location[0].y;

	//自分のLineColliderの始点とphereColliderの中心とのベクトルの計算
	vector_x[1] = sphere_collider->GetLocation().x - location[0].x;
	vector_y[1] = sphere_collider->GetLocation().y - location[0].y;

	//自分のLineColliderの終点とSphereColliderの中心とのベクトルの計算
	vector_x[2] = sphere_collider->GetLocation().x - location[1].x;
	vector_y[2] = sphere_collider->GetLocation().y - location[1].y;

	//単位ベクトルの計算
	unit_vector = sqrtf(powf(vector_x[0], 2) + powf(vector_y[0], 2));

	//最短距離の計算
	shortest_distance = (vector_x[1] * (vector_x[0] / unit_vector)) 
						- (vector_y[1] * (vector_y[0] / unit_vector));

	if (fabsf(shortest_distance) <= sphere_collider->GetRadius())
	{
		//内積の計算
		for (int i = 0; i < 2; i++)
		{
			inner_product[i] = (vector_x[i + 1] * vector_x[0]) 
								- (vector_y[i + 1] * vector_y[0]);
		}
		//円の中心との距離の計算
		for (int i = 0; i < 2; i++)
		{
			center_distance[i] = sqrtf(powf(vector_x[i + 1], 2) + powf(vector_y[i + 1], 2));
		}

		if ((inner_product[0] * inner_product[1] <= 0.0f) 
				|| (center_distance[0] < sphere_collider->GetRadius()) 
					||(center_distance[1] < sphere_collider->GetRadius()))
		{
			ret = true;
		}
	}

	return ret;
}

//-----------------------------------
// BoxColliderとの当たり判定
//-----------------------------------
bool LineCollider::HitBox(class BoxCollider* box_collider)
{
	bool ret = false; //返り値

	float vector_x[3]; //X座標のベクトル
	float vector_y[3]; //Y座標のベクトル

	float outer_product[4]; //外積

	//自分の当たり判定の範囲
	float my_x[2];
	float my_y[2];

	//相手の当たり判定の範囲
	float sub_x[2];
	float sub_y[2];
	
	bool sign = false; //最初の外積の符号(true:正の数、false:負の数)

	//自分の当たり判定の範囲の計算
	if (location[0].x <= location[1].x)
	{
		my_x[0] = location[0].x;
		my_x[1] = location[1].x;

	}
	else
	{
		my_x[0] = location[1].x;
		my_x[1] = location[0].x;
	}

	if (location[0].y <= location[1].y)
	{
		my_y[0] = location[0].y;
		my_y[1] = location[1].y;
	}
	else
	{
		my_y[0] = location[1].y;
		my_y[1] = location[0].y;
	}

	//相手の当たり判定の範囲の計算
	sub_x[0] = box_collider->GetLocation().x - (box_collider->GetArea().width / 2);
	sub_y[0] = box_collider->GetLocation().y - (box_collider->GetArea().height / 2);
	sub_x[1] = sub_x[0] + box_collider->GetArea().width;
	sub_y[1] = sub_y[0] + box_collider->GetArea().height;


	if ((my_x[0] < sub_x[1]) && (my_x[1] < sub_x[1]) 
			&& (my_y[0] < sub_y[1]) && (sub_y[0] < my_y[1]))
	{
		//自分のLineColliderの始点と終点とのベクトルの計算
		vector_x[0] = location[1].x - location[0].x;
		vector_y[0] = location[1].y - location[0].y;

		//自分のLineColliderの始点と左上の角とのベクトル
		vector_x[1] = box_collider->GetLocation().x - location[0].x;
		vector_y[1] = box_collider->GetLocation().y - location[0].y;

		//自分のLineColliderの始点と右下の角とのベクトル
		vector_x[2] = (box_collider->GetLocation().x 
						+ box_collider->GetArea().width) - location[0].x;
		vector_y[2] = (box_collider->GetLocation().y 
						+ box_collider->GetArea().height) - location[0].y;

		//各頂点と外積の計算
		for (int i = 0, n = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				outer_product[n++] = (vector_x[0] * vector_y[i]) 
					                    - (vector_y[0] * vector_x[j]);
			}
		}

		for (int i = 0; i < 4; i++)
		{
			if (outer_product[i] == 0) //外積の値が0なら当たっている
			{
				ret = true;
				break;
			}

			if (i == 0)
			{
				//右上の角との外積の符号の取得
				sign = signbit(outer_product[0]);
			}
			else
			{
			
				if (sign != signbit(outer_product[i]))	//左上の角との外積の符号が違ったら当たっている
				{
					ret = true;
					break;
				}
			}
		}
	}
	
	return ret;
}

//-----------------------------------
// LineColliderとの当たり判定
//-----------------------------------
bool LineCollider::HitLine(LineCollider* line_collider)
{

	bool ret = false; //返り値

	/*自分のLineColliderの始点を起点にして出たベクトル*/
	float vector_x[3]; //X座標のベクトル
	float vector_y[3]; //Y座標のベクトル

	float outer_product[2]; //外積

	//自分のLineColliderの始点と終点とのベクトルの計算
	vector_x[0] = location[1].x - location[0].x;
	vector_y[0] = location[1].y - location[0].y;

	//自分のLineColliderの始点と相手のLineColliderの始点とのベクトルの計算
	vector_x[1] = line_collider->GetLocation(0).x - location[0].x;
	vector_y[1] = line_collider->GetLocation(0).y - location[0].y;

	//自分のLineColliderの始点と相手のLineColliderの終点とのベクトルの計算
	vector_x[2] = line_collider->GetLocation(1).x - location[0].x;
	vector_y[2] = line_collider->GetLocation(1).y - location[0].y;

	//外積の計算
	for (int i = 0; i < 2; i++)
	{
		outer_product[i] = (vector_x[0] * vector_y[i + 1]) 
			                - (vector_x[i + 1] * vector_y[0]);
	}

	//当たり判定の判断
	if (outer_product[0] * outer_product[1] <= 0.0f)
	{
		ret = true;
	}

	return ret;
}

//-----------------------------------
// 線の端の座標の取得
//-----------------------------------
Location  LineCollider::GetLocation(int i)const
{
	return location[i];
}

//-----------------------------------
// 中心座標の設定
//-----------------------------------
void  LineCollider::SetLocation(Location location, int i)
{
	this->location[i] = location;
}