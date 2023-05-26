#include "Dragon.h"
#include "CameraWork.h"
#include"BulletManager.h"
#include "DxLib.h"


//ドラゴンの画像サイズ(未定、画像が出来次第調整）
#define DRAGON_SIZE_X 330
#define DRAGON_SIZE_Y 330

//ドラゴンのHP
#define HIT_POINTS 3000

//ドラゴンの移動速度
#define ATTACK_SPEED 3
#define SPEED 10

//ドラゴン尻尾攻撃
#define ATTACK_SCOPE 100
#define ATTACK_SCOPE_Y 10

//魔法攻撃した時の硬直時間
#define MAGIC_STANDBY 60

//近接攻撃した時の硬直時間
#define PHYSICAL_STANDBY 100

//咆哮後の硬直時間
#define	ROAR_TIME 130

//雷の数
#define THUNDER 3

//ドラゴンの攻撃力(攻撃別）
//尻尾攻撃
#define ATTACK_TAIL 20

//噛みつく
#define ATTACK_DITE 30 

//接近攻撃の範囲
#define MELEE_ATTACK 150

//攻撃切り替え時間
#define ATTACK_SWITCHOVER 10

//ブレス発射間隔
#define BREATH_INTERVAL 120

//ドロップ量(最小)
#define MIN_DROP 40

//ドロップ量(最大)
#define MAX_DROP 60

Dragon::Dragon(Location spawn_location)
{
	location = spawn_location;
	//location.y = 450;
	location.x = 17680;
	area.height = DRAGON_SIZE_Y;
	area.width = DRAGON_SIZE_X;

	hp = HIT_POINTS;
	speed = SPEED;

	animation = 0;
	attack_method = 0;
	magic_num = 0;
	old_x = 0;
	old_y = 0;
	player_x = 0;
	player_y = 0;
	animation_time = 0;
	switchover_time = 0;
	effect_time = 0;
	standby_time = 0;
	breath_time = 0;
	ascent_time = 0;
	fly_tim = 0;
	fly_num = 0;

	can_delete = false;
	attack_tail = false;
	wall_hit = false;
	left_move = true;
	attack = false;
	magic = false;

	kind = ENEMY_KIND::DRAGON;

	state = ENEMY_STATE::IDOL;

	attack_state = DRAGON_ATTACK::NONE;

	//ドロップアイテムの設定
	drop_element = new ElementItem * [FIRE_DROP];
	drop_type_volume = FIRE_DROP;

	int volume = 0;
	for (int i = 0; i < FIRE_DROP; i++)
	{
		volume = MIN_DROP + GetRand(MAX_DROP);
		drop_element[i] = new ElementItem(static_cast<ELEMENT_ITEM>(2 + i));
		drop_element[i]->SetVolume(volume);
		drop_volume += volume;
	}

	
	

	int num = static_cast<int>(kind) - static_cast<int>(ENEMY_KIND::SLIME);

	if (images[num].empty())
	{
		images[num].resize(2);
		LoadDivGraph("Images/Enemy/doragonwlak.png", 2, 2, 1, 500, 500, &images[num][0]);
		LoadDivGraph("Images/Enemy/dragonfly.png", 2, 2, 1, 260, 260, &images[1][0]); //通常
	}
	LoadDivGraph("Images/Enemy/Doragon/tktk_Other_4L.png", 8, 2, 4, 375, 384, biting_effects);

}

Dragon::~Dragon()
{

	for (int i = 0; i < FIRE_DROP; i++)
	{
		delete drop_element[i];
	}
	delete[] drop_element;
}

void Dragon::Update(const class Player* player, const class Stage* stage)
{
	Location old_location = location;	//前の座標
	HitMapChip hit_stage = { false,nullptr }; //ステージとの当たり判定


	//アニメーション
	if (++animation_time % 10 == 0)
	{
		if (state == ENEMY_STATE::ATTACK)
		{
			++animation;
		}
	}

	if (animation > 1)
	{
		animation = 0;
	}

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
				speed = SPEED;
				fall_speed = 0;
				fly_num = 0;
				fly_tim = 0;
				ascent_time = 0;

			}
		}
		break;
	case ENEMY_STATE::ATTACK:
		Attack(player->GetLocation());
		wall_hit = false;
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

		if (hit_direction == STAGE_DIRECTION::TOP)
		{
			location = old_location;
		}
		if ((hit_direction == STAGE_DIRECTION::RIGHT) || (hit_direction == STAGE_DIRECTION::LEFT))
		{
			location = old_location;
			left_move = !left_move;
			wall_hit = true;

			state = ENEMY_STATE::MOVE;
		}

	}

	if (state != ENEMY_STATE::FALL)
	{
		//画面内座標
		Location s_location = { location.x - CameraWork::GetCamera().x,location.y - CameraWork::GetCamera().y };

		//画面外(横)に出たかの判定
		bool is_out_screen = s_location.x - area.width / 2 < 0 || SCREEN_WIDTH < s_location.x + area.width / 2;

		if (is_out_screen == true)
		{
			location = old_location;
			left_move = !left_move;
			wall_hit = true;

			//state = ENEMY_STATE::MOVE;
		}

		//画面上部から出たら元の位置に戻す
		if (s_location.y - area.height / 2 < 0)
		{
			location = old_location;
		}
	}

	//毒のダメージ
	if (poison == true)
	{
		Poison();
	}

	


	if (CheckHp() && state != ENEMY_STATE::DEATH)
	{
		state = ENEMY_STATE::DEATH;
	}

	UpdateDamageLog();
}

//-----------------------------------
//描画
//-----------------------------------
void Dragon::Draw() const
{
	//スクロールに合わせて描画
	Location draw_location = location;
	Location camera = CameraWork::GetCamera();
	draw_location = draw_location - camera;

	//画面内に映ってからHPを描画する
	if (state != ENEMY_STATE::DEATH && 0 < draw_location.y - area.height / 2)
	{
		DrawHPBar(HIT_POINTS);
		DrawWeaknessIcon();
	}

	int num = static_cast<int>(kind) - static_cast<int>(ENEMY_KIND::SLIME);

	switch (state)
	{
	case ENEMY_STATE::MOVE:
		DrawRotaGraphF(draw_location.x, draw_location.y, 0.7,
			M_PI / 180, images[num][animation], TRUE, !left_move);
		break;
	case ENEMY_STATE::ATTACK:
	
		switch (attack_method)
		{
		case 0:
			DrawRotaGraphF(draw_location.x, draw_location.y, 0.7f,
				M_PI / 180, images[num][animation], TRUE, !left_move);
			if (left_move)
			{
				DrawRotaGraphF(draw_location.x-200, draw_location.y, 2,
					0, biting_effects[frame], TRUE, TRUE);
			}
			else
			{
				DrawRotaGraphF(draw_location.x + 200 , draw_location.y, 2,
					0, biting_effects[frame], TRUE, FALSE);
			}
			break;
		case 1:
			DrawRotaGraphF(draw_location.x, draw_location.y, 1.4f,
				M_PI / 180, images[1][animation], TRUE, !left_move);
			break;
		case 2:
			DrawRotaGraphF(draw_location.x, draw_location.y, 0.7f,
				M_PI / 180, images[num][0], TRUE, !left_move);
			break;
		}

		break;
	default:
		DrawRotaGraphF(draw_location.x, draw_location.y, 0.7f,
			M_PI / 180, images[num][0], TRUE, !left_move);
		break;
	}


	DrawDamageLog();
	

}

//-----------------------------------
//アイドル状態
//-----------------------------------
void Dragon::Idol()
{
	Location scroll; //画面スクロールを考慮したX座標
	Location camera = CameraWork::GetCamera(); //カメラ
	scroll = location - camera;

	if ((-area.width < scroll.x) && (scroll.x < SCREEN_WIDTH + area.width) &&
		(-area.height < scroll.y) && (scroll.y < SCREEN_HEIGHT + area.height))
	{
		state = ENEMY_STATE::FALL;
	}
}

//-----------------------------------
//動き
//-----------------------------------
void Dragon::Move(const Location player_location)
{


	//プレイヤーとの距離計算
	int range = player_location.x - location.x;
	int range_y = player_location.y - location.y;

	if (range <= ATTACK_SCOPE && range >= -ATTACK_SCOPE &&
		range_y <= ATTACK_SCOPE_Y && range_y >= -ATTACK_SCOPE_Y)
	{
		attack_state = DRAGON_ATTACK::TAIL_ATTACK;
		state = ENEMY_STATE::ATTACK;
	}

	standby_time--;
	if (standby_time < 0)
	{

		switch (attack_method)
		{
		case 0:
			attack_state = DRAGON_ATTACK::DITE;
			state = ENEMY_STATE::ATTACK;
			break;
		case 1:
			attack_state = DRAGON_ATTACK::DREATH;
			state = ENEMY_STATE::ATTACK;
			magic = true;
			break;
		case 2:
			attack_state = DRAGON_ATTACK::ROAR;
			state = ENEMY_STATE::ATTACK;
			magic = true;
			break;
		default:
			break;
		}
	}
}

//-----------------------------------
//攻撃
//-----------------------------------
void Dragon::Attack(const Location player_location)
{


	switch (attack_state)
	{

	case DRAGON_ATTACK::DITE://噛みつき
		DiteMove(player_location);
		break;
	case DRAGON_ATTACK::TAIL_ATTACK: //尻尾攻撃
		attack = false;
		state = ENEMY_STATE::MOVE;
		break;
	case DRAGON_ATTACK::DREATH: //ブレス攻撃
		DreathMove(player_location);
		break;
	case DRAGON_ATTACK::ROAR: //咆哮した後に雷を降らせる
		RoarMove(player_location);
		break;
	case DRAGON_ATTACK::NONE:
	default:
		break;
	}
	

}

//-----------------------------------
//接近攻撃時の噛みつき(這いつくばりながら攻撃）体当たりするイメージ
//-----------------------------------
void Dragon::DiteMove(const Location player_location)
{


	tick++;

	if (tick == 1) 
	{
		frame = 0;
	}
	if (tick % 3 == 0) 
	{
		frame++;
		if (frame > 7) 
		{
			tick = 0;
		}
	}

	speed = ATTACK_SPEED;

	if (left_move == true)
	{
		location.x -= speed;
		speed = SPEED;
	}
	else
	{
		location.x += speed;
		speed = SPEED;
	}

	if (wall_hit == true)
	{
		attack_method = GetRand(2);

		state = ENEMY_STATE::MOVE;
	}

}


//-----------------------------------
//遠距離攻撃（ブレス）この時飛びながらブレスを行う
//-----------------------------------
void Dragon::DreathMove(const Location player_location)
{
	Fly();
	if (++breath_time % BREATH_INTERVAL == 0)
	{
		BulletManager::GetInstance()->CreateEnemyBullet
		(new DragonBullet(location, player_location));
	}
}

//-----------------------------------
//遠距離攻撃（咆哮）
//-----------------------------------
void Dragon::RoarMove(const Location player_location)
{

	for (int i = 0; i < THUNDER; i++)
	{
	  
		switch (i)
		{
		case 0:
			BulletManager::GetInstance()->CreateEnemyBullet
			(new DragonThunder(player_location.x, 430));
			break;
		case 1:
			BulletManager::GetInstance()->CreateEnemyBullet
			(new DragonThunder(player_location.x-200, 430));
			break;
		case 2:
			BulletManager::GetInstance()->CreateEnemyBullet
			(new DragonThunder(player_location.x+200,430));
			break;
		default:
			break;
		}
	}


	

	attack_method = GetRand(2);


	standby_time = ROAR_TIME;

	state = ENEMY_STATE::MOVE;

	standby_time = 200;

}

//-----------------------------------
//攻撃が当たったのか
//-----------------------------------
AttackResource Dragon::Hit()
{
	AttackResource ret = { 0,nullptr,0 }; //戻り値

	if (attack_state == DRAGON_ATTACK::DITE)
	{
		attack = true;
		ENEMY_TYPE attack_type[1] = { ENEMY_TYPE::FIRE };
		ret.damage = ATTACK_DITE;
		ret.type = attack_type;
		ret.type_count = 1;
	}

	if (attack_state == DRAGON_ATTACK::TAIL_ATTACK && (!attack))
	{
		attack = true;
		ENEMY_TYPE attack_type[1] = { ENEMY_TYPE::FIRE };
		ret.damage = ATTACK_TAIL;
		ret.type = attack_type;
		ret.type_count = 2;
	}

	return ret;
}

//-----------------------------------
//落下
//-----------------------------------
void Dragon::Fall()
{
	location.y += fall_speed;

	if (fall_speed < 4)
	{
		fall_speed += ENEMY_FALL_SPEED;
	}

}

//-----------------------------------
//飛ぶ
//-----------------------------------
void Dragon::Fly()
{
	if (++ascent_time < 120)
	{
		location.y -= 4;
	}

	if (++fly_tim % 120 == 0)
	{
		if (++fly_num > 4)
		{
			state = ENEMY_STATE::FALL;
			attack_method = GetRand(2);
			standby_time = 200;
		}
	}
	if (fly_num < 4)
	{
		speed = ATTACK_SPEED;

		if (left_move == true)
		{
			location.x -= speed;
			speed = SPEED;
		}
		else
		{
			location.x += speed;
			speed = SPEED;
		}
	}
}

//-----------------------------------
//死亡（倒された）
//-----------------------------------
void Dragon::Death()
{
	can_delete = true;
}

//-----------------------------------
//プレイヤーの弾丸との当たり判定
//-----------------------------------
void Dragon::HitBullet(const BulletBase* bullet)
{
	PlayHitBulletSound(bullet->GetAttribute());

	int i = 0;
	int damage = 0;
	damage_log[i].log = false;

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
		damage = bullet->GetDamage() * RESISTANCE_DAMAGE; //効きにくい
		damage_log[i].congeniality = CONGENIALITY::RESISTANCE;
		break;
	case ATTRIBUTE::EXPLOSION: //爆発 
		damage = bullet->GetDamage() * 0; //効かない
		damage_log[i].congeniality = CONGENIALITY::INVALID;
		break;
	case ATTRIBUTE::MELT: //溶かす 　通常
		damage = bullet->GetDamage() * WEAKNESS_DAMAGE; //通常ダメージ
		damage_log[i].congeniality = CONGENIALITY::WEAKNESS;
		break;
	case ATTRIBUTE::POISON: //毒
		damage = bullet->GetDamage();
		if (!poison)
		{
			poison = true;
			poison_time = bullet->GetDebuffTime();
			poison_damage = bullet->GetDamageParSecond();
			damage_log[i].congeniality = CONGENIALITY::NORMAL;
		}
		break;
	case ATTRIBUTE::PARALYSIS: //麻痺 弱点
		damage = bullet->GetDamage();
		if (!paralysis)
		{
			paralysis = true;
			paralysis_time = bullet->GetDebuffTime() * WEAKNESS_DEBUFF;  //弱点
			damage_log[i].congeniality = CONGENIALITY::WEAKNESS;
			speed = PARALYSIS_SPEED;
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



void Dragon::DrawHPBar(const int max_hp) const
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
//座標の取得
//-----------------------------------
Location Dragon::GetLocation() const
{
	return Location();
}


#ifdef _DEBUG
//-----------------------------------
// 更新(DotByDot)
//-----------------------------------
void Dragon::Update(const ENEMY_STATE state)
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
void Dragon::DebugDraw()
{

}
#endif //_DEBUG