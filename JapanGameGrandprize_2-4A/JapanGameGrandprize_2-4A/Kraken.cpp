#include "Kraken.h"
#include "DxLib.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "CameraWork.h"
#include "BulletManager.h"

//クラーケンの画像  (画像なし、適当な数字を入れます）
#define KRAKEN_X 250
#define KRAKEN_Y 250

//クラーケン
#define KRAKEN_HP 500 //（HP調節）

//接近攻撃をする範囲
#define ATTACK_SCOPE 100
#define ATTACK_SCOPE_Y 100

//次の攻撃までの時間


//移動スピード
#define MOVE_SPEED 3

//移動時間
#define MOVE_TIME 120

//ドロップ量(最小)
#define MIN_DROP 40

//ドロップ量(最大)
#define MAX_DROP 60

//攻撃力
#define ATTACK_DAMAGE 20 

//体力
#define KRAKEN_HP 600



//-----------------------------------
// コンストラクタ
//-----------------------------------
Kraken::Kraken(Location spawn_location)
{

	/*初期化*/
	can_delete = false;
	left_move = true;

	hp = KRAKEN_HP;

	kind = ENEMY_KIND::KRAKEN;
	state = ENEMY_STATE::IDOL;
	attack_state = KRAKEN_ATTACK::NONE;
	move_state = KRAKEN_STATE::NONE;

	attack_num = 1;
	drop_volume = 0;
	poison_time = 0;
	poison_damage = 0;
	paralysis_time = 0;
	location = spawn_location;

	standby_attack = 0;

	/*当たり判定の設定*/
	area.width = KRAKEN_X;
	area.height = KRAKEN_Y;

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

//-----------------------------------
// デストラクタ
//-----------------------------------
Kraken::~Kraken()
{

	for (int i = 0; i < SOIL_DROP; i++)
	{
		delete drop_element[i];
	}

	delete[] drop_element;


}

//-----------------------------------
// 更新
//-----------------------------------
void Kraken::Update(const Player* player, const Stage* stage)
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

		hit_stage = HitStage(stage);
		if (hit_stage.hit) //ステージとの当たり判定
		{
			STAGE_DIRECTION hit_direction; //当たったステージブロックの面
			hit_direction = HitDirection(hit_stage.chip);

			if ((hit_direction == STAGE_DIRECTION::RIGHT) || (hit_direction == STAGE_DIRECTION::LEFT))
			{
				location = old_location;
				left_move = !left_move;
			}
		}

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
		break;
	case ENEMY_STATE::DEATH:
		Death();
		break;
	default:
		break;
	}

	Poison();

	if (CheckHp() && state != ENEMY_STATE::DEATH)
	{
		state = ENEMY_STATE::DEATH;
	}

	UpdateDamageLog();
}


//-----------------------------------
//アイドル状態
//-----------------------------------
void  Kraken::Idol()
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

//-----------------------------------
//移動
//-----------------------------------
void Kraken::Move(const Location player_location)
{

	int range = player_location.x - location.x;
	int range_y = player_location.y - location.y;

	//接近攻撃
	if (range <= ATTACK_SCOPE && range >= -ATTACK_SCOPE &&
		range_y <= ATTACK_SCOPE_Y && range_y >= -ATTACK_SCOPE_Y)
	{
		state = ENEMY_STATE::ATTACK;
		attack_state = KRAKEN_ATTACK::TENTACLE_ATTACK;
	}

		switch (attack_num)
		{
		case 0:
			attack_state = KRAKEN_ATTACK::BREATH;
			state = ENEMY_STATE::ATTACK;
			standby_attack = 40;
			break;
		case 1:
			attack_state = KRAKEN_ATTACK::HARD_ATTACK;
			state = ENEMY_STATE::ATTACK;
			standby_attack = 200;
			break;
		default:
			break;
		}

}

//-----------------------------------
//落下
//-----------------------------------
void Kraken::Fall()
{

	location.y += speed;
	if (speed < GRAVITY)
	{
		speed += ENEMY_FALL_SPEED;
	}
}

//-----------------------------------
//攻撃
//-----------------------------------
void  Kraken::Attack(const Location player_location)
{
	--standby_attack;
	if (standby_attack < 0)
	{
		switch (attack_state)
		{
		case KRAKEN_ATTACK::TENTACLE_ATTACK: //触手攻撃
			state = ENEMY_STATE::MOVE;
			break;
		case KRAKEN_ATTACK::BREATH: //ブレス攻撃
			AttackBreath(player_location);
			break;
		case KRAKEN_ATTACK::HARD_ATTACK: //水の塊を落とす
			AttackWater(player_location);
			attack_num = GetRand(1);
			state = ENEMY_STATE::MOVE;
			move_state = KRAKEN_STATE::NORMAL;
			break;
		case KRAKEN_ATTACK::NONE: //ノーマル
		default:
			break;
		}
	}
}

//-----------------------------------
//攻撃(水の塊を落とす）
//-----------------------------------
void Kraken::AttackWater(Location player_location)
{
	BulletManager::GetInstance()->CreateEnemyBullet
	(new KrakenBullet(location, player_location));

}


//-----------------------------------
//遠距離攻撃（ブレス攻撃）
//-----------------------------------
void Kraken::AttackBreath(const Location player_location)
{
	
		BulletManager::GetInstance()->CreateEnemyBullet
		(new KrakenBreath(location, player_location));
		
		attack_num = GetRand(1);
		state = ENEMY_STATE::MOVE;
		move_state = KRAKEN_STATE::NORMAL;
	
}


//-----------------------------------
//攻撃が当たっているか
//-----------------------------------
AttackResource Kraken::Hit()
{

	//触手出現させてみる？

	AttackResource ret = { 0,nullptr,0 }; //戻り値

	if (attack_state == KRAKEN_ATTACK::TENTACLE_ATTACK)
	{
		ENEMY_TYPE attack_type[1] = { ENEMY_TYPE::WATER };
		ret.damage = ATTACK_DAMAGE;
		ret.type = attack_type;
		ret.type_count = 1;
	}

	return ret;
}

//-----------------------------------
//死亡
//-----------------------------------
void Kraken::Death()
{

	can_delete = true;
}

//-----------------------------------
// プレイヤーの弾との当たり判定
//-----------------------------------
void Kraken::HitBullet(const BulletBase* bullet)
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

	if (hp < 0)
	{
		hp = 0;
	}
}

void Kraken::DrawHPBar(const int max_hp) const
{
	int color = GetColor(7, 255, 0);

	if (hp <= (max_hp / 2))
	{
		color = GetColor(255, 255 * static_cast<float>(hp) / max_hp, 0);
	}
	else
	{
		color = GetColor(7 + 2 * (248 * (1 - static_cast<float>(hp) / max_hp)), 255, 0);
	}

	DrawBox(160, 10, SCREEN_WIDTH - 160, 40, 0x000000, TRUE);
	DrawBox(160, 10, 160 + (960 * (static_cast<float>(hp) / max_hp)), 40, color, TRUE);
	DrawBox(160, 10, SCREEN_WIDTH - 160, 40, 0x8f917f, FALSE);
}


//-----------------------------------
// 描画
//-----------------------------------
void Kraken::Draw() const
{

	Location draw_location = location;
	Location camera = CameraWork::GetCamera();
	draw_location = draw_location - camera;

	if (location.x - area.width / 2 < SCREEN_WIDTH && state != ENEMY_STATE::DEATH)
	{
		DrawHPBar(KRAKEN_HP);
	}

	DrawDamageLog();
	DrawWeaknessIcon();

	DrawBox(draw_location.x - area.width / 2, draw_location.y - area.height / 2,
		draw_location.x + area.width / 2, draw_location.y + area.height / 2,
		GetColor(255, 255, 0), TRUE);

}

//-----------------------------------
//座標の取得
//-----------------------------------
Location Kraken::GetLocation() const
{

	return location;
}

#ifdef _DEBUG
//-----------------------------------
// 更新(DotByDot)
//-----------------------------------
void Kraken::Update(const ENEMY_STATE state)
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

//-----------------------------------
//描画(DotByDot)
//-----------------------------------
void Kraken::DebugDraw()
{

}
#endif //_DEBUG
