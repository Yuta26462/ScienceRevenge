#include "ItemController.h"
#include "EnemyBase.h"
#include "Player.h"

ItemController* ItemController::instance = nullptr;

//-----------------------------------
//コンストラクタ
//-----------------------------------
ItemController::ItemController()
{

	item_volume = 0;
	item_max = 0;
	item = nullptr;
}

//-----------------------------------
//デストラクタ
//-----------------------------------
ItemController::~ItemController()
{
	if (item != nullptr)
	{
		for (int i = 0; i < item_max; i++)
		{
			delete item[i];
		}

		delete[] item;
	}
	instance = nullptr;
}

//-----------------------------------
//更新
//-----------------------------------
void ItemController::Update(Player* player)
{

	for (int i = 0; i < item_volume; i++)
	{
		if (item[i] == nullptr)
		{
			break;
		}

		item[i]->Update(player->GetLocation());

		if (item[i]->HitBox(player))
		{
			player->SetElementItem(item[i]);
			delete item[i];
			item[i] = nullptr;
			SortItem(i);
			i--;
			item_volume--;
		}
	}
}

//-----------------------------------
//アイテムの生成
//-----------------------------------
void ItemController::SpawnItem(const EnemyBase* enemy)
{

	int old_item_max = item_max;//生成可能なエネミー数

	item_volume += enemy->GetDropVolume();

	if (item_max < item_volume)
	{
		item_max = item_volume;
		ArrangementItem(old_item_max);
	}

	int volume = 0; //生成数
	int j = 0;

	//アイテムの生成
	for (int i = 0; i < enemy->GetDropTypeVolume(); i++)
	{
		volume = enemy->GetDropItem(i).GetVolume();

		for (int j = 0; j < volume; j++)
		{
			for (int n = 0; n < item_max; n++)
			{
				if (item[n] == nullptr)
				{
					item[n] = new Item(enemy->GetDropItem(i).GetType(),
						                enemy->GetLocation());
					break;
				}
			}
		}
	}
}


void ItemController::Stage_SpawnItem(int drop_volume, int dorp_type_volume, Location location, std::vector<ElementItem> element_item)
{

	int old_item_max = item_max;//生成可能なエネミー数

	this->item_volume = drop_volume;

	if (item_max < item_volume)
	{
		item_max = item_volume;
		ArrangementItem(old_item_max);
	}

	int volume = 0; //生成数
	int j = 0;

	//アイテムの生成
	for (int i = 0; i < dorp_type_volume; i++)
	{
		volume = element_item.at(i).GetVolume();

		for (int j = 0; j < volume; j++)
		{
			for (int n = 0; n < item_max; n++)
			{
				if (item[n] == nullptr)
				{
					item[n] = new Item(element_item.at(i).GetType(),
						location);
					break;
				}
			}
		}
	}
}

//-----------------------------------
//アイテムの並び替え
//-----------------------------------
void ItemController::SortItem(const int item_num)
{

	//弾の中身をソートする
	for (int i = item_num + 1; i < item_max; i++)
	{
		if ((item[i] == nullptr))
		{
			break;
		}

		item[i - 1] = item[i];
		item[i] = nullptr;
	}
}

//-----------------------------------
//アイテムの整理
//-----------------------------------
void ItemController::ArrangementItem(const int old_item_max)
{

	if (item == nullptr)
	{
		//アイテムの初期化
		item = new Item * [item_max];

		for (int i = 0; i < item_max; i++)
		{
			item[i] = nullptr;
		}
	}
	else
	{
		Item** temporary_item; //避難用

		//もともとのアイテム一時的に避難する
		temporary_item = item;

		item = new Item * [item_max];

		//避難させたアイテムを元に戻す
		for (int i = 0; i < item_max; i++)
		{
			if (i < old_item_max)
			{
				item[i] = temporary_item[i];
			}
			else
			{
				item[i] = nullptr;
			}
		}

		delete[] temporary_item;
	}
}

//-----------------------------------
//描画
//-----------------------------------
void ItemController::Draw() const
{

	for (int i = 0; i < item_volume; i++)
	{
		if (item[i] == nullptr)
		{
			break;
		}

		item[i]->Draw();
	}
}