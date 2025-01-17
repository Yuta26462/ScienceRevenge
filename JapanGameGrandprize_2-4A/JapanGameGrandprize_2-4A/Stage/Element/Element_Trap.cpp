#include "Element_Trap.h"
#include "../Player.h"
#include "../CameraWork.h"
#include "../NormalBullet.h"

#define TIMER	4.0f

#define ATTACK_RANGE 200.0f

Element_Trap::Element_Trap(short type, Stage* stage,
	EnemyBase** enemy, Resource resource, Location location, Area area)
	: Stage_Element_Base(&resource.images.at(0), location, area)
{
	this->area = area;
	this->type = type;
	
	//プレイヤーとの当たり判定を無効化
	margin_area = { 0.0f,-9999.0f };

	this->resource = resource;
	this->images = resource.images;

	this->stage = stage;
	this->enemy = enemy;

	explosion.atribute = ATTRIBUTE::EXPLOSION;
	explosion.damage = 40;

	this->bullet = new NormalBullet(location.x, location.y, false, &explosion);

	state = STATE::NONE;
}

Element_Trap::~Element_Trap()
{
	image = 0;
}

void Element_Trap::Update(Player* player)
{
	
	if (state == STATE::EXTINGUISHMENT) { return; }

	BulletBase** player_bullet;
	player_bullet = player->GetBullet();

	//プレイヤーの弾との当たり判定
	for (int j = 0; j < BULLET_MAX; j++)
	{
		if (player_bullet[j] == nullptr) { continue; }


		if (player_bullet[j]->HitBox(this))
		{
			//範囲内にプレイヤーがいたらダメージを与える
			if (abs(location.x - player->GetLocation().x) <= ATTACK_RANGE
				&& abs(location.y - player->GetLocation().y) <= ATTACK_RANGE)
			{
				ENEMY_TYPE fireType = ENEMY_TYPE::FIRE;
				player->HpDamage(AttackResource{ 10, &fireType, 5 });
			}


			state = STATE::EXPLOSION;

			delete player_bullet[j];
			player_bullet[j] = nullptr;
			player->SortBullet(j);
			j--;
		}

		
	}

	if (state == STATE::EXPLOSION) 
	{
		PlaySurroundMem(resource.sounds.at(0), player, this);

		vector<ENEMY_LOCATION> spawn;
		spawn = stage->GetEnemy_SpawnLocation();

		for (int i = 0; i < spawn.size(); i++)
		{
			if (enemy[i] != nullptr)
			{
				//範囲内にエネミーがいたらダメージを与える
				if (abs(location.x - enemy[i]->GetLocation().x) <= ATTACK_RANGE
					&& abs(location.y - enemy[i]->GetLocation().y) <= ATTACK_RANGE)
				{
					enemy[i]->HitBullet(bullet);
				}
			}
		}

		if (GetElapsedTime(1.0f) > 0.3f)
		{
			SetImage(0);
			SetArea({ -MAP_CHIP_SIZE, -MAP_CHIP_SIZE });
		}

		std::function<void()> explosion = [&]() 
		{
			state = STATE::EXTINGUISHMENT;
		};

		LoopTimer(1.0f, &explosion);
	}
}

void Element_Trap::Draw()const
{

	//画像がない又はエラーの場合は描画しない
	if (image == 0) { return; }

	//printfDx("camera_x:%f\tcamera_y:%f\n", CameraWork::GetCamera().x, CameraWork::GetCamera().y);
	float x = location.x - CameraWork::GetCamera().x;
	float y = location.y - CameraWork::GetCamera().y;

	DrawRotaGraphF(x, y, 1.8f, 0, image, TRUE);

	if (state == STATE::EXPLOSION) 
	{
		float elapsed_time = GetElapsedTime(1.0f) * 255;
		SetDrawBlendMode(DX_BLENDMODE_ADD, 255 - elapsed_time);
		DrawCircleAA(x, y, ATTACK_RANGE, 40, 0xFFFFFF, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}