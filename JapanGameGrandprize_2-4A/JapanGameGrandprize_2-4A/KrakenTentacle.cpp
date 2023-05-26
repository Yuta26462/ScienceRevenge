#include "KrakenTentacle.h"
#include "DxLib.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "CameraWork.h"

//触手サイズ
#define SIZE_X 100
#define SIZE_Y 40

//触手移動速度
#define ATTACK_SPEED 3

//触手攻撃
#define ATTACK_DAMAGE 30

//ドロップ
#define MIN_DROP 10
#define MAX_DROP 20

//攻撃開始
#define ATTACK_TIME 200

//HP
#define HP_TENTACLE 200

KrakenTentacle::KrakenTentacle(Location spawn_location)
{
	/*初期化*/
	can_delete = false;
	left_move = true;

	hp = HP_TENTACLE;

	image = 0;
	time = 0;
	back_time = 0;
	old_player = 0;
	old_x = 0;
	go_back = false;
	attack = false;
	attack_state = false;
	speed = ATTACK_SPEED;

	kind = ENEMY_KIND::KRAKEN;
	state = ENEMY_STATE::MOVE;

	attack_move = KRAKEN_TENTACLE::NONE;

	drop_volume = 0;
	poison_time = 0;
	poison_damage = 0;
	paralysis_time = 0;
	location = spawn_location;


	/*当たり判定の設定*/
	area.width = SIZE_X;
	area.height = SIZE_Y;

	location.x -= MAP_CHIP_SIZE / 2;
	location.y -= MAP_CHIP_SIZE / 2;
	//ドロップアイテムの設定
	drop_element = new ElementItem * [WATER_DROP];
	drop_type_volume = WATER_DROP;

	int volume = 0;

	for (int i = 0; i < WATER_DROP; i++)
	{
		volume = MIN_DROP + GetRand(MAX_DROP);
		drop_element[i] = new ElementItem(static_cast<ELEMENT_ITEM>(2 + i));
		drop_element[i]->SetVolume(volume);
		drop_volume += volume;
	}

	InitDamageLog();
}

KrakenTentacle::~KrakenTentacle()
{
	for (int i = 0; i < SOIL_DROP; i++)
	{
		delete drop_element[i];
	}

	delete[] drop_element;


}

void KrakenTentacle::Update(const Player* player, const Stage* stage)
{
	HitMapChip hit_stage = { false,nullptr }; //ステージとの当たり判定
	Location old_location = location; //移動前の座標
	switch (state)
	{
	case ENEMY_STATE::IDOL:
		Idol();
		break;
	case ENEMY_STATE::MOVE:
		Move(player->GetLocation());
		break;
	case ENEMY_STATE::FALL:

		break;
	case ENEMY_STATE::ATTACK:
		Attack(player->GetLocation());
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
				go_back = true;
				attack = false;
				attack_state = false;
			}
		}
		break;
	case ENEMY_STATE::DEATH:
		Death();
		break;
	default:
		break;
	}

	hit_stage = HitStage(stage);
	if (hit_stage.hit) //ステージとの当たり判定
	{
		STAGE_DIRECTION hit_direction; //当たったステージブロックの面
		hit_direction = HitDirection(hit_stage.chip);

		if (hit_direction == STAGE_DIRECTION::TOP || hit_direction == STAGE_DIRECTION::BOTTOM)
		{
			location = old_location;
		}
		if ((hit_direction == STAGE_DIRECTION::RIGHT) || (hit_direction == STAGE_DIRECTION::LEFT))
		{
			location = old_location;
		}
	}


	Poison();

	if (CheckHp() && state != ENEMY_STATE::DEATH)
	{
		state = ENEMY_STATE::DEATH;
	}

	UpdateDamageLog();

}

void KrakenTentacle::Draw() const
{
	Location draw_location = location;
	Location camera = CameraWork::GetCamera();
	draw_location = draw_location - camera;

	if (state != ENEMY_STATE::DEATH)
	{
		DrawHPBar(HP_TENTACLE);
	}

	DrawDamageLog();

	DrawBox(draw_location.x - area.width / 2, draw_location.y - area.height / 2,
		draw_location.x + area.width / 2, draw_location.y + area.height / 2,
		GetColor(255, 255, 0), TRUE);

}

void KrakenTentacle::Idol()
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

void KrakenTentacle::Move(const Location player_location)
{

	if (go_back == true)
	{
		float distance = old_x - location.x ;
		float vector = sqrt(distance * distance);
		float radian = distance / vector;

		location.x += radian * 5;


		if (location.x + 10 > old_x && location.x - 10 < old_x)
		{
			go_back = false;
		}
	}
	else
	{
		location.y = player_location.y;

		if (++time % ATTACK_TIME == 0 && attack_state == false)
		{
			old_x = location.x;
			attack_state = true;
			old_player = player_location.x;
			state = ENEMY_STATE::ATTACK;
			attack_move=KRAKEN_TENTACLE::TENTACLE_ATTACK;
		}
	}
	
}

void KrakenTentacle::Fall()
{

}

void KrakenTentacle::Attack(const Location player_location)
{


	float distance = old_player - location.x;
	float distance_y = player_location.y - location.y;
	float vector = sqrt(distance * distance + distance_y * distance_y);
	float radian = distance / vector;
	float radian_y = distance_y / vector;

	

	location.x += radian * speed;
	location.y += radian_y * speed;

	if ( old_player + 10 > location.x && old_player - 10 < location.x)
	{
		go_back = true;
		attack_state = false;
		state = ENEMY_STATE::MOVE;
	}


}

AttackResource KrakenTentacle::Hit()
{
	AttackResource ret = { 0,nullptr,0 }; //戻り値

	if (attack_move == KRAKEN_TENTACLE::TENTACLE_ATTACK)
	{
		ENEMY_TYPE attack_type[1] = { ENEMY_TYPE::WATER };
		ret.damage = ATTACK_DAMAGE;
		ret.type = attack_type;
		ret.type_count = 1;
	}

	return ret;

}

void KrakenTentacle::Death()
{
	can_delete = true;
}

void KrakenTentacle::HitBullet(const BulletBase* bullet)
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
		damage = bullet->GetDamage() * RESISTANCE_DAMAGE; //効きにくい;
		damage_log[i].congeniality = CONGENIALITY::RESISTANCE;
		break;
	case ATTRIBUTE::EXPLOSION:
		damage = bullet->GetDamage() * WEAKNESS_DAMAGE;
		damage_log[i].congeniality = CONGENIALITY::WEAKNESS;
		break;
	case ATTRIBUTE::MELT:
		damage = bullet->GetDamage();
		damage_log[i].congeniality = CONGENIALITY::NORMAL;
		break;
	case ATTRIBUTE::POISON:
		if (!poison)
		{
			poison = true;
			poison_damage = bullet->GetDamage() * 0;
			poison_time = bullet->GetDebuffTime() * 0;
		}
		break;
	case ATTRIBUTE::PARALYSIS:
		damage = bullet->GetDamage() * RESISTANCE_DAMAGE;
		damage_log[i].congeniality = CONGENIALITY::RESISTANCE;
		if (!paralysis)
		{
			paralysis_time = bullet->GetDebuffTime() * RESISTANCE_DEBUFF;
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
}

Location KrakenTentacle::GetLocation() const
{
	return Location();
}

#ifdef _DEBUG
void KrakenTentacle::Update(const ENEMY_STATE state)
{
	switch (state)
	{
	case ENEMY_STATE::IDOL:
		break;
	case ENEMY_STATE::MOVE:
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

void KrakenTentacle::DebugDraw()
{
	DrawBox(location.x - area.width / 2, location.y - area.height / 2,
		location.x + area.width / 2, location.y + area.height / 2,
		0xff0000, FALSE);
}
#endif // _DEBUG

