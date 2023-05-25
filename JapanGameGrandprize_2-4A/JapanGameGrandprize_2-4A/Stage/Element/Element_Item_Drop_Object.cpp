#include "Element_Item_Drop_Object.h"
#include "../Player.h"
#include "../CameraWork.h"

Element_Item_Drop_Object::Element_Item_Drop_Object(short type,
	Resource resource, Location location, Area area)
	: Stage_Element_Base(&resource.images.at(0), location, area)
{

	this->area = area;
	this->type = type;
	this->images = images;

	//プレイヤーとの当たり判定を無効化
	margin_area = { 1000.0f,1000.0f };

	state = STATE::NONE;

	item_con = ItemController::GetInstance();
	//元素をランダムに設定
	for (int i = 0; i < 5; i++)
	{
		element_item.push_back(ElementItem(static_cast<ELEMENT_ITEM>(GetRand(6))));
	}

	drop_volume = 0;
}

Element_Item_Drop_Object::~Element_Item_Drop_Object()
{
	delete item_con;

	element_item.clear();
	element_item.shrink_to_fit();

	image = 0;
}

void Element_Item_Drop_Object::Update(Player* player)
{
	if (state == STATE::NONE)
	{
		BulletBase** player_bullet;
		player_bullet = player->GetBullet();

		for (int i = 0; i < BULLET_MAX; i++)
		{
			if (player_bullet[i] == nullptr) { continue; }

			if (player_bullet[i]->HitBox(this))
			{
				//プレイヤーの弾が当たったら破壊する
				state = STATE::DESTROY;

				int size = element_item.size();
		
				for (int i = 0; i < size; i++)
				{
					int rand_num = GetRand(5) + 1;
					element_item.at(i).SetVolume(rand_num);
					drop_volume += rand_num;

				}
				//破壊されたらアイテムを生成する
				item_con->Stage_SpawnItem(drop_volume, size, location, element_item);
				break;
			}
		}
	}
	else if(state==STATE::DESTROY)
	{
		item_con->Update(player);

		Item** item = item_con->GetItem();
		for (int i = 0; i < drop_volume; i++)
		{
			if (item[i] != nullptr)
			{
				break;
			}
			else if (i == drop_volume - 1)
			{
				state = STATE::EXTINGUISHMENT;
				SetArea({ -MAP_CHIP_SIZE, -MAP_CHIP_SIZE });
			}
		}

	}
}

void Element_Item_Drop_Object::Draw()const
{
	//画像がない又はエラーの場合は描画しない
	if (image == 0) { return; }

	if (state == STATE::NONE)
	{
		float x = location.x - CameraWork::GetCamera().x;
		float y = location.y - CameraWork::GetCamera().y;

		//DrawCircleAA(x, y, MAP_CHIP_SIZE,32, 0x00ff00, TRUE);
		//DrawRotaGraphF(x, y, 1.8f, 0, image, TRUE);
	}
	else
	{
		item_con->Draw();
	}
}
