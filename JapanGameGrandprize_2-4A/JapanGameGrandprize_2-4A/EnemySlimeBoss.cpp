#include<dxlib.h>
#define _USE_MATH_DEFINES
#include<math.h>
#include"EnemySlimeBoss.h"
#include"Player.h"
#include"Stage/Stage.h"

#include"BulletManager.h"

#define SLIME_MIN_DROP 0u
#define SLIME_MAX_DROP 3u

#define SLIME_BOSS_SPEED 3
#define SLIME_BOSS_JUMP_SPEED 5
#define SLIME_BOSS_ATTACK_DAMAGE 20
#define SLIME_BOSS_JUMP_DISTANCE 40

#define BOSS_SLIME_HP 500

#define SLIME_BOSS_WAIT_TIME 120
#define CLOUD_MOVE_TIME 80
#define SLIME_BOSS_WIDTH 180
#define SLIME_BOSS_HEIGHT 200

EnemySlimeBoss::EnemySlimeBoss(Location spawn_location)
{
	location = spawn_location;
	location.y -= 100;

	area.height = SLIME_BOSS_WIDTH;
	area.width = SLIME_BOSS_HEIGHT;

	state = ENEMY_STATE::FALL;
	now_state = state;

	left_move = true;

	kind = ENEMY_KIND::SLIME_BOSS;

	slime_boss_jump_distance = SLIME_BOSS_JUMP_DISTANCE;

	//仮hp
	hp = 500;
	speed_y = 0;
	speed = 0;


	//ドロップアイテムの設定

	int slime_min_drop = 5;
	int slime_drop = 10;

	drop_element = new ElementItem * [SOIL_DROP];
	drop_type_volume = SOIL_DROP;

	int volume = 0;

	for (int i = 0; i < WATER_DROP; i++)
	{
		volume = slime_min_drop + GetRand(slime_drop);
		drop_element[i] = new ElementItem(static_cast<ELEMENT_ITEM>(2 + i));
		drop_element[i]->SetVolume(volume);
		drop_volume += volume;
	}
	wait_time = 0;

	int num = static_cast<int>(kind) - static_cast<int>(ENEMY_KIND::SLIME);

	if (images[num].empty())
	{
		images[num].resize(1);
		images[num][0] = LoadGraph("Images/Enemy/SlimeBoss3.png");
	}

	magic_circle_image = LoadGraph("Images/Enemy/magic_circle_2.png");

	cloud_brightness = 0;
	attack_type = ATTACKTYPE::NONE;
}

EnemySlimeBoss::~EnemySlimeBoss()
{
	for (int i = 0; i < SOIL_DROP; i++)
	{
		delete drop_element[i];
	}

	delete[] drop_element;


}

void EnemySlimeBoss::MagicBullet(const Location player_location)
{
	if (attack_type == ATTACKTYPE::MAGIC_BULLET)
	{
		BulletManager::GetInstance()->CreateEnemyBullet
		(new SlimeBossBullet(location, player_location));
	}
}


void EnemySlimeBoss::Thunder(const Location player_location)
{
	BulletManager::GetInstance()->CreateEnemyBullet
	(new SlimeBossThunder(cloud_location));
}

void EnemySlimeBoss::Update_Cloud(const Player* player)
{
	if (attack_type == ATTACKTYPE::THUNDER)
	{
		if (cloud_brightness <= 255)cloud_brightness += 5;
		else cloud_brightness = 255;

		if (++cloud_move_time < CLOUD_MOVE_TIME)
		{
			cloud_location = player->GetLocation();
			cloud_location.y = 280;
		}
	}
	else
	{
		cloud_brightness = 0;
		cloud_location = player->GetLocation();
		cloud_location.y = 280;
	}
}


void EnemySlimeBoss::Update(const Player* player, const Stage* stage)
{
	Update_Cloud(player);

	state = now_state;

	Location old_location = location;	//前の座標
	HitMapChip hit_stage = { false,nullptr }; //ステージとの当たり判定

	switch (state)
	{
	case ENEMY_STATE::IDOL:
		Idol(player);
		break;

	case ENEMY_STATE::MOVE:

		if (left_move)speed = -SLIME_BOSS_SPEED;
		else speed = SLIME_BOSS_SPEED;

		speed_y = -(slime_boss_jump_distance / 3);
		if (--slime_boss_jump_distance <= 0)
		{
			state = ENEMY_STATE::FALL;
			speed_y = 0;
			slime_boss_jump_distance = SLIME_BOSS_JUMP_DISTANCE;
		}

		location.y += speed_y;

		hit_stage = HitStage(stage);

		if (hit_stage.hit) //ステージとの当たり判定
		{
			Location chip_location = hit_stage.chip->GetLocation();
			Area chip_area = hit_stage.chip->GetArea();

			STAGE_DIRECTION hit_direction; //当たったステージブロックの面
			hit_direction = HitDirection(hit_stage.chip);

			location.y = old_location.y;
		}
		Move(player->GetLocation());

		hit_stage = HitStage(stage);

		if (hit_stage.hit) //ステージとの当たり判定
		{
			STAGE_DIRECTION hit_direction; //当たったステージブロックの面
			hit_direction = HitDirection(hit_stage.chip);

			location.x = old_location.x;
			left_move = !left_move;
		}
		if (state == ENEMY_STATE::MOVE)
		{
			Location scroll = location - CameraWork::GetCamera();
			Area harea = { area.height / 2 + 20,area.width / 2 };

			if (scroll.x - harea.width < 10 || SCREEN_WIDTH < scroll.x + harea.width)
			{
				location.x = old_location.x;
				left_move = !left_move;
			}
		}
		
		break;

	case ENEMY_STATE::FALL:

		if (speed != 0)
		{
			if (left_move)speed = -SLIME_BOSS_SPEED;
			else speed = SLIME_BOSS_SPEED;
		}

		location.x += speed;
		
		hit_stage = HitStage(stage);

		if (hit_stage.hit) //ステージとの当たり判定
		{
			STAGE_DIRECTION hit_direction; //当たったステージブロックの面
			hit_direction = HitDirection(hit_stage.chip);

			location.x = old_location.x;
			left_move = !left_move;	
		}
		if (state == ENEMY_STATE::FALL)
		{
			Location scroll = location - CameraWork::GetCamera();
			Area harea = { area.height / 2 + 20,area.width / 2 };

			if (scroll.x - harea.width < 10 || SCREEN_WIDTH < scroll.x + harea.width)
			{
				location.x = old_location.x;
				left_move = !left_move;
			}
		}

		Fall();

		hit_stage = HitStage(stage);

		if (hit_stage.hit) //ステージとの当たり判定
		{
			Location chip_location = hit_stage.chip->GetLocation();
			Area chip_area = hit_stage.chip->GetArea();

			STAGE_DIRECTION hit_direction; //当たったステージブロックの面
			hit_direction = HitDirection(hit_stage.chip);

			location.y = old_location.y;

			state = ENEMY_STATE::IDOL;

			wait_time = SLIME_BOSS_WAIT_TIME;

		}

		break;

	case ENEMY_STATE::ATTACK:

		break;

	case ENEMY_STATE::DEATH:
		Death();
		break;

	default:
		break;
	}

	if (CheckHp() && state != ENEMY_STATE::DEATH)
	{
		state = ENEMY_STATE::DEATH;
	}

	now_state = state;
	state = ENEMY_STATE::ATTACK;

	UpdateDamageLog();
}

void EnemySlimeBoss::Draw()const
{
	Location draw_location = location;
	Location camera = CameraWork::GetCamera();
	draw_location = draw_location - camera;

	int num = static_cast<int>(kind) - static_cast<int>(ENEMY_KIND::SLIME);

	//画面内に映ってからHPを描画する
	if (state != ENEMY_STATE::DEATH && -MAP_CHIP_SIZE < draw_location.y - area.height / 2)
	{
		DrawHPBar(BOSS_SLIME_HP);
		DrawDamageLog();
		DrawWeaknessIcon();
	}
	

	//DrawBox(draw_location.x - (SLIME_BOSS_WIDTH / 2), draw_location.y - (SLIME_BOSS_HEIGHT / 2), draw_location.x + (SLIME_BOSS_WIDTH / 2), draw_location.y + (SLIME_BOSS_HEIGHT / 2), 0xffffff, FALSE);
	DrawRotaGraph(draw_location.x, draw_location.y, 1, 0, images[num][0], true, !left_move);

	//DrawGraph(100, 200, magic_circle_image, true);
	//DrawModiGraph(100, 200, 300, 200, 300, 240, 100, 240, magic_circle_image, true);

	//DrawFormatString(0, 0, 0xffffff, "%d", draw_location.x);

	Draw_Cloud();
}

void EnemySlimeBoss::Draw_Cloud()const
{
	if(attack_type == ATTACKTYPE::THUNDER)
	{
		Location draw_location = cloud_location;
		Location camera = CameraWork::GetCamera();
		draw_location = draw_location - camera;

		double work = cloud_brightness;
		double size = (work / 255);
		if (size >= 1)size = 1;
		
		SetDrawBright(cloud_brightness, cloud_brightness, cloud_brightness);
		DrawRotaGraph(draw_location.x, draw_location.y, size, 0, magic_circle_image, true, false);

		SetDrawBright(255, 255, 255);
	}
}

//-----------------------------------
//アイドル状態
//-----------------------------------
void EnemySlimeBoss::Idol(const Player* player)
{
	if (!ScreenOut())
	{
		if (wait_time == SLIME_BOSS_WAIT_TIME / 2)
		{
			if (attack_type == ATTACKTYPE::MAGIC_BULLET)
			{
				MagicBullet(player->GetLocation());	
			}
			else if(attack_type == ATTACKTYPE::THUNDER)
			{
				
				Thunder(player->GetLocation());
			}
		}

		if (--wait_time <= 0)
		{
			wait_time = 0;
			state = ENEMY_STATE::MOVE;

			if (attack_type == ATTACKTYPE::NONE)attack_type = ATTACKTYPE::MAGIC_BULLET;
			else if (attack_type == ATTACKTYPE::MAGIC_BULLET)
			{
				attack_type = ATTACKTYPE::THUNDER;
				cloud_move_time = 0;
			}
			else attack_type = ATTACKTYPE::MAGIC_BULLET;
		}	
	}
}

//-----------------------------------
//移動
//-----------------------------------
void EnemySlimeBoss::Move(const Location player_location)
{
	location.x += speed;
}

//-----------------------------------
//落下
//-----------------------------------
void EnemySlimeBoss::Fall()
{
	location.y += speed_y;

	if (speed_y < GRAVITY)
	{
		speed_y += ENEMY_FALL_SPEED;
	}
}

//-----------------------------------
//攻撃
//-----------------------------------
void  EnemySlimeBoss::Attack(Location player_location)
{
}

//-----------------------------------
//攻撃が当たっているか
//-----------------------------------
AttackResource EnemySlimeBoss::Hit()
{
	AttackResource ret = { 0,nullptr,0 }; //戻り値

	ENEMY_TYPE attack_type[1] = { ENEMY_TYPE::WATER };
	ret.damage = SLIME_BOSS_ATTACK_DAMAGE;
	ret.type = attack_type;
	ret.type_count = 1;

	return ret;
}

//-----------------------------------
//死亡
//-----------------------------------
void EnemySlimeBoss::Death()
{
	can_delete = true;
}

void EnemySlimeBoss::DrawHPBar(const int max_hp) const
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
// プレイヤーの弾との当たり判定
//-----------------------------------
void EnemySlimeBoss::HitBullet(const BulletBase* bullet)
{
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

	PlayHitBulletSound(bullet->GetAttribute());

	switch (bullet->GetAttribute())
	{
	case ATTRIBUTE::NORMAL:
		damage = bullet->GetDamage() * RESISTANCE_DAMAGE;
		damage_log[i].congeniality = CONGENIALITY::RESISTANCE;
		break;
	case ATTRIBUTE::EXPLOSION:
		damage = bullet->GetDamage() * WEAKNESS_DAMAGE;
		damage_log[i].congeniality = CONGENIALITY::WEAKNESS;
		break;
	case ATTRIBUTE::MELT:
		damage = bullet->GetDamage() * WEAKNESS_DAMAGE;
		damage_log[i].congeniality = CONGENIALITY::WEAKNESS;
		break;
	case ATTRIBUTE::POISON:
		if (!poison)
		{
			poison_damage = bullet->GetDamage() * 0;
			poison_time = bullet->GetDebuffTime() * 0;
		}
		break;
	case ATTRIBUTE::PARALYSIS:
		damage = bullet->GetDamage();
		damage_log[i].congeniality = CONGENIALITY::NOMAL;
		if (!paralysis)
		{
			paralysis = true;
			paralysis_time = bullet->GetDebuffTime();
			speed *= PARALYSIS_SPEED;
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
//座標の取得
//-----------------------------------
Location EnemySlimeBoss::GetLocation() const
{
	return location;
}

#ifdef _DEBUG
//-----------------------------------
// 更新(DotByDot)
//-----------------------------------
void EnemySlimeBoss::Update(const ENEMY_STATE state)
{
	if ((old_state != state))
	{
		location = debug_location;
	}


	old_state = state;
}

//-----------------------------------
//描画(DotByDot)
//-----------------------------------
void EnemySlimeBoss::DebugDraw()
{
	Location draw_location = location;
	Location camera = CameraWork::GetCamera();
	draw_location = draw_location - camera;

	DrawCircle(draw_location.x, draw_location.y, 15, 0xff0000, true, true);
}
#endif //_DEBUG

//#ifdef _DEBUG
////-----------------------------------
//// 更新(DotByDot)
////-----------------------------------
//void EnemySlime::Update(const ENEMY_STATE state)
//{
//	if ((old_state != state) || (attack_time < 0))
//	{
//		location = debug_location;
//	}
//	switch (state)
//	{
//	case ENEMY_STATE::IDOL:
//		break;
//	case ENEMY_STATE::MOVE:
//		if (++image_change_time > 2)
//		{
//			image_type += image_addition;
//			if (image_type == 6)
//			{
//				image_addition = -1;
//			}
//			else if (image_type == 0)
//			{
//				image_addition = 1;
//			}
//			image_change_time = 0;
//		}
//		break;
//	case ENEMY_STATE::FALL:
//		break;
//	case ENEMY_STATE::ATTACK:
//		if ((old_state != state) || (attack_time < 0))
//		{
//			jump_distance.y = SLIME_ATTACK_DISTANCE_Y;
//			attack_time = SLIME_ATTACK_DISTANCE_Y * 3;
//			location = debug_location;
//		}
//		if (SLIME_ATTACK_DISTANCE_Y < attack_time)
//		{
//			location.y -= (jump_distance.y / 3);
//			jump_distance.y--;
//			if (left_move)
//			{
//				speed = -SLIME_ATTACK_SPEED;
//			}
//			else
//			{
//				speed = SLIME_ATTACK_SPEED;
//			}
//			location.x += speed;
//
//		}
//		attack_time--;
//
//		break;
//	case ENEMY_STATE::DEATH:
//		break;
//	default:
//		break;
//	}
//	old_state = state;
//}
//
////-----------------------------------
////描画(DotByDot)
////-----------------------------------
//void EnemySlime::DebugDraw()
//{
//	DrawRotaGraphF(location.x, location.y, 0.23,
//		M_PI / 180 * slime_angle, images[image_type], TRUE, !left_move);
//
//	DrawBox(location.x - area.width / 2, location.y - area.height / 2,
//		location.x + area.width / 2, location.y + area.height / 2,
//		0xff0000, FALSE);
//}
//#endif //_DEBUG