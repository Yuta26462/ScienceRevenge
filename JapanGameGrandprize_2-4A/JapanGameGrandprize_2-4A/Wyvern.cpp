#include "Wyvern.h"
#include "Player.h"
#include "BulletManager.h"
#include "WyvernBreath.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "CameraWork.h"
#include "EnemySE.h"


//アニメーション
#define WYVERN_ANIMATION 10

//移動スピード
#define WYVERN_MOVE_SPEED 2

//y座標の移動量
#define WYVERN_MOVEMENT_Y 5

//強襲攻撃スピード
#define WYVERN_ASSAULT_SPEED 10

//強襲攻撃で移動できる量
#define WYVERN_ASSAULT_DISTANCE 700
//強襲攻撃の終了距離
#define ASSAULT_END_DISTANCE 50

//強襲攻撃のダメージ
#define WYVERN_ASSAULT_DAMAGE 15

#define WYVERN_MIN_DROP 1
#define WYVERN_DROP 8

//体力
#define WYVERN_HP 150

//ブレスのレート
#define WYVERN_SHOT_RATE 40

//ブレスのインターバル
#define BREATH_INTERVAL 300

//ブレスを打つまでの待機時間
#define BREATH_WAIT_TIME 60

//攻撃が終わって移動するまでの待機時間
#define MOVE_WAIT_TIME 60

//三連ブレスのインターバル
#define TRIPLE_BREATH_INTERVAL 600

//強襲攻撃のインターバル
#define ASSAULT_INTERVAL 480

//攻撃のインターバル
#define WYVERN_ATTACK_INTERVAL 180

//強襲攻撃の終了時間
#define ASSAULT_END_TIME 90

//-----------------------------------
//コンストラクタ
//-----------------------------------
Wyvern::Wyvern(Location spawn_location)
{
	/*初期化*/
	can_delete = false;
	left_move = true;
	attack_end = false;
	attack = false;

	hp = WYVERN_HP;
	attack_interval = WYVERN_ATTACK_INTERVAL;
	breath_interval = BREATH_INTERVAL;
	triple_breath_interval = TRIPLE_BREATH_INTERVAL;
	assault_interval = ASSAULT_INTERVAL;
	breath_wait_time = 0;
	move_wait_time = 0;
	animation = 0;
	shot_rate = 0;
	shot_count = 0;
	assault_speed.x = 0;
	assault_speed.y = 0;
	attack_angle = 0;
	image_argument = 0;
	speed = 0;
	kind = ENEMY_KIND::WYVERN;
	state = ENEMY_STATE::IDOL;
	attack_state = WYVERN_ATTACK::NONE;
	now_assault = false;
	drop_volume = 0;
	poison_time = 0;
	poison_damage = 0;
	paralysis_time = 0;
	location = spawn_location;
	assault_start = location;
	assault_end_time = 0;
	assault_hit_stage = 0;
	/*当たり判定の設定*/
	area.width = 40;
	area.height = 80;

	location.x -= MAP_CHIP_SIZE / 2;
	location.y -= MAP_CHIP_SIZE / 2;
	//ドロップアイテムの設定
	drop_element = new ElementItem * [FIRE_DROP];
	drop_type_volume = FIRE_DROP;

	int volume = 0;
	for (int i = 0; i < FIRE_DROP; i++)
	{
		volume = WYVERN_MIN_DROP + GetRand(WYVERN_DROP);
		drop_element[i] = new ElementItem(static_cast<ELEMENT_ITEM>(i));
		drop_element[i]->SetVolume(volume);
		drop_volume += volume;
	}

	hit_stage.hit = false;
	hit_stage.chip = nullptr;
	int num = static_cast<int>(kind) - static_cast<int>(ENEMY_KIND::SLIME);

	if (images[num].empty())
	{
	
		images[num].resize(4);

		LoadDivGraph("Images/Enemy/Wyvern1.png", 2, 2, 1, 250, 250, &images[num][0]);
		LoadDivGraph("Images/Enemy/Wyvernattack.png", 2, 2, 1, 250, 250, &images[num][2]);
	}

}

//-----------------------------------
//デストラクタ
//-----------------------------------
Wyvern::~Wyvern()
{
	for (int i = 0; i < WIND_DROP; i++)
	{
		delete drop_element[i];
	}
	delete[] drop_element;

}

//-----------------------------------
//更新
//-----------------------------------
void Wyvern::Update(const Player* player, const Stage* stage)
{
	Location old_location = location;	//前の座標

	switch (state)
	{
	case ENEMY_STATE::IDOL:
		Idol();
		break;
	case ENEMY_STATE::MOVE:
		Move(player->GetLocation());

		hit_stage = HitStage(stage);

		if (hit_stage.hit)
		{
			left_move = !left_move;
			speed = -speed;
		}

		attack_interval--;
		breath_interval--;
		triple_breath_interval--;
		assault_interval--;

		if (attack_interval < 0)
		{
			if (breath_interval < 0) //ブレスに移行
			{
				state = ENEMY_STATE::ATTACK;
				attack_state = WYVERN_ATTACK::BREATH;
				breath_wait_time = BREATH_WAIT_TIME;
				if (location.x < player->GetLocation().x)
				{
					left_move = false;
				}
				else
				{
					left_move = true;
				}
				break;
			}

			if (triple_breath_interval < 0) //トリプルブレスに移行
			{
				state = ENEMY_STATE::ATTACK;
				attack_state = WYVERN_ATTACK::TRIPLE_BRACE;
				breath_wait_time = BREATH_WAIT_TIME;
				if (location.x < player->GetLocation().x)
				{
					left_move = false;
				}
				else
				{
					left_move = true;
				}
				break;
			}

			if (assault_interval < 0) //アサルトに移行
			{
				
				state = ENEMY_STATE::ATTACK;
				attack_state = WYVERN_ATTACK::ASSAULT;
				now_assault = true;
				attack_angle = atan2f((player->GetLocation().y + 10) - location.y,
					(player->GetLocation().x + 10) - location.x);
				assault_speed.x = WYVERN_ASSAULT_SPEED * cosf(attack_angle);
				assault_speed.y = WYVERN_ASSAULT_SPEED * sinf(attack_angle);
				assault_start = location;
				break;
			}
		}
		break;
	case ENEMY_STATE::FALL:
		Fall();
		break;
	case ENEMY_STATE::ATTACK:
		if (!attack_end)
		{
			Attack(player->GetLocation());

			if (attack_state == WYVERN_ATTACK::ASSAULT)
			{
				hit_stage = HitStage(stage);

				if (hit_stage.hit)
				{
					Location distance_location; //xとyそれぞれの強襲攻撃を始めた位置からの距離

					location = old_location;

					distance_location = assault_start - location;

					attack_angle = atan2f(distance_location.y, distance_location.x);
					assault_speed.x = WYVERN_ASSAULT_SPEED * cosf(attack_angle);
					assault_speed.y = WYVERN_ASSAULT_SPEED * sinf(attack_angle);

					assault_hit_stage++;
					now_assault = false;
				}
			}
		}
		else
		{
			move_wait_time--;
			if (move_wait_time < 0)
			{
				attack_end = false;
				state = ENEMY_STATE::MOVE;
				if (left_move)
				{
					speed = -WYVERN_MOVE_SPEED;
				}
				else
				{
					speed = WYVERN_MOVE_SPEED;
				}
			}
		}
		
		break;
	case ENEMY_STATE::DEATH:
		Death();
		break;
	default:
		break;
	}

	Poison();
	Paralysis();
	UpdateDamageLog();
	Animation();
	if (CheckHp() && (state != ENEMY_STATE::DEATH))
	{
		state = ENEMY_STATE::DEATH;
	}
}

//-----------------------------------
//アイドル状態
//-----------------------------------
void Wyvern::Idol()
{
	if (!ScreenOut())
	{
		state = ENEMY_STATE::MOVE;
		if (left_move)
		{
			speed = -WYVERN_MOVE_SPEED;
		}
		else
		{
			speed = WYVERN_MOVE_SPEED;
		}
	}
}

//-----------------------------------
//移動
//-----------------------------------
void Wyvern::Move(const Location player_location)
{
	location.x += speed;

	if ((location.x - CameraWork::GetCamera().x < -(area.width * 2)) || 
		((SCREEN_WIDTH + area.width * 2) < location.x - CameraWork::GetCamera().x))
	{
		left_move = !left_move;
		speed = -speed;
	}

	if (ScreenOut())
	{
		state = ENEMY_STATE::IDOL;
	}
}

//-----------------------------------
//移動時のアニメーション
//-----------------------------------
void Wyvern::Animation()
{
	animation++;

	if (animation % WYVERN_ANIMATION == 0)
	{
		image_argument = ++image_argument % 2;
	}
}

//-----------------------------------
//落下
//-----------------------------------
void Wyvern::Fall()
{

}

//-----------------------------------
//攻撃
//-----------------------------------
void Wyvern::Attack(const Location player_location)
{
	switch (attack_state)
	{
	case WYVERN_ATTACK::BREATH:
		Breath(player_location);
		break;
	case WYVERN_ATTACK::TRIPLE_BRACE:
		TripleBreath(player_location);
		break;
	case WYVERN_ATTACK::ASSAULT:
		Assault();
		break;
	case WYVERN_ATTACK::NONE:
		break;
	default:
		break;
	}

	if(attack_end)
	{
		switch (attack_state) //インターバルの設定
		{
		case WYVERN_ATTACK::BREATH:
			breath_interval = BREATH_INTERVAL;
			break;
		case WYVERN_ATTACK::TRIPLE_BRACE:
			triple_breath_interval = TRIPLE_BREATH_INTERVAL;
			shot_count = 0;
			break;
		case WYVERN_ATTACK::ASSAULT:
			assault_interval = ASSAULT_INTERVAL;
			assault_speed.x = 0;
			assault_speed.y = 0;
			assault_end_time = 0;
			assault_hit_stage = 0;
			break;
		case WYVERN_ATTACK::NONE:
			break;
		default:
			break;
		}
		attack_state = WYVERN_ATTACK::NONE;
		attack_interval = WYVERN_ATTACK_INTERVAL;
		attack = false;
		move_wait_time = MOVE_WAIT_TIME;
	}
}

//-----------------------------------
//ブレス
//-----------------------------------
void Wyvern::Breath(const Location player_location)
{
	breath_wait_time--;

	if (breath_wait_time < 0) //発射待機時間終了
	{
		PlaySoundMem(EnemySE::GetEnemySE(kind).attack, DX_PLAYTYPE_BACK);
		BulletManager::GetInstance()->
			CreateEnemyBullet(new WyvernBreath(location, player_location));
		attack_end = true;
	}
}

//-----------------------------------
//トリプルブレス
//-----------------------------------
void Wyvern::TripleBreath(const Location player_location)
{
	breath_wait_time--;

	if (breath_wait_time < 0) //発射待機時間終了
	{
		shot_rate++;

		if (shot_rate % WYVERN_SHOT_RATE == 0)
		{
			PlaySoundMem(EnemySE::GetEnemySE(kind).attack, DX_PLAYTYPE_BACK);

			BulletManager::GetInstance()->
				CreateEnemyBullet(new WyvernBreath(location, player_location));
			shot_count++;
		}	
	}

	if (3 <= shot_count)
	{
		attack_end = true;
	}
}

//-----------------------------------
//強襲攻撃
//-----------------------------------
void Wyvern::Assault()
{
	float distance = 0; //強襲攻撃を始めた位置からの距離
	Location distance_location; //xとyそれぞれの強襲攻撃を始めた位置からの距離

	location = location + assault_speed;
	distance_location = assault_start - location;

	//強襲攻撃を始めた位置からの距離の計算
	distance = sqrtf(powf(distance_location.x, 2) + powf(distance_location.y, 2));

	if (WYVERN_ASSAULT_DISTANCE < distance)
	{
		float radian; //角度

		radian = atan2f(distance_location.y,distance_location.x);
		assault_speed.x = WYVERN_ASSAULT_SPEED * cosf(radian);
		assault_speed.y = WYVERN_ASSAULT_SPEED * sinf(radian);

		now_assault = false;
	}

	if (!now_assault)
	{
		assault_end_time++;
		if ((distance < ASSAULT_END_DISTANCE) ||
			(ASSAULT_END_TIME < assault_end_time) ||
			(1 < assault_hit_stage) )
		{
			attack_end = true;
		}
	}
}

//-----------------------------------
// 攻撃していない
//-----------------------------------
void Wyvern::AttackNone()
{

}

//-----------------------------------
//攻撃が当たっているか
//-----------------------------------
AttackResource Wyvern::Hit()
{
	AttackResource ret = { 0,nullptr,0 }; //戻り値

	if (!attack)
	{
		Location distance_location; //xとyそれぞれの強襲攻撃を始めた位置からの距離
		float radian; //角度

		attack = true;
		ENEMY_TYPE attack_type[1] = { ENEMY_TYPE::NORMAL };
		ret.damage = WYVERN_ASSAULT_DAMAGE;
		ret.type = attack_type;
		ret.type_count = 1;

		now_assault = false;
		distance_location = assault_start - location;
		radian = atan2f(distance_location.y, distance_location.x);
		assault_speed.x = WYVERN_ASSAULT_SPEED * cosf(radian);
		assault_speed.y = WYVERN_ASSAULT_SPEED * sinf(radian);
	}

	return ret;
}

//-----------------------------------
//死亡
//-----------------------------------
void Wyvern::Death()
{
	can_delete = true;
}

//-----------------------------------
//プレイヤーの弾との当たり判定
//-----------------------------------
void Wyvern::HitBullet(const BulletBase* bullet)
{
	PlayHitBulletSound(bullet->GetAttribute());

	int i;
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

	switch (bullet->GetAttribute())
	{
	case ATTRIBUTE::NORMAL:
		damage = bullet->GetDamage();
		damage_log[i].congeniality = CONGENIALITY::NORMAL;
		break;
	case ATTRIBUTE::EXPLOSION:
		damage = static_cast<int>(bullet->GetDamage() * RESISTANCE_DAMAGE);
		damage_log[i].congeniality = CONGENIALITY::RESISTANCE;
		break;
	case ATTRIBUTE::MELT:
		damage = static_cast<int>(bullet->GetDamage() * WEAKNESS_DAMAGE);
		damage_log[i].congeniality = CONGENIALITY::WEAKNESS;
		break;
	case ATTRIBUTE::POISON:
		damage = bullet->GetDamage();
		damage_log[i].congeniality = CONGENIALITY::RESISTANCE;
		if (!poison)
		{
			poison = true;
			poison_damage = bullet->GetDamageParSecond();
			poison_time = static_cast<int>(bullet->GetDebuffTime() * RESISTANCE_DEBUFF);
		}
		break;
	case ATTRIBUTE::PARALYSIS:
		damage = bullet->GetDamage();
		damage_log[i].congeniality = CONGENIALITY::RESISTANCE;
		if (!paralysis)
		{
			paralysis_time = static_cast<int>(bullet->GetDebuffTime() * RESISTANCE_DEBUFF);
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
//描画
//-----------------------------------
void Wyvern::Draw() const
{
	Location draw_location = location;
	Location camera = CameraWork::GetCamera();
	draw_location = draw_location - camera;
	int num = static_cast<int>(kind) - static_cast<int>(ENEMY_KIND::SLIME);

	if (state != ENEMY_STATE::DEATH)
	{
		DrawHPBar(WYVERN_HP);
	}
	DrawDamageLog();

	DrawWeaknessIcon();
	if (attack_state == WYVERN_ATTACK::ASSAULT)
	{
		if (now_assault)
		{
			DrawRotaGraphF(draw_location.x, draw_location.y,
				0.5, attack_angle, images[num][3], TRUE, !left_move);
		}
		else
		{
			DrawRotaGraphF(draw_location.x, draw_location.y,
				0.5, attack_angle, images[num][image_argument + 2], TRUE, !left_move);
		}
		
	}
	else
	{
		DrawRotaGraphF(draw_location.x, draw_location.y,
			0.5, 0, images[num][image_argument], TRUE, !left_move);
	}
	

	/*DrawBox(draw_location.x - area.width / 2, draw_location.y - area.height / 2,
		draw_location.x + area.width / 2, draw_location.y + area.height / 2, 0xff0000, FALSE);*/
}

//-----------------------------------
//座標の取得
//-----------------------------------
Location Wyvern::GetLocation() const
{
	return location;
}

#ifdef _DEBUG
//-----------------------------------
//更新(DotByDot)
//-----------------------------------
void Wyvern::Update(const ENEMY_STATE state)
{

}

//-----------------------------------
//描画(DotByDot)
//-----------------------------------
void Wyvern::DebugDraw()
{

}
#endif //_DEBUG