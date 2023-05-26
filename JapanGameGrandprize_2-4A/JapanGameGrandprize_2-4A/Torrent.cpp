#include "Torrent.h"
#include "DxLib.h"
#include "Player.h"
#include "Stage/Stage.h"
#include "TorrentBullet.h"
#include "TorrentNuts.h"
#include "BulletManager.h"
#include "CameraWork.h"

//画像枚数
#define TORRENT_IMAGES 27

//移動速度
#define TORRENT_SPEED 8

//タックルのダメージ
#define TORRENT_TACKLE_DAMAGE 20

//体力
#define TORRENT_HP 1000

//タックル準備時間
#define TORRENT_TACKLE_PREPARATION 60

//発射速度
#define TORRENT_SHOT_RATE 15

//ドロップ数
#define TORRENT_MIN_DROP 20
#define TORRENT_DROP 20

//葉っぱを飛ばしている時間
#define LEAF_CUTTER_TIME 420

//次の葉っぱを飛ばす攻撃に移る時間
#define LEAF_CUTTER_INTERVAL 300

//木の実を落としている時間
#define DROP_NUTS_TIME 600

//次の木の実を落とす攻撃に移る時間
#define DROP_NUTS_INTERVAL 300

//木の実の生成する時間
#define SPAWN_NUTS_INTERVAL 30

//木の実の生成地点数
#define SPAWN_NUTS_POINT 15

//木の実が一回でスポーンできる最大数
#define SPAWN_NUTS_MAX 6

//木の実のスポーン地点
#define SPAWN_NUTS_Y 100

//スポーン地点の間隔
#define NUTS_SPAWN_SPACE 80

//アニメーション
#define TORRENT_ANIMATION 5

//-----------------------------------
//コンストラクタ
//-----------------------------------
Torrent::Torrent(Location spawn_location)
{
	//仮hp
	hp = TORRENT_HP;
	left_move = true;
	attack = false;
	tackle_end = false;
	speed = -TORRENT_SPEED;
	tackle_end_point = 0;
	shot_rate = 0;
	leaf_cutter_interval = 0;
	drop_nuts_interval = 0;
	spawn_interval = 0;
	animation = 0;
	image_argument = 0;
	attack_time = 20;
	state = ENEMY_STATE::MOVE;
	attack_state = TORRENT_ATTACK::NONE;

	/*当たり判定の設定*/
	area.width = 180;
	area.height = 300;
	location = spawn_location;

	location.x += MAP_CHIP_SIZE * 10;
	location.y += MAP_CHIP_SIZE / 2;

	kind = ENEMY_KIND::TORRENT;

	//ドロップアイテムの設定
	drop_element = new ElementItem * [SOIL_DROP];
	drop_type_volume = SOIL_DROP;

	int volume = 0;

	for (int i = 0; i < SOIL_DROP; i++)
	{
		volume = TORRENT_MIN_DROP + GetRand(TORRENT_DROP);
		drop_element[i] = new ElementItem(static_cast<ELEMENT_ITEM>(2 + i));
		drop_element[i]->SetVolume(volume);
		drop_volume += volume;
	}

	int num = static_cast<int>(kind) - static_cast<int>(ENEMY_KIND::SLIME);

	if (images[num].empty())
	{
		images[num].resize(TORRENT_IMAGES);

		LoadDivGraph("Images/Enemy/torrent_tackle.png", 8, 8, 1, 500, 500, &images[num][0]);
		LoadDivGraph("Images/Enemy/torrent_nut.png", 9, 9, 1, 500, 500, &images[num][8]);
		LoadDivGraph("Images/Enemy/torrent_leaf.png", 10, 10, 1, 500, 500, &images[num][17]);
	}
}

//-----------------------------------
//デストラクタ
//-----------------------------------
Torrent::~Torrent()
{
	DeleteGraph(magic_circle_image);

	for (int i = 0; i < WIND_DROP; i++)
	{
		delete drop_element[i];
	}
	delete[] drop_element;
}

//-----------------------------------
//更新
//-----------------------------------
void Torrent::Update(const Player* player, const Stage* stage)
{
	animation++;
	Location old_location = location;	//前の座標


	Location scroll; //画面スクロールを考慮した座標
	Location camera = CameraWork::GetCamera(); //カメラ
	scroll = location - camera;

	if (SCREEN_WIDTH < scroll.x + area.width / 2 + speed)
	{
		state = ENEMY_STATE::MOVE;
	}
	else if (state != ENEMY_STATE::DEATH)
	{
		state = ENEMY_STATE::ATTACK;
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

	Animation();
	UpdateDamageLog();
	Poison();
	if (CheckHp() && state != ENEMY_STATE::DEATH)
	{
		state = ENEMY_STATE::DEATH;
	}
}

//-----------------------------------
//アイドル状態
//-----------------------------------
void Torrent::Idol()
{

}

//-----------------------------------
//移動
//-----------------------------------
void Torrent::Move(const Location player_location)
{
	location.x += static_cast<float>(speed);
}

//-----------------------------------
//落下
//-----------------------------------
void Torrent::Fall()
{

}

//-----------------------------------
//攻撃
//-----------------------------------
void  Torrent::Attack(Location player_location)
{

	leaf_cutter_interval--;
	drop_nuts_interval--;


	switch (attack_state)
	{
	case TORRENT_ATTACK::TACKLE:
		Tackle();
		break;
	case TORRENT_ATTACK::LEAF_CUTTER:
		LeafCutter(player_location);
		break;
	case TORRENT_ATTACK::DROP_NUTS:
		DropNuts();
		break;
	case TORRENT_ATTACK::NONE:
		AttackNone();
		break;
	default:
		break;
	}
}

//-----------------------------------
//タックル攻撃
//-----------------------------------
void Torrent::Tackle()
{
	if (attack_time < 0)
	{
		if(CheckSoundMem(EnemySE::GetTorrentSE().tackle_se)==0)
		{
			PlaySoundMem(EnemySE::GetTorrentSE().tackle_se, DX_PLAYTYPE_LOOP);
		}
		location.x += speed;

		if (left_move)
		{
			if (location.x <= tackle_end_point)
			{
				tackle_end = true;
				left_move = !left_move;
			}
		}
		else
		{
			if (tackle_end_point <= location.x)
			{
				tackle_end = true;
				left_move = !left_move;
			}
		}

		if (tackle_end) //タックル終了
		{
			StopSoundMem(EnemySE::GetTorrentSE().tackle_se);
			attack = false;
			int next_attack;	//次の攻撃
			next_attack = GetRand(10) + 1;  //次の攻撃の設定

			if (7 < next_attack)
			{
				if (drop_nuts_interval < leaf_cutter_interval)
				{
					attack_state = TORRENT_ATTACK::DROP_NUTS;
					attack_time = DROP_NUTS_TIME;
				}
				else
				{
					attack_state = TORRENT_ATTACK::LEAF_CUTTER;
					attack_time = LEAF_CUTTER_TIME;
				}
			}
			else
			{
				if (drop_nuts_interval < leaf_cutter_interval)
				{
					attack_state = TORRENT_ATTACK::LEAF_CUTTER;
					attack_time = LEAF_CUTTER_TIME;
				}
				else
				{
					attack_state = TORRENT_ATTACK::DROP_NUTS;
					attack_time = DROP_NUTS_TIME;				
				}
			}
			
			
			image_argument = 0;

		}
	}
	else
	{
		attack_time--;
	}
}

//-----------------------------------
//葉っぱを飛ばす攻撃
//-----------------------------------
void Torrent::LeafCutter(const Location player_location)
{
	if (CheckSoundMem(EnemySE::GetTorrentSE().leaves_cutter) == 0)
	{
		PlaySoundMem(EnemySE::GetTorrentSE().leaves_cutter, DX_PLAYTYPE_BACK);
	}
	attack_time--;
	CreateLeaf(player_location);
	if (attack_time < 0) //攻撃の終了
	{
		leaf_cutter_interval = LEAF_CUTTER_INTERVAL; //次の攻撃までの時間の設定
		if (drop_nuts_interval < 0) //木の実を落とす攻撃が可能かどうか
		{
			TORRENT_ATTACK next_attack; //次の攻撃
			next_attack = static_cast <TORRENT_ATTACK>(GetRand(1) * 2); //次の攻撃の設定

			switch (next_attack) //次の攻撃に移行
			{
			case TORRENT_ATTACK::TACKLE:
				attack_state = TORRENT_ATTACK::TACKLE;
				attack_time = TORRENT_TACKLE_PREPARATION;
				tackle_end = false;
				tackle_end_point = CameraWork::GetCamera().x;
				if (left_move) //左に向いている
				{
					speed = -TORRENT_SPEED;
					tackle_end_point += static_cast<int>(area.width / 2) + MAP_CHIP_SIZE;
				}
				else
				{
					speed = TORRENT_SPEED;
					tackle_end_point += static_cast<int>(SCREEN_WIDTH - area.width / 2) - MAP_CHIP_SIZE;
				}
				break;
			case TORRENT_ATTACK::DROP_NUTS:
				attack_state = TORRENT_ATTACK::DROP_NUTS;
				attack_time = DROP_NUTS_TIME;
				break;
			case TORRENT_ATTACK::LEAF_CUTTER:
			case TORRENT_ATTACK::NONE:
			default:
				break;
			}
			image_argument = 0;
		}
		else
		{
			attack_state = TORRENT_ATTACK::TACKLE; //タックル攻撃に移行
			attack_time = TORRENT_TACKLE_PREPARATION;
			tackle_end = false;
			tackle_end_point = CameraWork::GetCamera().x;

			if (left_move) //左に向いている
			{
				speed = -TORRENT_SPEED;
				tackle_end_point += static_cast<int>(area.width / 2) + MAP_CHIP_SIZE;
			}
			else
			{
				speed = TORRENT_SPEED;
				tackle_end_point += static_cast<int>(SCREEN_WIDTH - area.width / 2) - MAP_CHIP_SIZE;
			}
			image_argument = 0;
		}
	}
}

//-----------------------------------
//葉っぱの生成
//-----------------------------------
void Torrent::CreateLeaf(Location player_location)
{

	shot_rate++;

	if (shot_rate % TORRENT_SHOT_RATE == 0) //葉っぱの生成
	{
		Location spawn_location = location; //生成座標
		spawn_location.y = location.y - area.height / 2;
		BulletManager::GetInstance()->
			CreateEnemyBullet(new TorrentBullet(spawn_location, player_location));
	}
}

//-----------------------------------
//木の実を落とす攻撃
//-----------------------------------
void Torrent::DropNuts()
{

	attack_time--;
	CreateNuts();
	if (attack_time < 0)
	{
		drop_nuts_interval = DROP_NUTS_INTERVAL;
		spawn_interval = 0;
		if (leaf_cutter_interval < 0) //木の実を落とす攻撃が可能かどうか
		{
			TORRENT_ATTACK next_attack; //次の攻撃
			next_attack = static_cast <TORRENT_ATTACK>(GetRand(1) * 2); //次の攻撃の設定

			switch (next_attack) //次の攻撃に移行
			{
			case TORRENT_ATTACK::TACKLE:
				attack_state = TORRENT_ATTACK::TACKLE;
				attack_time = TORRENT_TACKLE_PREPARATION;
				tackle_end = false;
				tackle_end_point = CameraWork::GetCamera().x;

				if (left_move) //左に向いている
				{
					speed = -TORRENT_SPEED;
					tackle_end_point += static_cast<int>(area.width / 2) + MAP_CHIP_SIZE;
				}
				else
				{
					speed = TORRENT_SPEED;
					tackle_end_point += static_cast<int>(SCREEN_WIDTH - area.width / 2) - MAP_CHIP_SIZE;
				}
				break;
			case TORRENT_ATTACK::LEAF_CUTTER:
				attack_state = TORRENT_ATTACK::LEAF_CUTTER;
				attack_time = LEAF_CUTTER_INTERVAL;
				break;
			case TORRENT_ATTACK::DROP_NUTS:
			case TORRENT_ATTACK::NONE:
			default:
				break;
			}
		}
		else
		{
			attack_state = TORRENT_ATTACK::TACKLE; //タックル攻撃に移行
			attack_time = TORRENT_TACKLE_PREPARATION;
			tackle_end = false;
			tackle_end_point = CameraWork::GetCamera().x;

			if (left_move) //左に向いている
			{
				speed = -TORRENT_SPEED;
				tackle_end_point += static_cast<int>(area.width / 2) + MAP_CHIP_SIZE;
			}
			else
			{
				speed = TORRENT_SPEED;
				tackle_end_point += static_cast<int>(SCREEN_WIDTH - area.width / 2) - MAP_CHIP_SIZE;
			}
		}
		image_argument = 0;
	}
}

//-----------------------------------
//木の実の生成
//-----------------------------------
void Torrent::CreateNuts()
{

	bool spawn_point[SPAWN_NUTS_POINT]; //スポーン地点
	int spawn_volume = 0; //スポーン数
	bool spawn = false; //スポーンした

	spawn_interval++;

	if (spawn_interval % SPAWN_NUTS_INTERVAL == 0)
	{
		for (int i = 0; i < SPAWN_NUTS_POINT; i++)
		{
			spawn_point[i] = false;
		}

		spawn_volume = GetRand(SPAWN_NUTS_MAX - 1) + 1; //スポーン数の設定

		for (int i = 0; i < spawn_volume; i++) //木の実の生成
		{
			spawn = false;

			while (!spawn)
			{
				int spawn_point_rand; //スポーン地点
				spawn_point_rand = GetRand(SPAWN_NUTS_POINT - 1); //スポーン地点の設定
				if (!spawn_point[spawn_point_rand])
				{
					Location spawn_location = CameraWork::GetCamera(); //スポーン地点
					spawn_location.x += (spawn_point_rand + 1) * NUTS_SPAWN_SPACE;
					
					spawn_location.y = SPAWN_NUTS_Y;
					spawn_point[spawn_point_rand] = true;
					spawn = true;

           			BulletManager::GetInstance()->
						CreateEnemyNuts(new TorrentNuts(ENEMY_TYPE::WATER, spawn_location));
				}

			}
		}
	}
}

//-----------------------------------
// 攻撃していない
//-----------------------------------
void Torrent::AttackNone()
{

	attack_time--;
	if (attack_time < 0)
	{
		TORRENT_ATTACK next_attack;	//次の攻撃
		next_attack = static_cast<TORRENT_ATTACK>(GetRand(1) + 1);  //次の攻撃の設定

		switch (next_attack)
		{
		case TORRENT_ATTACK::LEAF_CUTTER:
			attack_state = TORRENT_ATTACK::LEAF_CUTTER;
			attack_time = LEAF_CUTTER_TIME;
			break;
		case TORRENT_ATTACK::DROP_NUTS:
			attack_state = TORRENT_ATTACK::DROP_NUTS;
			attack_time = DROP_NUTS_TIME;
			break;
		case TORRENT_ATTACK::TACKLE:
		case TORRENT_ATTACK::NONE:
		default:
			break;
		}
		image_argument = 0;
	}
}

//-----------------------------------
//攻撃が当たっているか
//-----------------------------------
AttackResource Torrent::Hit()
{

	AttackResource ret = { 0,nullptr,0 }; //戻り値

	if (attack_state == TORRENT_ATTACK::TACKLE)
	{
		if (!attack)
		{
			attack = true;
			ENEMY_TYPE attack_type[1] = { ENEMY_TYPE::SOIL };
			ret.damage = TORRENT_TACKLE_DAMAGE;
			ret.type = attack_type;
			ret.type_count = 1;
		}
	}
	return ret;
}

//-----------------------------------
//死亡
//-----------------------------------
void Torrent::Death()
{
	StopSoundMem(EnemySE::GetTorrentSE().break_nut_se);
	StopSoundMem(EnemySE::GetTorrentSE().falling_nut_se);
	StopSoundMem(EnemySE::GetTorrentSE().leaves_cutter);
	StopSoundMem(EnemySE::GetTorrentSE().tackle_se);
	can_delete = true;
}

//-----------------------------------
//プレイヤーの弾との当たり判定
//-----------------------------------
void Torrent::HitBullet(const BulletBase* bullet)
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
		damage_log[i].congeniality = CONGENIALITY::NOMAL;

		break;
	case ATTRIBUTE::EXPLOSION:
		damage = bullet->GetDamage() * 0;
		damage_log[i].congeniality = CONGENIALITY::INVALID;
		break;
	case ATTRIBUTE::MELT:
		damage = bullet->GetDamage() * WEAKNESS_DAMAGE;
		damage_log[i].congeniality = CONGENIALITY::WEAKNESS;
		break;
	case ATTRIBUTE::POISON:
		damage = bullet->GetDamage();
		damage_log[i].congeniality = CONGENIALITY::NOMAL;
		if (!poison)
		{
			poison_damage = bullet->GetDamageParSecond();
			poison_time = bullet->GetDebuffTime() * RESISTANCE_DEBUFF;
		}
		break;
	case ATTRIBUTE::PARALYSIS:
		damage = bullet->GetDamage();
	case ATTRIBUTE::HEAL:
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
//タックルのアニメーション
//-----------------------------------
void Torrent::Animation()
{
	if (animation % TORRENT_ANIMATION == 0)
	{
		image_argument++;
	}
}

//-----------------------------------
//描画
//-----------------------------------
void Torrent::Draw() const
{
	Location draw_location = location; //描画座標
	Location camera = CameraWork::GetCamera();
	draw_location = draw_location - camera;

	int num = static_cast<int>(kind) - static_cast<int>(ENEMY_KIND::SLIME);

	if (state == ENEMY_STATE::MOVE)
	{
		DrawRotaGraphF(draw_location.x, draw_location.y, 0.7, 0, images[num][image_argument % 8], TRUE, !left_move);
	}
	else
	{
		switch (attack_state)
		{
		case TORRENT_ATTACK::TACKLE:
			DrawRotaGraphF(draw_location.x, draw_location.y, 0.7, 0, images[num][image_argument % 8], TRUE, !left_move);
			break;
		case TORRENT_ATTACK::LEAF_CUTTER:
			DrawRotaGraphF(draw_location.x, draw_location.y, 0.7, 0, images[num][image_argument % 10 + 17], TRUE, !left_move);
			break;
		case TORRENT_ATTACK::DROP_NUTS:
			DrawRotaGraph3(SCREEN_WIDTH / 2, 50, 640, 640, 1, 0.05, 0, magic_circle_image, TRUE);
			DrawRotaGraphF(draw_location.x, draw_location.y, 0.7, 0, images[num][image_argument % 9 + 8], TRUE, !left_move);
			break;
		case TORRENT_ATTACK::NONE:
			DrawRotaGraphF(draw_location.x, draw_location.y, 0.7, 0, images[num][0], TRUE, !left_move);
			break;
		default:
			break;
		}
	}

	//画面内に映ってからHPを描画する
	if (state != ENEMY_STATE::DEATH && draw_location.x + area.width / 2 < SCREEN_WIDTH + MAP_CHIP_SIZE)
	{
		DrawHPBar(TORRENT_HP);
		DrawDamageLog();
		DrawWeaknessIcon();
	}
	
}

//-----------------------------------
//HPバーの描画
//-----------------------------------
void Torrent::DrawHPBar(const int max_hp) const
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
Location Torrent::GetLocation() const
{

	return location;
}

#ifdef _DEBUG
//-----------------------------------
// 更新(DotByDot)
//-----------------------------------
void Torrent::Update(const ENEMY_STATE state)
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
void Torrent::DebugDraw()
{
	DrawBox(location.x - area.width / 2, location.y - area.height / 2,
		location.x + area.width / 2, location.y + area.height / 2,
		0xff0000, FALSE);
}
#endif //_DEBUG