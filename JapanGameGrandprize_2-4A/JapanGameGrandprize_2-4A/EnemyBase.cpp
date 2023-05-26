#include "EnemyBase.h"
#include "DxLib.h"
#include "vector"
#include "CameraWork.h"

int EnemyBase::log_font[4];
int* EnemyBase::icon_images = nullptr;
int EnemyBase::weakness_num[11];
ATTRIBUTE* EnemyBase::weakness[11];
std::vector<std::vector<int>> EnemyBase::images(12);
int EnemyBase::magic_circle_image = 0;

#define HP_BAR_Y1 20
#define HP_BAR_Y2 10

#define HP_BAR 60

//-----------------------------------
//コンストラクタ
//-----------------------------------
EnemyBase::EnemyBase() 
{
	can_delete = false;
	left_move = true;
	poison = false;
	paralysis = false;
	hp = 0;
	speed = 0;
	paralysis_time = 0;
	poison_damage = 0;
	poison_time = 0;
	drop_volume = 0;
	drop_type_volume = 0; 

	drop_element = nullptr;

	kind = ENEMY_KIND::NONE; 
	state = ENEMY_STATE::IDOL;

	InitDamageLog();

	if (icon_images == nullptr)
	{
		icon_images = new int[5];
		LoadDivGraph("Images/Enemy/Icon.png", 5, 5, 1, 23, 22, icon_images);
	}

	if (magic_circle_image == 0)
	{
		magic_circle_image = LoadGraph("Images/Enemy/Magic/MagicCircle.png");
	}

}

//-----------------------------------
//弱点属性の情報のロード
//-----------------------------------
void EnemyBase::LoadWeakness()
{
	FILE* file = NULL;

	char line[20];
	int temp[5]; //仮変数
	int num = 0;
	int i = 0;
	errno_t error;

	for (int i = 0; i < 11; i++)
	{
		weakness[i] = nullptr;
	}

	error = fopen_s(&file, "Data/Enemy/EnemyWeakness.csv", "r");

	if (error == 0)
	{
		while (fgets(line, 20, file))
		{
			sscanf_s(line, "%d,%d,%d,%d,%d,%d",
				&weakness_num[i], &temp[0], &temp[1], &temp[2], &temp[3], &temp[4]);
			if (0 < weakness_num[i])
			{
				weakness[i] = new ATTRIBUTE[weakness_num[i]];
			}

			for (int j = 4; 0 <= j; j--)
			{
				if (weakness_num[i] <= num)
				{
					break;
				}

				if (temp[j] == 3)
				{
					weakness[i][num] = static_cast<ATTRIBUTE>(j);
					num++;
				}
			}
			i++;
			num = 0;
		}
		fclose(file);
	}
}

//-----------------------------------
//ログ用のフォントの作成
//-----------------------------------
void EnemyBase::CreateLogFont()
{
	log_font[0] = CreateFontToHandle("Weakness", 24, 2, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	log_font[1] = CreateFontToHandle("Nomal", 20, 2, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	log_font[2] = CreateFontToHandle("Resistance", 20, 2, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	log_font[3] = CreateFontToHandle("Invalid", 16, 2, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
}

//-----------------------------------
//ログ用のフォントの削除
//-----------------------------------
void EnemyBase::DeleteLogFont()
{
	for (int i = 0; i < 4; i++)
	{
		DeleteFontToHandle(log_font[i]);
	}
}


// ---------------------------------- -
//弱点属性の情報の削除
//-----------------------------------
void EnemyBase::DeleteWeakness()
{
	if (weakness != nullptr)
	{
		for (int i = 0; i < 11; i++)
		{
			delete[] weakness[i];
		}
	}

	if (icon_images != nullptr)
	{
		for (int i = 0; i < 5; i++)
		{
			DeleteGraph(icon_images[i]);
		}
		delete[] icon_images;
		icon_images = nullptr;
	}
}

//画像の削除
void EnemyBase::DeleteImage()
{
	int size = 0;

	for (int i = 0; i < 12; i++)
	{
		if (!images[i].empty())
		{
			for (int j = 0; j < images[i].size(); j++)
			{
				DeleteGraph(images[i][j]);
			}
		}
		images[i].clear();
	}
}

//-----------------------------------
// HPが0かどうか判断(0になったらtrue)
//-----------------------------------
bool EnemyBase::CheckHp()
{

	bool ret = false;

	if (hp <= 0)
	{
		ret = true;
	}

	return ret;
}

//-----------------------------------
//画面外に出た
//-----------------------------------
bool EnemyBase::ScreenOut()
{

	bool ret = false; //戻り値
	Location scroll; //画面スクロールを考慮した座標
	Location camera = CameraWork::GetCamera(); //カメラ
	scroll = location - camera;

	if ((scroll.x < (-(SCREEN_WIDTH + area.width) * 2)) ||
		(((SCREEN_WIDTH + area.width) * 2) < scroll.x) ||
		(scroll.y < -(SCREEN_HEIGHT + area.height) * 2) ||
		(((SCREEN_HEIGHT + area.height) * 2) < scroll.y))
	{
		ret = true;
	}

	return ret;
}

//-----------------------------------
//ステージとの当たり判定
//-----------------------------------
HitMapChip EnemyBase::HitStage(const Stage* stage)
{

	HitMapChip ret = { false,nullptr }; //戻り値

	//マップチップ
	std::vector<MapChip*>map_chip = stage->GetMapChip();

	for (MapChip* chip : map_chip)
	{
		if (chip != nullptr)
		{
			Location chip_location = chip->GetLocation();
			Area chip_area = chip->GetArea();

			//EnemyのUpdate範囲外にあるmap_chipは計算しない
			if ((chip_location.x < location.x - SCREEN_WIDTH) ||
				(location.x + SCREEN_WIDTH < chip_location.x) ||
				(chip_location.y < location.y - SCREEN_HEIGHT) ||
				(location.y + SCREEN_HEIGHT < chip_location.y))
			{
				continue;
			}

			if ((location.x - (area.width / 2) <= chip_location.x + (chip_area.width / 2)) &&
				(chip_location.x - (chip_area.width / 2) <= location.x + (area.width / 2)))
			{
				if (HitBox(chip))
				{
					ret.hit = true;
					ret.chip = chip;
					break;
				}
			}
		}
	}

	return ret;
}

//-----------------------------------
//ステージのどの面と当たったを判断
//-----------------------------------
STAGE_DIRECTION EnemyBase::HitDirection(const MapChip* map_chip)
{

	STAGE_DIRECTION ret = STAGE_DIRECTION::TOP; //戻り値

	Location chip_location = map_chip->GetLocation();
	Area chip_area = map_chip->GetArea();
	Location vertex = location; //頂点
	float distance[4]; //距離
	int min_vertex; //当たった頂点
	float min_distance; //最低距離

	//左上との距離
	distance[0] = sqrtf(powf(chip_location.x - (location.x + area.width / 2), 2) +
		powf(chip_location.y - (location.y - area.height / 2), 2));

	//右上との距離
	distance[1] = sqrtf(powf(chip_location.x - (location.x - area.width / 2), 2) +
		powf(chip_location.y - (location.y - area.height / 2), 2));

	//左下との距離
	distance[2] = sqrtf(powf(chip_location.x - (location.x - area.width / 2), 2) +
		powf(chip_location.y - (location.y + area.height / 2), 2));

	//右下との距離
	distance[3] = sqrtf(powf(chip_location.x - (location.x + area.width / 2), 2) +
		powf(chip_location.y - (location.y + area.height / 2), 2));

	min_distance = distance[0];
	min_vertex = 0;

	for (int i = 1; i < 4; i++)
	{
		if (distance[i] < min_distance)
		{
			min_distance = distance[i];
			min_vertex = i;
		}
	}
	
	switch (min_vertex)
	{
	case 0:
		vertex = { location.x - (area.width / 4),location.y - (area.height / 4) };
		break;
	case 1:
		vertex = { location.x + (area.width / 4),location.y - (area.height / 4) };
		break;
	case 2:
		vertex = { location.x - (area.width / 4),location.y + (area.height / 4) };
		break;
	case 3:
		vertex = { location.x + (area.width / 4),location.y + (area.height / 4) };
		break;
	default:
		break;
	}

	//上面との距離
	distance[0] = sqrtf(powf(chip_location.x - vertex.x, 2) +
		powf((chip_location.y - CHIP_SIZE / 2) - vertex.y, 2));

	//下面との距離
	distance[1] = sqrtf(powf(chip_location.x - vertex.x, 2) +
		powf((chip_location.y + CHIP_SIZE / 2) - vertex.y, 2));

	//左面との距離
	distance[2] = sqrtf(powf((chip_location.x - CHIP_SIZE / 2) - vertex.x, 2) +
		powf(chip_location.y - vertex.y, 2));

	//右面との距離
	distance[3] = sqrtf(powf((chip_location.x + CHIP_SIZE / 2) - vertex.x, 2) +
		powf(chip_location.y - vertex.y, 2));

	min_distance = distance[0];
	for (int i = 1; i < 4; i++)
	{
		if (distance[i] < min_distance)
		{
			min_distance = distance[i];
			ret = static_cast<STAGE_DIRECTION>(i);
		}
	}

	return ret;
}

//-----------------------------------
//毒状態の処理
//-----------------------------------
void EnemyBase::Poison()
{
	int i;
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

	if (poison)
	{
		poison_time--;
		if (0 < poison_time)
		{
			if (poison_time % POISON_DAMAGE_FLAME == 0)
			{
				damage_log[i].log = true;
				damage_log[i].time = LOG_TIME;
				damage_log[i].damage = poison_damage;
				hp -= poison_damage;

				if (hp < 0)
				{
					hp = 0;
				}
			}
		}
		else
		{
			poison = false;
		}
	}
}

//-----------------------------------
//麻痺状態の処理
//-----------------------------------
void EnemyBase::Paralysis()
{

	if (paralysis)
	{
		paralysis_time--;
		if (paralysis_time < 0)
		{
			paralysis = false;
		}
	}
}

//ダメージログの更新
void EnemyBase::UpdateDamageLog()
{
	for (int i = 0; i < LOG_NUM; i++)
	{
		if (!damage_log[i].log)
		{
			break;
		}

		damage_log[i].time--;

		if (damage_log[i].time < 0)
		{
			damage_log[i].log = false;
			for (int j = i + 1; j < LOG_NUM; j++)
			{
				if (!damage_log[j].log)
				{
					break;
				}

				damage_log[j - 1] = damage_log[j];
				damage_log[j].log = false;
			}
			i--;
		}
	}
}


//-----------------------------------
//HPバーの描画
//-----------------------------------
void EnemyBase::DrawHPBar(const int max_hp) const
{
	Location draw_location = location;
	Location camera = CameraWork::GetCamera();

	draw_location = draw_location - camera;

	int color = GetColor(7,255,0);

	if (hp <= (max_hp / 2))
	{
		color = GetColor(255,255 * static_cast<float>(hp) / max_hp, 0);
	}
	else
	{
		color = GetColor(7 + 2 * (248 * (1 - static_cast<float>(hp) / max_hp)),255, 0);
	}

	DrawBox(draw_location.x - HP_BAR / 2, draw_location.y - (area.height / 2 + HP_BAR_Y1),
		draw_location.x + HP_BAR / 2, draw_location.y - (area.height / 2 + HP_BAR_Y2), 0x000000, TRUE);
	DrawBox(draw_location.x - HP_BAR / 2, draw_location.y - (area.height / 2 + HP_BAR_Y1),
		draw_location.x - HP_BAR / 2 + (HP_BAR * (static_cast<float>(hp) / max_hp)), draw_location.y - (area.height / 2 + HP_BAR_Y2), color, TRUE);
	DrawBox(draw_location.x - HP_BAR / 2, draw_location.y - (area.height / 2 + HP_BAR_Y1),
		draw_location.x + HP_BAR / 2, draw_location.y - (area.height / 2 + HP_BAR_Y2), 0x8f917f, FALSE);
}

//-----------------------------------
//ダメージログの描画
//-----------------------------------
void EnemyBase::DrawDamageLog()const
{
	int color = 0;
	Location draw_location = location;
	Location camera = CameraWork::GetCamera();

	draw_location = draw_location - camera;

	for (int i = 0; i < LOG_NUM; i++)
	{
		if (!damage_log[i].log)
		{
			break;
		}
		switch (damage_log[i].congeniality)
		{
		case CONGENIALITY::WEAKNESS:	//弱点
			color = 0xff0000;
			break;
		case CONGENIALITY::NORMAL:
			color = 0xffffff;
			break;
		case CONGENIALITY::RESISTANCE:	//耐性
			color = 0xc0c0c0;
			break;
		case CONGENIALITY::INVALID:		//無効
			color = 0xa1a3a6;
			break;
		default:
			break;
		}

		DrawFormatStringToHandle(draw_location.x - 20, draw_location.y - 110 - (LOG_TIME - damage_log[i].time),
			color, log_font[static_cast<int>(damage_log[i].congeniality)],
			"%3d", damage_log[i].damage);

	}
}

//弱点属性のアイコン
void EnemyBase::DrawWeaknessIcon() const
{
	Location draw_location = location;
	Location camera = CameraWork::GetCamera();

	draw_location = draw_location - camera;

	int num = static_cast<int>(kind) - static_cast<int>(ENEMY_KIND::SLIME); //弱点の数の添え字
	for (int i = 0; i < weakness_num[num]; i++)
	{
		if (static_cast<int>(kind) < static_cast<int>(ENEMY_KIND::SLIME_BOSS))
		{
			DrawRotaGraphF((draw_location.x + HP_BAR / 2) - (23 * i),
				draw_location.y - (area.height / 2 + HP_BAR_Y1 + 10), 0.8, 0.0,
				icon_images[static_cast<int>(weakness[num][i])], TRUE, TRUE);
		}
		else
		{
			DrawRotaGraphF(SCREEN_WIDTH - 175 - (46 * i),60, 1.6, 0.0,
				icon_images[static_cast<int>(weakness[num][i])], TRUE, TRUE);
		}

	}
}

//---------------------------------
// 被弾時のSE再生
//---------------------------------
void EnemyBase::PlayHitBulletSound(ATTRIBUTE attribute)const
{
	int se = EnemySE::GetBulletSE(attribute);

	if (!se)return;

	PlaySoundMem(EnemySE::GetBulletSE(attribute), DX_PLAYTYPE_BACK);
	
}

//-----------------------------------
//ドロップする種類の量の取得
//-----------------------------------
int EnemyBase::GetDropTypeVolume() const
{

	return drop_type_volume;
}

//-----------------------------------
//ドロップするアイテムの量の取得
//-----------------------------------
int EnemyBase::GetDropVolume() const
{

	return drop_volume;
}

//-----------------------------------
// ドロップアイテムの取得
//-----------------------------------
ElementItem EnemyBase::GetDropItem(int i) const
{ 

	return *drop_element[i]; 
}

//-----------------------------------
// エネミーの種類の取得
//-----------------------------------
ENEMY_KIND EnemyBase::GetEnemyKind() const
{

	return kind;
}

//-----------------------------------
//エネミーの状態の取得
//-----------------------------------
ENEMY_STATE EnemyBase::GetState()const
{

	return state;
}

//-----------------------------------
//削除可能状態の取得
//-----------------------------------
bool EnemyBase::GetCanDelete() const
{

	return can_delete;
}

//-----------------------------------
//ダメージログの初期化
//-----------------------------------
void EnemyBase::InitDamageLog()
{
	for (int i = 0; i < LOG_NUM; i++)
	{
		damage_log[i].log = false;
		damage_log[i].damage = -1;
		damage_log[i].congeniality = CONGENIALITY::NORMAL;
	}
}