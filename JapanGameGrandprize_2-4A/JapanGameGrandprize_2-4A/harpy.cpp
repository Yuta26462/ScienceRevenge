#include "Harpy.h"
#include "CameraWork.h"
#include "BulletManager.h"
#include "DxLib.h"

//ハーピィの画像サイズ
#define HARPY_SIZE_X 90
#define HARPY_SIZE_Y 98

//プレイヤー発見距離
#define DETECTION_DISTANCE 300
#define DETECTION_DISTANCE_Y 250

//攻撃範囲
#define ATTACK_RANGE 150

//魔法攻撃した時の硬直時間
#define MAGIC_STANDBY 60

//近接攻撃した時の硬直時間
#define PHYSICAL_STANDBY 100

//ハーピィの攻撃力
#define HARPY_ATTACK_DAMAGE 4

//復帰
#define Standby 2

//移動スピード
#define SPEED 2.5f

//攻撃スピード
#define ATTACK_SPEED 5

//ドロップ量(最小)
#define HARPY_MIN_DROP 1

//ドロップ量(最大)
#define HARPY_DROP 6

#define HARPY_HP 40
//-----------------------------------
// コンストラクタ
//-----------------------------------
Harpy::Harpy(Location spawn_location)
{

	can_delete = false;
	left_move = true;
	attack = false;
	go_back = false;

	hp = HARPY_HP;
	time = 0;
	old_x = 0;
	old_y = 0;
	standby_num = 0;
	physical_time = 0;
	magic_num = 0;
	magic_time = 1;
	animation = 0;
	animation_time = 0;
	location = spawn_location;
	//位置調整
	location.x -= 10;

	standby_attack = 0;
	travel = 0;
	travel_y = 0;
	range = 0;
	range_y = 0;
	vector = 0;
	speed = SPEED;
	area.width = HARPY_SIZE_X;
	area.height = HARPY_SIZE_Y;

	location.x -= MAP_CHIP_SIZE / 2;
	location.y -= MAP_CHIP_SIZE / 2;

	standby_time = 0;
	physical_attack = false;
	magic_attack = false;
	kind = ENEMY_KIND::HARPY;


	//ドロップアイテムの設定
	drop_element = new ElementItem * [WIND_DROP];
	drop_type_volume = WIND_DROP;

	int volume = 0;
	for (int i = 0; i < WIND_DROP; i++)
	{
		volume = HARPY_MIN_DROP + GetRand(HARPY_DROP);
		drop_element[i] = new ElementItem(static_cast<ELEMENT_ITEM>(i));
		drop_element[i]->SetVolume(volume);
		drop_volume += volume;
	}

	attack_state = HARPY_ATTACK::NONE;
	state = ENEMY_STATE::IDOL;
	action_type = HARPY_STATE::NORMAL;

	
	int num = static_cast<int>(kind) - static_cast<int>(ENEMY_KIND::SLIME);

	if (images[num].empty())
	{
		images[num].resize(6);
		LoadDivGraph("Images/Enemy/HarpleImage.png", 6, 6, 1, 250, 250, &images[num][0]); //通常
	}
	GetGraphSizeF(images[num][0], &size.width, &size.height);
}

//-----------------------------------
//デストラクタ
//-----------------------------------
Harpy::~Harpy()
{
	for (int i = 0; i < WIND_DROP; i++)
	{
		delete drop_element[i];
	}
	delete[] drop_element;
}

//-----------------------------------
// 描画以外の処理
//-----------------------------------
void Harpy::Update(const class Player* player, const class Stage* stage)
{

	if (paralysis == true)
	{
		speed = PARALYSIS_SPEED;
	}
	else
	{
		speed = SPEED;
	}

	//アニメーション
	if (++animation_time % 10 == 0)
	{
		--animation;
	}

	if (animation < 0)
	{
		animation = 5;
	}

	Location old_location = location;	//前の座標
	HitMapChip hit_stage = { false,nullptr }; //ステージとの当たり判定

	switch (state)
	{
	case ENEMY_STATE::IDOL:
		Idol();
		break;
	case ENEMY_STATE::MOVE:
		Move(player->GetLocation());
		break;
	case ENEMY_STATE::FALL:
		Fall();

		hit_stage = HitStage(stage);

		if (hit_stage.hit) //ステージとの当たり判定
		{
			Location chip_location = hit_stage.chip->GetLocation();
			Area chip_area = hit_stage.chip->GetArea();

			location.y = chip_location.y -
				(chip_area.height / 2) - (area.height / 2);

			STAGE_DIRECTION hit_direction; //当たったステージブロックの面
			hit_direction = HitDirection(hit_stage.chip);

			if (hit_direction == STAGE_DIRECTION::TOP)
			{
				state = ENEMY_STATE::MOVE;

			}
		}

		break;
	case ENEMY_STATE::ATTACK:
		Attack(player->GetLocation());
		if (physical_attack == true)
		{
			PhysicalMove(player->GetLocation());
		}
		break;
	case ENEMY_STATE::DEATH:
		Death();
		break;
	default:
		break;
	}


	if (state != ENEMY_STATE::IDOL)
	{
		hit_stage = HitStage(stage);
		if (hit_stage.hit) //ステージとの当たり判定
		{
			STAGE_DIRECTION hit_direction; //当たったステージブロックの面
			hit_direction = HitDirection(hit_stage.chip);

			if (hit_direction == STAGE_DIRECTION::TOP)
			{
				location = old_location;
			}
			if ((hit_direction == STAGE_DIRECTION::RIGHT) || (hit_direction == STAGE_DIRECTION::LEFT))
			{
				location = old_location;

				if (state != ENEMY_STATE::ATTACK)
				{
					left_move = !left_move;
				}
			}

		}
	}

	if (CheckHp() && state != ENEMY_STATE::DEATH)
	{
		state = ENEMY_STATE::DEATH;
	}

	if (poison == true)
	{
		Poison();
	}

	UpdateDamageLog();
	Paralysis();

	if (ScreenOut())
	{
		state = ENEMY_STATE::IDOL;
	}

}

//アイドル状態
void Harpy::Idol()
{

	Location scroll; //画面スクロールを考慮したX座標
	Location camera = CameraWork::GetCamera(); //カメラ
	scroll = location - camera;

	if ((-area.width < scroll.x) && (scroll.x < SCREEN_WIDTH + area.width) &&
		(-area.height < scroll.y) && (scroll.y < SCREEN_HEIGHT + area.height))
	{
		state = ENEMY_STATE::MOVE;
	}
}

//移動
void Harpy::Move(const Location player_location)
{
	if (go_back == true)
	{
		Comeback();
	}
	else
	{
		//プレイヤーとの距離計算
		range = player_location.x - location.x;
		range_y = player_location.y - location.y;


		//プレイヤーが発見距離内にいたら
		if (range <= DETECTION_DISTANCE && range >= -DETECTION_DISTANCE &&
			range_y <= DETECTION_DISTANCE_Y && range_y >= -DETECTION_DISTANCE_Y)
		{
			//接近攻撃
			if (range <= ATTACK_RANGE && range >= -ATTACK_RANGE)
			{

				state = ENEMY_STATE::ATTACK;
				attack_state = HARPY_ATTACK::PHYSICAL_ATTACK;
				standby_time = PHYSICAL_STANDBY;
				physical_attack = true;

			}
			//遠距離攻撃
			else
			{

				state = ENEMY_STATE::ATTACK;
				attack_state = HARPY_ATTACK::MAGIC_ATTACK;
				standby_time = MAGIC_STANDBY;
				magic_attack = true;

				//発射間隔
				if (magic_time++ % 2 == 0)
				{
					//弾の生成
					magic_num++;
					BulletManager::GetInstance()->CreateEnemyBullet
					(new HarpyBullet(location, player_location));
				}
			}
		}
		else //発見距離にプレイヤーがいなかったら。通常移動
		{
			if (left_move == true)
			{
				action_type = HARPY_STATE::NORMAL; //左
			}
			else
			{
				action_type = HARPY_STATE::NORMAL_RIGHT; //右
			}
			switch (action_type)
			{
			case HARPY_STATE::NORMAL:  //通常移動
				location.x -= speed;
				break;
			case HARPY_STATE::NORMAL_RIGHT://右
				location.x += speed;
				break;
			case HARPY_STATE::NONE:
				break;
			default:
				break;
			}
		}

		if (magic_num > 3)
		{
			state = ENEMY_STATE::FALL;
			magic_num = 0;
			old_x = location.x;
			old_y = location.y;
			standby_time = 200; //地上におりて硬直する時間
			go_back = true;
		}
	}
}


//-----------------------------------
//落下する前の座標に戻る
//-----------------------------------
void Harpy::Comeback()
{
	standby_time--;
	if (standby_time < 0)
	{
		range = old_x - location.x;
		range_y = old_y - location.y;

		vector = sqrt(range * range + range_y * range_y);

		travel = range / vector;
		travel_y = range_y / vector;
		location.x += travel * 2;
		location.y += travel_y * 2;


		if (old_x+10 > location.x && old_x-10 <location.x  && old_y+10>location.y && old_y-10 < location.y)
		{
			go_back = false;
			speed = SPEED;
			standby_num = 0;
			standby_time = 0;
			state = ENEMY_STATE::MOVE;
		}
	}
}

//-----------------------------------
//攻撃
//-----------------------------------
void  Harpy::Attack(const Location player_location)
{

	standby_time--;
	if (standby_time < 0)
	{
		switch (attack_state)
		{
		case HARPY_ATTACK::PHYSICAL_ATTACK:
			attack = false;
			physical_attack = false;
			attack_state = HARPY_ATTACK::NONE;
			break;
		case HARPY_ATTACK::MAGIC_ATTACK:
			magic_attack = false;
			break;
		case HARPY_ATTACK::NONE:
			break;
		default:
			break;
		}
		standby_time = 0;
		state = ENEMY_STATE::MOVE;
	}
}

//-----------------------------------
//攻撃が当たっているか
//-----------------------------------
AttackResource Harpy::Hit()
{

	AttackResource ret = { 0,nullptr,0 }; //戻り値

	if (attack_state == HARPY_ATTACK::PHYSICAL_ATTACK && (!attack))
	{
		attack = true;
		ENEMY_TYPE attack_type[1] = { ENEMY_TYPE::WIND };
		ret.damage = HARPY_ATTACK_DAMAGE;
		ret.type = attack_type;
		ret.type_count = 1;
	}

	return ret;
}

//-----------------------------------
//死亡
//-----------------------------------
void Harpy::Death()
{

	can_delete = true;
}

//-----------------------------------
// 描画
//-----------------------------------
void Harpy::Draw()const
{
	//スクロールに合わせて描画
	Location draw_location = location;
	Location camera = CameraWork::GetCamera();
	draw_location = draw_location - camera;
	int num = static_cast<int>(kind) - static_cast<int>(ENEMY_KIND::SLIME);

	Area center;

	if (left_move)
	{
		center.width = (size.width / 3)+40;
	}
	else
	{
		center.width = (size.width / 3);
	}
	center.height = (size.height / 3)+20;


	if (state != ENEMY_STATE::DEATH)
	{
		DrawHPBar(HARPY_HP);
	}
	DrawDamageLog();
	DrawWeaknessIcon();

	DrawRotaGraph2F(draw_location.x, draw_location.y, center.width, center.height,
		0.5, 0, images[num][animation], TRUE, !left_move);
}

//-----------------------------------
//落下
//-----------------------------------
void Harpy::Fall()
{

	location.y += speed;

	if (speed < 3)
	{
		speed += ENEMY_FALL_SPEED;
	}
}

//-----------------------------------
// プレイヤーの弾丸との当たり判定
//-----------------------------------
void Harpy::HitBullet(const BulletBase* bullet)
{
	PlayHitBulletSound(bullet->GetAttribute());

	int i = 0;
	int damage = 0;

	for (i = 0; i < LOG_NUM; i++)
	{
		if (!damage_log[i].log)
		{
			break;
		}
	}

	if (LOG_NUM <= i)
	{
		for (i = 0; i < LOG_NUM - 1; i++)
		{
			damage_log[i] = damage_log[i + 1];
		}
		i = LOG_NUM - 1;

	}

	switch (bullet->GetAttribute()) //受けた化合物の属性
	{
	case ATTRIBUTE::NORMAL: //通常弾 
		damage = bullet->GetDamage()* WEAKNESS_DAMAGE;
		damage_log[i].congeniality = CONGENIALITY::WEAKNESS;
		break;
	case ATTRIBUTE::EXPLOSION: //爆発 
		damage = bullet->GetDamage();
		damage_log[i].congeniality = CONGENIALITY::NORMAL;

		break;
	case ATTRIBUTE::MELT: //溶かす 
		damage = bullet->GetDamage() * RESISTANCE_DAMAGE;
		damage_log[i].congeniality = CONGENIALITY::RESISTANCE;

		break;
	case ATTRIBUTE::POISON: //毒　弱点
		damage = bullet->GetDamage();
		damage_log[i].congeniality = CONGENIALITY::WEAKNESS;
		if (!poison)
		{
			poison = true;
			poison_time = bullet->GetDebuffTime() * WEAKNESS_DEBUFF;
			poison_damage = bullet->GetDamageParSecond();
		}
		break;
	case ATTRIBUTE::PARALYSIS: //麻痺 弱点
		damage = bullet->GetDamage() * WEAKNESS_DAMAGE;
		damage_log[i].congeniality = CONGENIALITY::WEAKNESS;
		if (!paralysis)
		{
			paralysis = true;
			paralysis_time = static_cast<int>(bullet->GetDebuffTime() * WEAKNESS_DEBUFF);
		}
		break;
	case ATTRIBUTE::HEAL:
		break;
	default:
		break;
	}

	damage_log[i].log = true;
	damage_log[i].time = LOG_TIME;
	damage_log[i].damage = damage;
	hp -= damage;

	if (hp < 0)
	{
		hp = 0;
	}
}

//-----------------------------------
//接近攻撃（物理攻撃）時の動き
//-----------------------------------
void Harpy::PhysicalMove(const Location player_location)
{

	range = player_location.x - location.x;
	range_y = player_location.y - location.y;

	vector = sqrt(range * range + range_y * range_y);

	travel = range / vector;
	travel_y = range_y / vector;
	location.x += travel * speed;
	location.y += travel_y * speed;

	if (physical_time++ % 5 == 0)
	{
		magic_num = 0;
	}
}

//-----------------------------------
//座標の取得
//-----------------------------------
Location Harpy::GetLocation() const
{

	return location;
}

#ifdef _DEBUG
//-----------------------------------
// 更新(DotByDot)
//-----------------------------------
void Harpy::Update(const ENEMY_STATE state)
{
	switch (state)
	{
	case ENEMY_STATE::IDOL:
		break;
	case ENEMY_STATE::MOVE:
		//アニメーション
		if (++animation_time % 10 == 0)
		{
			--animation;
		}

		if (animation < 0)
		{
			animation = 5;
		}
		break;
	case ENEMY_STATE::FALL:
		break;
	case ENEMY_STATE::ATTACK:
		break;
	case ENEMY_STATE::DEATH:
		break;
	default:
		break;
	}
}

//-----------------------------------
//描画(DotByDot)
//-----------------------------------
void Harpy::DebugDraw()
{
	int num = static_cast<int>(kind) - static_cast<int>(ENEMY_KIND::SLIME);

	DrawRotaGraphF(location.x, location.y, 1.4f,
		M_PI / 180, images[num][animation], TRUE, !left_move);

	DrawBox(location.x - area.width / 2, location.y - area.height / 2,
		location.x + area.width / 2, location.y + area.height / 2,
		0xff0000, FALSE);
}
#endif //_DEBUG