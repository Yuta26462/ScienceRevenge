#include "LastBoss.h"
#include "LastBossHand.h"
#include "DxLib.h"
#include "CameraWork.h"
#include "LastBossMagic.h"

//体力
#define LAST_BOSS_HP 2500

//手の数
#define HAND_NUM 2

//魔法の発動数
#define MAGIC_NUM1 3
#define MAGIC_NUM2 4
#define MAGIC_NUM3 5


//ダウン時の落ちる速度
#define DOWN_MOVE_SPEED 10

//ダウンしている時間
#define DOWN_TIME 60 * 10

//魔法攻撃
#define MAGIC_TIME 60 * 30

//魔法の発動レート
#define MAGIC_RATE 60

//パンチ攻撃している時間
#define PUNCH_TIME 60 * 20

//攻撃間隔
#define ATTACK_INTERVAL 60 * 1

//次の魔法攻撃までの時間
#define MAGIC_INTERVAL 60 * 10

//次のパンチまでの時間
#define PUNCH_INTERVAL 60 * 10

//次の剣攻撃までの時間
#define SWORD_INTERVAL 60 * 10

//必殺技の時間
#define SPECIAL_MOVES_TIME 60 * 10

//アニメーション
#define LAST_BOSS_ANIMATION 5

//-----------------------------------
//コンストラクタ
//-----------------------------------
LastBoss::LastBoss(Location spawn_location)
{
	Location spawn_hand;
	this->spawn_location = spawn_location;
	location = spawn_location;

	spawn_hand = location;
	spawn_hand.x -= 200;

	spawn_hand.y += 100;

	//ステージ開始時は画面外上部にいる
	location.y -= MAP_CHIP_SIZE * 15;

	hand = new EnemyBase * [HAND_NUM];

	area.height = 200;
	area.width = 200;

	for (int i = 0; i < HAND_NUM; i++)
	{
		spawn_hand.x += (400 * i);
		hand[i] = new LastBossHand(spawn_hand, static_cast<bool>(i));
	}

	magic = nullptr;

	kind = ENEMY_KIND::LAST_BOSS;

	can_special_moves = true;
	down = false;
	attack = false;
	attack_time = 0;
	down_time = 0;
	attack_interval = ATTACK_INTERVAL;
	magic_interval = 0;
	punch_interval = 0;
	sword_interval = 0;
	magic_rate = 0;
	magic_volume = 0;
	special_moves_time = 0;
	hp = LAST_BOSS_HP;
	animation = 0;
	image_argument = 0;

	state = ENEMY_STATE::IDOL;
	attack_state = LAST_BOSS_ATTACK::NONE;
	hit_stage = { false,nullptr };

	Location tmp = spawn_location;
	tmp.y = 0;

	special_moves = nullptr;
	barrier = nullptr;

	int num = static_cast<int>(kind) - static_cast<int>(ENEMY_KIND::SLIME);

	if (images[num].empty())
	{
		images[num].resize(4);
		LoadDivGraph("Images/Enemy/lastbossdown1.png", 4, 4, 1, 500, 500, &images[num][0]);
	}
}

//-----------------------------------
//デストラクタ
//-----------------------------------
LastBoss::~LastBoss()
{
	for (int i = 0; i < HAND_NUM; i++)
	{
		delete hand[i];
	}
	delete[] hand;

	for (int i = 0; i < magic_volume; i++)
	{
		delete magic;
	}
	delete[] magic;

	delete special_moves;
	delete barrier;

}

//-----------------------------------
//更新
//-----------------------------------
void LastBoss::Update(const class Player* player, const class Stage* stage)
{
	LastBossHand* me_hand;

	if (down)
	{
		down_time--;
		if (down_time < 0)
		{
			Animation();
			if (Revival())
			{
				down = false;
				if (barrier == nullptr)
				{
					for (int i = 0; i < HAND_NUM; i++)
					{
						me_hand = dynamic_cast<LastBossHand*>(hand[i]);

						me_hand->Revival();
					}
				}
			}
		}
		else
		{
			if (!hit_stage.hit)
			{
				location.y += DOWN_MOVE_SPEED;
			}
		}

		if (state == ENEMY_STATE::DEATH)
		{
			can_delete = true;
		}
	}
	else
	{
		switch (state)
		{//ステージ開始時
		case ENEMY_STATE::IDOL:
			Idol();

			for (int i = 0; i < HAND_NUM; i++)
			{
				if (hand[i] != nullptr)
				{
					hand[i]->Update(player, stage);
				}
			}
			break;
		case ENEMY_STATE::MOVE:
			break;
		case ENEMY_STATE::FALL:
			break;
		case ENEMY_STATE::ATTACK:
			Attack(player->GetLocation());
			break;
		case ENEMY_STATE::DEATH:
			can_delete = true;
			break;
		default:
			break;
		}

		if (barrier == nullptr && state != ENEMY_STATE::IDOL)
		{
			for (int i = 0; i < HAND_NUM; i++)
			{
				hand[i]->Update(player, stage);
			}

			if ((hand[0]->GetState() == ENEMY_STATE::DEATH) &&
				(hand[1]->GetState() == ENEMY_STATE::DEATH))
			{
				down = true;
				down_time = DOWN_TIME;

				if (magic != nullptr)
				{
					for (int i = 0; i < magic_volume; i++)
					{
						delete magic[i];
					}
					delete[] magic;
				}

				magic = nullptr;

				attack_state = LAST_BOSS_ATTACK::NONE;
				magic_interval = MAGIC_INTERVAL;
			}
		}

		if (state != ENEMY_STATE::IDOL)
		{
			punch_interval--;
			magic_interval--;
			sword_interval--;
		}
	}
	hit_stage = HitStage(stage);

	UpdateDamageLog();

	if (CheckHp())
	{
		state = ENEMY_STATE::DEATH;
	}
}

//-----------------------------------
//ダウンからの復帰
//-----------------------------------
bool LastBoss::Revival()
{
	bool ret = false; //戻り値

	location.y -= DOWN_MOVE_SPEED / 2;

	if (location.y <= spawn_location.y)
	{
		ret = true;
	}

	return ret;
}

//-----------------------------------
//アイドル状態
//-----------------------------------
void LastBoss::Idol()
{
	if (location.y < spawn_location.y)
	{
		location.y += 2;

		if (spawn_location.y <= location.y)
		{
			location.y = spawn_location.y;
			state = ENEMY_STATE::ATTACK;
		}
	}
}

//-----------------------------------
//移動
//-----------------------------------
void LastBoss::Move(const Location player_location)
{

}

//-----------------------------------
//移動時のアニメーション
//-----------------------------------
void LastBoss::MoveAnimation()
{

}

//-----------------------------------
//落下
//-----------------------------------
void LastBoss::Fall()
{

}

//-----------------------------------
//攻撃
//-----------------------------------
void  LastBoss::Attack(const Location player_location)
{
	attack_time--;

	switch (attack_state)
	{
	case LAST_BOSS_ATTACK::MAGIC:
		Magic(player_location);
		break;
	case LAST_BOSS_ATTACK::PUNCH:
		Punch();
		break;
	case LAST_BOSS_ATTACK::SWORD:
		Sword();
		break;
	case LAST_BOSS_ATTACK::SPECIAL_MOVES:
		if (barrier != nullptr)
		{
			barrier->Update();
			if (barrier->Break())
			{
				down = true;
				down_time = DOWN_TIME;
				delete barrier;
				barrier = nullptr;
				delete special_moves;
				special_moves = nullptr;
			}
		}
		if (special_moves != nullptr)
		{
			special_moves->Update();
		}
		break;
	case LAST_BOSS_ATTACK::NONE:
		AttackNone();
		break;
	default:
		break;
	}
}

//-----------------------------------
//魔法攻撃の初期化
//-----------------------------------
void LastBoss::InitMagic()
{
	attack_time = MAGIC_TIME;
	if(hp < (LAST_BOSS_HP / 3)) //魔法の発動数の設定
	{
		magic_volume = MAGIC_NUM3;
		magic_rate = MAGIC_RATE / 3;
	}
	else if (hp < ((LAST_BOSS_HP / 3) * 2))
	{
		magic_volume = MAGIC_NUM2;
		magic_rate = MAGIC_RATE / 2;

	}
	else
	{
		magic_volume = MAGIC_NUM1;
		magic_rate = MAGIC_RATE;

	}
	magic = new EnemyBulletBase * [magic_volume];

	for (int i = 0; i < magic_volume; i++)
	{
		magic[i] = nullptr;
	}
	attack_state = LAST_BOSS_ATTACK::MAGIC;
}

//-----------------------------------
//魔法攻撃
//-----------------------------------
void LastBoss::Magic(const Location player_location)
{
	bool player_spawn;
	bool end = true;
	attack_time--;
	if (0 < attack_time)
	{
		//魔法の生成
		if (attack_time % magic_rate == 0)
		{
			for (int i = 0; i < magic_volume; i++)
			{
				if (magic[i] == nullptr)
				{
					player_spawn = static_cast<bool>(GetRand(1));
					if (player_spawn)
					{
						magic[i] = new LastBossMagic(player_location);
					}
					else
					{
						magic[i] = new LastBossMagic();
					}
					break;
				}
			}
		}
	}
	else
	{
		//すべての魔法が発動終了しているかどうか
		for (int i = 0; i < magic_volume; i++)
		{
			if (magic[i] != nullptr)
			{
				end = false;
				break;
			}
		}

		if (end) //終了
		{
			for (int i = 0; i < magic_volume; i++)
			{
				delete magic[i];
			}
			delete[] magic;

			magic = nullptr;

			attack_state = LAST_BOSS_ATTACK::NONE;
			magic_interval = MAGIC_INTERVAL;
		}
	}

	//魔法の発動
	if (magic != nullptr)
	{
		for (int i = 0; i < magic_volume; i++)
		{
			if (magic[i] != nullptr)
			{
				LastBossMagic* my_magic = static_cast<LastBossMagic*>(magic[i]);

				my_magic->Update();

				if (my_magic->GetCanDelete())
				{
					delete magic[i];
					magic[i] = nullptr;
				}
			}
		}
	}
}

//-----------------------------------
//パンチ攻撃の初期化
//-----------------------------------
void LastBoss::InitPunch()
{
	LastBossHand* me_hand;

	attack_state = LAST_BOSS_ATTACK::PUNCH;

	for (int i = 0; i < HAND_NUM; i++)
	{
		me_hand = dynamic_cast<LastBossHand*>(hand[i]);

		me_hand->StartAttack();
	}

	attack_time = PUNCH_TIME;
}

//-----------------------------------
//パンチ攻撃
//-----------------------------------
void LastBoss::Punch()
{
	LastBossHand* me_hand;

	if (attack_time < 0)
	{
		for (int i = 0; i < HAND_NUM; i++)
		{
			me_hand = dynamic_cast<LastBossHand*>(hand[i]);

			me_hand->EndAttack();
		}
		attack_state = LAST_BOSS_ATTACK::NONE;
		punch_interval = PUNCH_INTERVAL;
	}
}

//-----------------------------------
//剣攻撃の初期化
//-----------------------------------
void LastBoss::InitSword()
{

}

//-----------------------------------
//剣攻撃
//-----------------------------------
void LastBoss::Sword()
{


}

//-----------------------------------
//攻撃しない
//-----------------------------------
void LastBoss::AttackNone()
{
	LAST_BOSS_ATTACK next_attack = LAST_BOSS_ATTACK::NONE; //次の攻撃
	int attack_num = 0; //攻撃ができる種類の数
	attack_interval--;
	if (attack_interval < 0) //攻撃可能
	{
		if (magic_interval < 0) //魔法攻撃可能
		{
			attack_num++;
		}

		if (punch_interval < 0) //パンチ攻撃可能
		{
			attack_num++;
		}

		if (sword_interval < 0) //剣攻撃可能
		{
			attack_num++;
		}

		if ((attack_num == 3) || (attack_num == 0)) //攻撃ができる種類の数が3個か0個
		{
			next_attack = static_cast<LAST_BOSS_ATTACK>(GetRand(2));
		}
		else if (attack_num == 2) //攻撃ができる種類の数が2個
		{
			int rand = GetRand(1);

			if ((magic_interval < 0) && (punch_interval < 0)) //魔法攻撃とパンチ攻撃が可能
			{
				next_attack = static_cast<LAST_BOSS_ATTACK>(rand);
			}
			else if((magic_interval < 0) && (sword_interval < 0)) //魔法攻撃と剣攻撃が可能
			{
				next_attack = static_cast<LAST_BOSS_ATTACK>(2 * rand);
			}
			else if ((punch_interval < 0) && (sword_interval < 0)) //パンチ攻撃と剣攻撃が可能
			{
				next_attack = static_cast<LAST_BOSS_ATTACK>(rand + 1);
			}
			else{}
		}
		else if (attack_num == 1) //攻撃ができる種類の数が1個
		{
			if (magic_interval < 0) //魔法攻撃可能
			{
				next_attack = LAST_BOSS_ATTACK::MAGIC;
			}
			else if (punch_interval < 0) //パンチ攻撃可能
			{
				next_attack = LAST_BOSS_ATTACK::PUNCH;
			}
			else if (sword_interval < 0) //剣攻撃可能
			{
				next_attack = LAST_BOSS_ATTACK::SWORD;
			}
			else{}
		}
		else{}

		switch (next_attack) //次の攻撃
		{
		case LAST_BOSS_ATTACK::MAGIC:
			InitMagic();
			break;
		case LAST_BOSS_ATTACK::PUNCH:
			InitPunch();
			break;
		case LAST_BOSS_ATTACK::SWORD:
			InitSword();
			break;
		case LAST_BOSS_ATTACK::SPECIAL_MOVES:
		case LAST_BOSS_ATTACK::NONE:
		default:
			break;
		}
	}

		
}

//-----------------------------------
//攻撃が当たっているか
//-----------------------------------
AttackResource LastBoss::Hit()
{
	AttackResource ret = { 0,nullptr,0 };

	return ret;
}

//-----------------------------------
//攻撃が当たっているか
//-----------------------------------
AttackResource LastBoss::Hit(const BoxCollider* player)
{
	AttackResource ret = { 0,nullptr,0 };

	switch (attack_state)
	{
	case LAST_BOSS_ATTACK::MAGIC:
		for (int i = 0; i < magic_volume; i++)
		{
			if (magic[i] != nullptr)
			{
				LastBossMagic* my_magic = static_cast<LastBossMagic*>(magic[i]);

				if (!my_magic->GetDoStandby())
				{
					if (my_magic->HitBox(player))
					{
						ret.type_count = 1;
						ENEMY_TYPE type[1] = { magic[i]->GetType() };
						ret.type = type;
						ret.damage = magic[i]->GetDamage();
					}
				}
			}
		}
		break;
	case LAST_BOSS_ATTACK::PUNCH:
		ret = PunchAttack(player);
		break;
	case LAST_BOSS_ATTACK::SWORD:
		break;
	case LAST_BOSS_ATTACK::SPECIAL_MOVES:
		if (special_moves != nullptr)
		{
			if (special_moves->GetEnd())
			{
				ret.type_count = 1;
				ENEMY_TYPE type[1] = { ENEMY_TYPE::NORMAL };
				ret.type = type;
				ret.damage = special_moves->GetDamage();
			}
		}
		break;
	case LAST_BOSS_ATTACK::NONE:
		break;
	default:
		break;
	}

	return ret;
}
//-----------------------------------
//プレイヤーとパンチとの当たり判定
//-----------------------------------
AttackResource LastBoss::PunchAttack(const BoxCollider* player)
{
	AttackResource ret = { 0,nullptr,0 };

	LastBossHand* my_hand;
	for (int i = 0; i < HAND_NUM; i++)
	{
		my_hand = dynamic_cast<LastBossHand*>(hand[i]);

		if (my_hand->IfAttack())
		{
			if (my_hand->HitBox(player))
			{
				ret = my_hand->Hit();
			}
		}
	}

	return ret;
}

//-----------------------------------
//死亡
//-----------------------------------
void LastBoss::Death()
{

}

//-----------------------------------
//プレイヤーの弾との当たり判定
//-----------------------------------
void LastBoss::HitBullet(const BulletBase* bullet)
{
	PlayHitBulletSound(bullet->GetAttribute());

	int i;
	int damage = 0;

	if (barrier == nullptr)
	{
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
			damage = bullet->GetDamage() * RESISTANCE_DAMAGE;
			damage_log[i].congeniality = CONGENIALITY::RESISTANCE;
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
	}

	if (hp < 0)
	{
		hp = 0;
	}

	if ((hp <= 1) && (can_special_moves))
	{
		can_special_moves = false;
		hp = 1;
		barrier = new LastBossBarrier(spawn_location);
		special_moves = new LastBossSpecialMoves(spawn_location);
		down = false;
		location = spawn_location;
		attack_state = LAST_BOSS_ATTACK::SPECIAL_MOVES;
	}
}

//-----------------------------------
//プレイヤーの弾との当たり判定
//-----------------------------------
bool LastBoss::CheckHitBullet(const BulletBase* bullet)
{
	bool ret = false; //戻り値

	if (down)
	{
		if (HitSphere(bullet))
		{
			ret = true;
			HitBullet(bullet);
		}
	}

	if (!ret)
	{
		if (barrier != nullptr)
		{
			if (barrier->HitSphere(bullet))
			{
				if ((special_moves != nullptr) && !special_moves->EndCharge())
				{
					barrier->HitBullet(bullet);
				}
				ret = true;

			}
		}
		else
		{
			for (int i = 0; i < HAND_NUM; i++)
			{
				if (hand[i]->HitSphere(bullet))
				{
					ret = true;
					hand[i]->HitBullet(bullet);
					break;
				}
			}
		}
	}

	return ret;
}


//-----------------------------------
//アニメーション
//-----------------------------------
void LastBoss::Animation()
{
	animation++;
	if (animation % LAST_BOSS_ANIMATION == 0)
	{
		image_argument++;
	}
}

//-----------------------------------
//描画
//-----------------------------------
void LastBoss::Draw() const
{

	Location draw_location = location;
	Location camera = CameraWork::GetCamera();
	draw_location = draw_location - camera;

	int num = static_cast<int>(kind) - static_cast<int>(ENEMY_KIND::SLIME);

	if (down)
	{
		DrawRotaGraphF(draw_location.x, draw_location.y, 1.0, M_PI /180 * 30, images[num][image_argument % 4], TRUE);
	}
	else
	{
		DrawRotaGraphF(draw_location.x, draw_location.y, 1.0, 0.0, images[num][0],TRUE);
	}

	if (barrier != nullptr)
	{
		barrier->DrawDurabilityBar();
		barrier->Draw();
	}
	else if (state != ENEMY_STATE::IDOL)
	{//画面内に映って初めてHPを描画する
		DrawHPBar(LAST_BOSS_HP);
	}

	DrawDamageLog();
	DrawWeaknessIcon();
	for (int i = 0; i < HAND_NUM; i++)
	{
		hand[i]->Draw();
	}


	if (magic != nullptr)
	{
		for (int i = 0; i < magic_volume; i++)
		{
			if (magic[i] != nullptr)
			{
				magic[i]->Draw();
			}
		}
	}

	if (special_moves != nullptr)
	{
		special_moves->Draw();
	}

}

//-----------------------------------
//HPバーの描画
//-----------------------------------
void LastBoss::DrawHPBar(const int max_hp) const
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

	DrawBox(160, 10,SCREEN_WIDTH - 160, 40, 0x000000, TRUE);
	DrawBox(160, 10,160 + (960 * (static_cast<float>(hp) / max_hp)), 40, color, TRUE);
	DrawBox(160, 10, SCREEN_WIDTH - 160, 40, 0x8f917f, FALSE);
}

//-----------------------------------
//座標の取得
//-----------------------------------
Location LastBoss::GetLocation() const
{
	return location;
}

#ifdef _DEBUG
//-----------------------------------
//更新(DotByDot)
//-----------------------------------
void LastBoss::Update(const ENEMY_STATE state)
{

}

//-----------------------------------
//描画(DotByDot)
//-----------------------------------
void LastBoss::DebugDraw()
{

}
#endif //_DEBUG