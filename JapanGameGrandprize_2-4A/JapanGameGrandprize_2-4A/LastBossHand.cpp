#include "LastBossHand.h"
#include "Player.h"
#include "Stage/Stage.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "CameraWork.h"
#include "ItemController.h"
#include "EnemySE.h"

#define HAND_IMAGES

#define HAND_WIDTH 100
#define HAND_HEIGHT 100
//移動速度
#define HAND_MOVE_SPEED 2

//次のパンチまでの時間
#define PUNCH_INTERVAL 60 * 2

//パンチするまでの時間
#define PUNCH_STANDBY_TIME 30

#define PUNCH_END_STANDBY_TIME 30

//パンチのスピード
#define PUNCH_SPEED 15

//パンチのダメージ
#define PUNCH_DAMAGE 15

//パンチのスポーン地点(Y座標)
#define PUNCH_Y 200

//HP
#define HAND_HP 200

//死亡している時間
#define DEATH_TIME 1200

//移動量
#define MOVE_VOLUME 100

//角度の上昇量
#define ADD_ANGLE 0.5f

//半径の上昇量
#define ADD_RADIUS 1

//移動半径
#define MOVE_RADIUS 150

//テレポート時間
#define TELEPORT_TIME 30

//アニメーションの更新時間
#define HAND_ANIMATION 5


//-----------------------------------
//コンストラクタ
//-----------------------------------
LastBossHand::LastBossHand()
{
	spawn_location.x = 0;
	spawn_location.y = 0;

	location = spawn_location;

	punch_start = location;

	area.height = HAND_HEIGHT;
	area.width = HAND_WIDTH;

	teleporting = false;
	teleport = false;
	punch = false;
	attack = false;

	move_volume = 0;
	angle = 0;
	radius = 0;
	teleport_time = 0;
	teleport_standby = 0;
	attack_interval = 0;
	punch_standby_time = 0;
	standby_time = 0;
	animation = 0;
	image_argument = 0;
	size = 1;

	can_delete = false;
	left_hand = false;
	left_move = false;
	poison = false;
	paralysis = false;

	hp = HAND_HP;
	speed = -HAND_MOVE_SPEED;
	death_time = 0;
	poison_time = 0;
	poison_damage = 0;
	paralysis_time = 0;

	kind = ENEMY_KIND::LAST_BOSS;
	state = ENEMY_STATE::MOVE;

	move = HAND_MOVE::UP_DOWN;

	hit_block.chip = nullptr;
	hit_block.hit = false;
	old_stage_hit = hit_block.hit;


	drop_element = new ElementItem * [7];
	drop_type_volume = 7;

	int volume = 0;

	for (int i = 0; i < drop_type_volume; i++)
	{
		volume = 1 + GetRand(2);

		drop_element[i] = new ElementItem(static_cast<ELEMENT_ITEM>(i));

		drop_element[i]->SetVolume(volume);
		drop_volume += volume;
	}
}

//-----------------------------------
//コンストラクタ
//-----------------------------------
LastBossHand::LastBossHand(const Location spawn_location, const bool left_hand)
{
	this->spawn_location = spawn_location;
	location = spawn_location;
	//ステージ開始時は画面外上部にいる
	location.y -= MAP_CHIP_SIZE * 15;

	punch_start = location;

	area.height = HAND_HEIGHT;
	area.width = HAND_WIDTH;

	teleporting = false;
	teleport = false;
	punch = false;
	attack = false;

	move_volume = 0;
	angle = 0;
	radius = 0;
	teleport_time = 0;
	teleport_standby = 0;
	attack_interval = 0;
	punch_standby_time = 0;
	animation = 0;
	image_argument = 0;
	size = 1;
	standby_time = 0;
	can_delete = false;
	this->left_hand = left_hand;
	left_move = false;
	poison = false;
	paralysis = false;

	hp = HAND_HP;
	speed = -HAND_MOVE_SPEED;
	death_time = 0;
	poison_time = 0;
	poison_damage = 0;
	paralysis_time = 0;

	kind = ENEMY_KIND::LAST_BOSS;
	state = ENEMY_STATE::IDOL;

	move = HAND_MOVE::UP_DOWN;

	hit_block.chip = nullptr;
	hit_block.hit = false;

	if (images[11].empty())
	{
		images[11].resize(7);
		LoadDivGraph("Images/Enemy/lastbossleft.png", 6, 6, 1, 150, 150, &images[11][0]);
		images[11][6] = LoadGraph("Images/Enemy/lastbossleftfist.png");
	}

	hit_block.chip = nullptr;
	hit_block.hit = false;
	old_stage_hit = hit_block.hit;


	drop_element = new ElementItem * [7];
	drop_type_volume = 7;

	int volume = 0;
	for (int i = 0; i < drop_type_volume; i++)
	{
		volume = 1 + GetRand(2);

		drop_element[i] = new ElementItem(static_cast<ELEMENT_ITEM>(i));

		drop_element[i]->SetVolume(volume);
		drop_volume += volume;
	}
}

//-----------------------------------
//デストラクタ
//-----------------------------------
LastBossHand::~LastBossHand()
{
	for (int i = 0; i < drop_type_volume; i++)
	{
		delete drop_element[i];
	}

	delete[] drop_element;

}

//-----------------------------------
//更新
//-----------------------------------
void LastBossHand::Update(const Player* player, const Stage* stage)
{
	switch (state)
	{
	case ENEMY_STATE::IDOL:
		//ステージ開始時
		Idol();
		break;
	case ENEMY_STATE::MOVE:
		Move(player->GetLocation());
		Animation();

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
	old_stage_hit = hit_block.hit;

	hit_block = HitStage(stage);

	if (state != ENEMY_STATE::IDOL)
	{
		attack_interval--;
	}

	if (CheckHp() && state != ENEMY_STATE::DEATH)
	{
		state = ENEMY_STATE::DEATH;

		ItemController::GetInstance()->SpawnItem(this);
		location.x = -100;
		location.y = -100;
		death_time = DEATH_TIME;
	}

	UpdateDamageLog();
}

//-----------------------------------
//アイドル状態
//-----------------------------------
void LastBossHand::Idol()
{
	//ステージ開始時、上から降りてくる
	if (location.y < spawn_location.y)
	{
		location.y += 2;
		if (spawn_location.y <= location.y)
		{
			location.y = spawn_location.y;
			state = ENEMY_STATE::MOVE;
		}
	}

	Animation();
}

//-----------------------------------
//移動
//-----------------------------------
void LastBossHand::Move(const Location player_location)
{
	if (!teleporting)
	{
		switch (move)
		{
		case HAND_MOVE::UP_DOWN:
		{
			location.y += speed;
			move_volume += speed;

			if ((move_volume < -MOVE_VOLUME) || ((MOVE_VOLUME / 4) < move_volume))
			{
				speed = -speed;
			}
		}
		break;
		case HAND_MOVE::CIRCULAR_MOTION:
		{
			Location circular_speed; //スピード
			float radian = 0; //角度

			if (left_hand)		//角度の設定
			{
				angle += ADD_ANGLE;
			}
			else
			{
				angle += -ADD_ANGLE;
			}

			if (radius < MOVE_RADIUS) //半径
			{
				radius += ADD_RADIUS;
			}

			//角度の計算
			radian = angle * (M_PI / 180);

			//スピードの計算
			circular_speed.x = cosf(radian) * radius;
			circular_speed.y = sinf(radian) * radius;

			location = spawn_location + circular_speed;
		}
		break;
		case HAND_MOVE::NONE:
		default:
			break;
		}
	}
	else
	{
		Teleport(spawn_location);
	}
}

//-----------------------------------
//テレポート
//-----------------------------------
void LastBossHand::Teleport(const Location teleport_location)
{

	if(teleport)
	{
		teleport_time++;
		size = 1 - (static_cast<float>(teleport_time) / TELEPORT_TIME);

		area.width = HAND_WIDTH * size;
		area.height = HAND_HEIGHT * size;
		angle += 45;
		if (size <= 0)
		{
			location = teleport_location;
			teleport = false;
		}
	}
	else
	{
		teleport_time -= 2;
		size = 1 - (static_cast<float>(teleport_time) / TELEPORT_TIME);

		area.width = HAND_WIDTH * size;
		area.height = HAND_HEIGHT * size;
		angle += 45;

		if (1 <= size)
		{
			teleporting = false;
			angle = 0;
		}
	}
}

//-----------------------------------
//落下
//-----------------------------------
void LastBossHand::Fall()
{

}

//-----------------------------------
//攻撃
//-----------------------------------
void LastBossHand::Attack(const Location player_location)
{
	bool old_punch = punch;
	bool old_teleporting = teleporting;

	if (punch) //パンチしている
	{
		Punch();
	}
	else
	{
		if (attack_interval < 0)
		{

			if (!teleporting)
			{
				teleporting = true;
				teleport = true;
				teleport_time = 0;
				punch_start.x = player_location.x;
				punch_start.y = player_location.y - PUNCH_Y;
				if (punch_start.y < area.height / 2)
				{
					punch_start.y = area.height / 2;
				}
			}

			Teleport(punch_start);

			if (old_teleporting  && !teleporting)
			{
				attack = true;
				punch = true;
				speed = PUNCH_SPEED;
				standby_time = PUNCH_STANDBY_TIME;
			}
		}
	}

	if (old_punch && (old_punch != punch)) //パンチ終了
	{
		attack_interval = PUNCH_INTERVAL;
	}
}


//-----------------------------------
//攻撃開始
//-----------------------------------
void LastBossHand::StartAttack()
{
	state = ENEMY_STATE::ATTACK;
	attack = false;
	punch_standby_time = PUNCH_STANDBY_TIME;
	if (left_hand)
	{
		attack_interval = PUNCH_INTERVAL;
	}
	else
	{
		attack_interval = PUNCH_INTERVAL * 2;
	}
}

//-----------------------------------
//攻撃終了
//-----------------------------------
void LastBossHand::EndAttack()
{
	state = ENEMY_STATE::MOVE;
	punch = false;
	teleporting = true;
	teleport = true;
	teleport_time = 0;
	speed = HAND_MOVE_SPEED;
}

//-----------------------------------
//パンチ
//-----------------------------------
void LastBossHand::Punch()
{
	if (punch_standby_time < 0)
	{
		if (standby_time < 0)
		{
			location.y += speed;
		}
		else
		{
			standby_time--;
		}

		if (!old_stage_hit && hit_block.hit)
		{
			PlaySoundMem(EnemySE::GetLastBossSE().punch, DX_PLAYTYPE_BACK);
			speed = -(PUNCH_SPEED / 3);
			attack = false;
			standby_time = PUNCH_END_STANDBY_TIME;
		}

		if (location.y < punch_start.y)
		{
			punch = false;
		}
		
	}
	else
	{
		punch_standby_time--;
	}
}

//-----------------------------------
//ダウンからの復帰
//-----------------------------------
void LastBossHand::Revival()
{
	hp = HAND_HP;
	location = spawn_location;
	state = ENEMY_STATE::MOVE;
	move = static_cast<HAND_MOVE>(GetRand(2));
	teleporting = false;
	drop_volume = 0;
	int volume;
	for (int i = 0; i < drop_type_volume; i++)
	{
		volume = 1 + GetRand(2);

		drop_element[i]->SetVolume(volume);
		drop_volume += volume;
	}
}

//-----------------------------------
//攻撃が当たっているか
//-----------------------------------
AttackResource LastBossHand::Hit()
{
	AttackResource ret = { 0,nullptr,0 };
	ENEMY_TYPE attack_type[1] = { ENEMY_TYPE::NORMAL };


	ret.damage = PUNCH_DAMAGE;
	ret.type = attack_type;
	ret.type_count = 1;
	attack = false;

	return ret;
}

//-----------------------------------
//死亡
//-----------------------------------
void LastBossHand::Death()
{
	bool old_teleporting = teleporting;

	death_time--;

	if (death_time < 0)
	{
		if (!teleporting)
		{
			teleporting = true;
			teleport = true;
			teleport_time = 0;
		}

		Teleport(spawn_location);

		if (old_teleporting && !teleporting)
		{
			state = ENEMY_STATE::MOVE;
			hp = HAND_HP;

			move = static_cast<HAND_MOVE>(GetRand(2));
		}
	}
}

//-----------------------------------
//プレイヤーの弾との当たり判定
//-----------------------------------
void LastBossHand::HitBullet(const BulletBase* bullet)
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
		damage = bullet->GetDamage();
		damage_log[i].congeniality = CONGENIALITY::NORMAL;
		break;
	case ATTRIBUTE::MELT:
		damage = bullet->GetDamage();
		damage_log[i].congeniality = CONGENIALITY::NORMAL;
		break;
	case ATTRIBUTE::POISON:
	case ATTRIBUTE::PARALYSIS:
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
//移動時のアニメーション
//-----------------------------------
void LastBossHand::Animation()
{
	animation++;

	if (animation % HAND_ANIMATION == 0)
	{
		image_argument++;
	}
}

//-----------------------------------
//描画
//-----------------------------------
void LastBossHand::Draw() const
{
	Location draw_location = location;
	Location camera = CameraWork::GetCamera();
	if (state != ENEMY_STATE::DEATH)
	{
		draw_location = draw_location - camera;

		if (teleporting)
		{
			DrawRotaGraphF(draw_location.x, draw_location.y,
				0.1, M_PI / 180 * (angle / 45), magic_circle_image, TRUE);
		}

		if (state != ENEMY_STATE::IDOL)
		{//画面内に映って初めてHPを描画する
			DrawHPBar(HAND_HP);
		}

		DrawDamageLog();
		DrawWeaknessIcon();

		if (punch)
		{
			DrawRotaGraphF(draw_location.x, draw_location.y,
				size, 0, images[11][6], TRUE, left_hand);
		}
		else
		{
			DrawRotaGraphF(draw_location.x, draw_location.y,
				size,0, images[11][image_argument % 6], TRUE, left_hand);
		}
		

	}

}

//-----------------------------------
//HPバーの描画
//-----------------------------------
void LastBossHand::DrawHPBar(const int)const
{

	Location draw_location = location;
	Location camera = CameraWork::GetCamera();

	int bar_size = 50;

	draw_location = draw_location - camera;

	DrawBox(draw_location.x - bar_size, draw_location.y - 80,
		draw_location.x + bar_size, draw_location.y - 70, 0x000000, TRUE);

	DrawBox(draw_location.x - bar_size, draw_location.y - 80,
		draw_location.x - bar_size + ((bar_size * 2) * (static_cast<float>(hp) / HAND_HP)),
		draw_location.y - 70, 0x07ff00, TRUE);

	DrawBox(draw_location.x - bar_size, draw_location.y - 80,
		draw_location.x + bar_size, draw_location.y - 70, 0x8f917f, FALSE);
}
//-----------------------------------
//座標の取得
//-----------------------------------
Location LastBossHand::GetLocation() const
{
	return location;
}

//-----------------------------------
//攻撃中かどうか
//-----------------------------------
bool LastBossHand::IfAttack() const
{
	bool ret = false; //戻り値

	if (punch && attack)
	{
		ret = true;
	}

	return ret;
}

#ifdef _DEBUG
//-----------------------------------
//更新(DotByDot)
//-----------------------------------
void LastBossHand::Update(const ENEMY_STATE state)
{

}

//-----------------------------------
//描画(DotByDot)
//-----------------------------------
void LastBossHand::DebugDraw()
{

}
#endif //_DEBUG