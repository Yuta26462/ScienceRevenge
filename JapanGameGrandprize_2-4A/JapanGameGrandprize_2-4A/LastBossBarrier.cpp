#include "LastBossBarrier.h"
#include "DxLib.h"
#include "CameraWork.h"
#include "NormalBullet.h"

//耐久値
#define DURABILITY 500

//画像枚数
#define BARRIER_NUM 14

//エフェクトの本体のアニメーション
#define BARRIER_ANIMATION 2



//-----------------------------------
//コンストラクタ
//-----------------------------------
LastBossBarrier::LastBossBarrier(const Location spawn_location)
{
	end_deployment = false;

	location = spawn_location;
	radius = 160;
	images = new int[14];

	LoadDivGraph("Images/Enemy/SpecialMoves/barrier2_2.png", BARRIER_NUM, 2, 7, 384, 383,images);


		animation = 0;
	size = 2.0;
	angle = 0.0;
	count = 0;
	durability = DURABILITY;
	old_durability = durability;
	alpha = 255;
}

//-----------------------------------
//デストラクタ
//-----------------------------------
LastBossBarrier::~LastBossBarrier()
{
	for (int i = 0; i < BARRIER_NUM; i++)
	{
		DeleteGraph(images[i]);
	}
	delete[] images;
}

//-----------------------------------
//更新
//-----------------------------------
void LastBossBarrier::Update()
{
	count++;



	if (!end_deployment)
	{
		Deployment();
	}
	else
	{
		alpha = static_cast<int>(255 * (static_cast<double>(durability) / DURABILITY));
		if (count % static_cast<int>(BARRIER_ANIMATION * 2.5) == 0)
		{
			animation = (++animation) % 3;
		}

	}
	
}

//バリアの展開
void LastBossBarrier::Deployment()
{
	PlaySoundMem(EnemySE::GetBarrierSE().appearance, DX_PLAYTYPE_BACK);
	
	if (count % BARRIER_ANIMATION == 0)
	{
		animation++;
	}
	if (9 < animation)
	{
		end_deployment = true;
	}
}

//-----------------------------------
//プレイヤーの弾との当たり判定
//-----------------------------------
void LastBossBarrier::HitBullet(const BulletBase* bullet)
{
	PlaySoundMem(EnemySE::GetBarrierSE().hit, DX_PLAYTYPE_BACK);
	durability -= bullet->GetDamage();
	if (durability < 0)
	{
		durability = 0;
	}
}

//-----------------------------------
//描画
//-----------------------------------
void LastBossBarrier::Draw() const
{
	Location draw_location = location;
	Location camera = CameraWork::GetCamera();
	draw_location = draw_location - camera;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	if (end_deployment)
	{
		DrawRotaGraphF(draw_location.x, draw_location.y, size, angle, images[animation + 8], TRUE);
	}
	else
	{
		DrawRotaGraphF(draw_location.x, draw_location.y, size, angle, images[animation], TRUE);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

}

//-----------------------------------
//耐久値バーの描画
//-----------------------------------
void LastBossBarrier::DrawDurabilityBar() const
{
	DrawBox(160, 10, SCREEN_WIDTH - 160, 40, 0x000000, TRUE);
	DrawBox(160, 10, 160 + (960 * (static_cast<float>(durability) / DURABILITY)), 40, 0xffffff, TRUE);
	DrawBox(160, 10, SCREEN_WIDTH - 160, 40, 0x8f917f, FALSE);
}

//-----------------------------------
//破壊された
//-----------------------------------
bool LastBossBarrier::Break() const
{
	bool ret = false;

	if (durability <= 0)
	{
		PlaySoundMem(EnemySE::GetBarrierSE().breaked, DX_PLAYTYPE_BACK);
		ret = true;
	}

	return ret;
}