#pragma once
#include"Item.h"
#include "ElementItem.h"
#include <vector>

class ItemController
{
private:
	static ItemController* instance; //インスタンス

private:
	//コンストラクタ
	ItemController();

public:
	static ItemController* GetInstance()
	{
		if (instance == nullptr)
		{
			instance = new ItemController();
		}

		return instance;
	}
	//シングルトン
private:

	int item_volume; //生成しているアイテムの量
	int item_max; //アイテムの生成できる最大数
	Item** item; //アイテム

public:

	//デストラクタ
	~ItemController();

	//更新
	void Update( class Player* player);

	//描画
	void Draw() const;

	//アイテムの生成
	void SpawnItem(const class EnemyBase* enemy);

	void Stage_SpawnItem(int drop_volume, int dorp_type_volume, Location location, std::vector<ElementItem>element_item);

	//アイテムの並び替え
	void SortItem(const int item_num);

	//アイテムの整理
	void ArrangementItem(const int old_item_volume);

	Item** GetItem() { return item; }
};