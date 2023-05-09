#include "Player.h"
#include "DxLib.h"
#include "NormalBullet.h"
#include "PadInput.h"
#include "CameraWork.h"
#include "Item.h"
#include <iostream>

#define ANIMATION_MOVE 10
#define CHEMICAL_FORMURA_DRAW_X 50
#define CHEMICAL_FORMURA_DRAW_Y 150
#define PLAYER_SIZE 0.32

//-----------------------------------
// コンストラクタ
//-----------------------------------
Player::Player()
{

	stage_builder = new StageBuilder();
	objects = stage_builder->OutPutObjects();

	animation = 0;
	location.x = 100;
	location.y = 60;
	image = new int[PLAYER_IMAGES];
	LoadDivGraph("Images/Player/Player.png", 7, 7, 1, 250, 250, image);
	jump_image = new int[3];
	LoadDivGraph("Images/Player/Fly_ON.png", 3, 3, 1, 250, 250, jump_image);
	image_size_x = 40;
	image_size_y = 80;
	area.width = image_size_x;
	area.height = image_size_y;
	bullet_count = 0;
	shoot_count = 0;
	flashing_count = 0;
	damage_count = 0;
	fly = 0.0;
	fly_power = 0.0;
	not_jet_count = 0;
	speed_x = 0.0;
	fuel = 100.0;
	gravity_down = 0.0;
	old_x = 0.0;
	old_y = 0.0;
	damage = 0;
	image_count = 0;
	jump_power = 0.0f;

	bullet = new BulletBase * [BULLET_MAX];

	for (int i = 0; i < BULLET_MAX; i++)
	{
		bullet[i] = nullptr;
	}

	move_left = false;

	damage_flg = false;
	i = 0;

	attribute[0] = ATTRIBUTE::NORMAL;
	attribute[1] = ATTRIBUTE::EXPLOSION;
	attribute[2] = ATTRIBUTE::MELT;
	attribute[3] = ATTRIBUTE::POISON;
	attribute[4] = ATTRIBUTE::PARALYSIS;
	attribute[5] = ATTRIBUTE::HEAL;

	attribute_c[0] = "NORMAL";
	attribute_c[1] = "EXPLOSION";
	attribute_c[2] = "MELT";
	attribute_c[3] = "POISON";
	attribute_c[4] = "PARALYSIS";
	attribute_c[5] = "HEAL";

	explosion = nullptr;
	melt = nullptr;
	poison = nullptr;
	pararysis = nullptr;
	heal = nullptr;

	player_state = PLAYER_STATE::STOP;

	display_attribute = 0;

	hp = 0;

	beam = nullptr;

	stage = nullptr;

	//元素の初期化
	element = new ElementItem * [PLAYER_ELEMENT];
	for (int i = 0; i < PLAYER_ELEMENT; i++)
	{
		element[i] = new ElementItem(static_cast<ELEMENT_ITEM>(i));
	}

	pouch = nullptr;
}

//-----------------------------------
// コンストラクタ
//-----------------------------------
Player::Player(Stage* stage)
{
	stage_builder = new StageBuilder();
	objects = stage_builder->OutPutObjects();

	animation = 0;
	area.height = 80;
	area.width = 40;

	this->stage = stage;
	location.x = stage->GetSpawnPoint().x;
	location.y = (stage->GetSpawnPoint().y - MAP_CHIP_SIZE / 2) - 1.0;
	image = new int[PLAYER_IMAGES];
	LoadDivGraph("Images/Player/Player.png", 7, 7, 1, 250, 250, image);
	jump_image = new int[3];
	LoadDivGraph("Images/Player/Fly_ON.png", 3, 3, 1, 250, 250, jump_image);
	attribute_images = new int[ATTRIBUTE_IMAGES];
	LoadDivGraph("Images/Player/zokusei_icon_x2.png", 10, 5, 2, 55, 51, attribute_images);
	hp_image = LoadGraph("Images/Player/HP_Bar.png");

	image_size_x = 40;
	image_size_y = 80;
	area.width = image_size_x;
	area.height = image_size_y;
	bullet_count = 0;
	damage_count = 0;
	shoot_count = 0;
	select_count = 0;
	damage = 0;
	flashing_count = 0;
	fly = 0.0;
	fly_power = 0.0;
	not_jet_count = 0;
	speed_x = 0.0;
	old_x = 0.0;
	old_y = 0.0;
	fuel = 100.0;
	gravity_down = 0.0;
	jump_power = 100.0f;
	jump_bottun_count = 0;
	image_count = 0;

	normal.atribute = ATTRIBUTE::NORMAL;
	normal.chemical_formula[0] = 'n';
	normal.chemical_formula_name[0] = 'a';
	normal.damage = 5;
	normal.damage_per_second = 0;
	normal.material.carbon = 0;
	normal.material.chlorine = 0;
	normal.material.hydrogen = 0;
	normal.material.nitrogen = 0;
	normal.material.oxygen = 0;
	normal.material.sulfur = 0;
	normal.material.uranium = 0;
	normal.number_of_bullets = 50;
	normal.time = 0;
	normal.make_bool = true;

	bullet = new BulletBase * [BULLET_MAX];

	for (int i = 0; i < BULLET_MAX; i++)
	{
		bullet[i] = nullptr;
	}

	damage_flg = false;
	fire_flg = false;
	fire_second_bool = false;
	pouch_open = false;
	move_left = false;
	i = 0;
	damage_by_second = 0;
	damage_time = 0;

	attribute[0] = ATTRIBUTE::NORMAL;
	attribute[1] = ATTRIBUTE::EXPLOSION;
	attribute[2] = ATTRIBUTE::MELT;
	attribute[3] = ATTRIBUTE::POISON;
	attribute[4] = ATTRIBUTE::PARALYSIS;
	attribute[5] = ATTRIBUTE::HEAL;

	attribute_c[0] = "NORMAL";
	attribute_c[1] = "EXPLOSION";
	attribute_c[2] = "MELT";
	attribute_c[3] = "POISON";
	attribute_c[4] = "PARALYSIS";
	attribute_c[5] = "HEAL";

	player_state = PLAYER_STATE::STOP;

	display_attribute = 0;

	hp = 100;

	explosion = nullptr;
	melt = nullptr;
	poison = nullptr;
	pararysis = nullptr;
	heal = nullptr;

	beam = nullptr;

	this->stage = stage;

	area = { 80,40 };

	//元素の初期化
	element = new ElementItem * [PLAYER_ELEMENT];
	for (int i = 0; i < PLAYER_ELEMENT; i++)
	{
		element[i] = new ElementItem(static_cast<ELEMENT_ITEM>(i));
	}

	pouch = new Pouch();

	for (int i = 0; i < PLAYER_ELEMENT; i++)
	{
		pouch->SetElement(element[i], i);
		pouch->SetElementConstruct(i);
	}
}

//-----------------------------------
// デストラクタ
//-----------------------------------
Player::~Player()
{

	for (int i = 0; i < BULLET_MAX; i++)
	{
		delete bullet[i];
	}
	delete[] bullet;
}

//-----------------------------------
// 描画
//-----------------------------------
void Player::Draw() const
{

	float x = location.x - CameraWork::GetCamera().x;
	float y = location.y - CameraWork::GetCamera().y;

	PlayerUiDraw(x, y);



	for (int i = 0; i < bullet_count; i++)
	{
		if (bullet[i] != nullptr)
		{
			bullet[i]->Draw();
		}
	}

	//ダメージを受けた時点滅する
	if (damage_flg)
	{
		if (player_state == PLAYER_STATE::FLY)
		{
			if (flashing_count < 5)
			{
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 0);
				DrawRotaGraphF(x, y, PLAYER_SIZE, 0, jump_image[image_count], TRUE, move_left);

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			else if (flashing_count < 10)
			{
				DrawRotaGraphF(x, y, PLAYER_SIZE, 0, jump_image[image_count], TRUE, move_left);
			}
			else {}
		}
		if (flashing_count < 5)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 0);
			DrawRotaGraphF(x, y, PLAYER_SIZE, 0, image[image_count], TRUE, move_left);

			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		else if (flashing_count < 10)
		{
			DrawRotaGraphF(x, y, PLAYER_SIZE, 0, image[image_count], TRUE, move_left);
		}
		else {}
	}
	else
	{
		if (player_state == PLAYER_STATE::FLY)
		{
			DrawRotaGraphF(x, y, PLAYER_SIZE, 0, jump_image[image_count], TRUE, move_left);
		}
		else
		{
			DrawRotaGraphF(x, y, PLAYER_SIZE, 0, image[image_count], TRUE, move_left);
		}
	}

	SetFontSize(30);



	DrawFormatString(0, 400, 0x999999, "%d", hp);
}

void Player::PouchDraw() const
{
	if (pouch_open)
	{
		pouch->Draw();
	}
}

void Player::PlayerUiDraw(float x, float y) const
{
	//燃料ゲージ
	float now_fuel = (fuel / FUEL_MAX) * FUEL_BAR_HEIGHT;
	float fuel_x = x - 50;
	float fuel_y = y - (area.height / 2);
	if (fuel >= 50)
	{
		DrawBoxAA(fuel_x, fuel_y + (FUEL_MAX - now_fuel),
			fuel_x + 5, fuel_y + FUEL_BAR_HEIGHT, GREEN, TRUE);
	}
	else if (fuel >= 20)
	{
		DrawBoxAA(fuel_x, fuel_y + (FUEL_MAX - now_fuel),
			fuel_x + 5, fuel_y + FUEL_BAR_HEIGHT, YELLOW, TRUE);
	}
	else
	{
		DrawBoxAA(fuel_x, fuel_y + (FUEL_MAX - now_fuel),
			fuel_x + 5, fuel_y + FUEL_BAR_HEIGHT, RED, TRUE);
	}
	//ここまで

	//Hpゲージ
	//HPバーの表示ここから
	float now_hp = (hp / HP_MAX) * HP_BAR_WIDTH;
	float hp_start = 120;
	float hp_y = 620;
	if (hp >= 50)
	{
		DrawBoxAA(hp_start, hp_y, hp_start + (now_hp - 1), hp_y + HP_BAR_HEIGHT, GREEN, TRUE);
	}
	else if (hp >= 20)
	{
		DrawBoxAA(hp_start, hp_y, hp_start + (now_hp - 1), hp_y + HP_BAR_HEIGHT, YELLOW, TRUE);
	}
	else
	{
		DrawBoxAA(hp_start, hp_y, hp_start + (now_hp - 1), hp_y + HP_BAR_HEIGHT, RED, TRUE);
	}
	DrawRotaGraphF(230, 580, 0.75, 0, hp_image, true);
	//ここまで

	//現在の選択肢
	float chemical_formula_y = hp_y - 25;
	float bullet_remain_y = chemical_formula_y - 35;
	float bullet_remain_x = hp_start - 70;
	float chemical_icon_x = hp_start - 43;
	float chemical_icon_y = hp_y + 25;
	switch (display_attribute)
	{
	case 0:
		DrawStringF(hp_start, chemical_formula_y, "--", 0x00ff00);
		DrawStringF(bullet_remain_x, bullet_remain_y, "--", 0x00ff00);
		break;
	case 1:
		DrawGraph(hp_start, chemical_formula_y, explosion->name_image, TRUE);
		DrawFormatStringF(bullet_remain_x, bullet_remain_y, 0xffffff, "%d", explosion->number_of_bullets);
		DrawRotaGraph(chemical_icon_x, chemical_icon_y, 1, 0,
			attribute_images[5], TRUE);
		break;
	case 2:
		DrawGraph(hp_start, chemical_formula_y, melt->name_image, TRUE);
		DrawFormatStringF(bullet_remain_x, bullet_remain_y, 0xffffff, "%d", melt->number_of_bullets);
		DrawRotaGraph(chemical_icon_x, chemical_icon_y, 1, 0,
			attribute_images[9], TRUE);
		break;
	case 3:
		DrawGraph(hp_start, chemical_formula_y, poison->name_image, TRUE);
		DrawFormatStringF(bullet_remain_x, bullet_remain_y, 0xffffff, "%d", poison->number_of_bullets);
		DrawRotaGraph(chemical_icon_x, chemical_icon_y, 1, 0,
			attribute_images[6], TRUE);
		break;
	case 4:
		DrawGraph(hp_start, chemical_formula_y, pararysis->name_image, TRUE);
		DrawFormatStringF(bullet_remain_x, bullet_remain_y, 0xffffff, "%d", pararysis->number_of_bullets);
		DrawRotaGraph(chemical_icon_x, chemical_icon_y, 1, 0,
			attribute_images[7], TRUE);
		break;
	case 5:
		DrawGraph(hp_start, chemical_formula_y, heal->name_image, TRUE);
		DrawFormatStringF(bullet_remain_x, bullet_remain_y, 0xffffff, "%d", heal->number_of_bullets);
		DrawRotaGraph(chemical_icon_x, chemical_icon_y, 1, 0,
			attribute_images[8], TRUE);
		break;
	default:
		break;
	}
	ChemicalFormulaDraw(display_attribute, 0);
	float element_thing = hp_start + 15;
	SetFontSize(30);
	DrawFormatString(element_thing, 670, 0xffffff, "%d", element[2]->GetVolume());
	DrawFormatString(element_thing + (50 * 1), 670, 0xffffff, "%d", element[0]->GetVolume());
	DrawFormatString(element_thing + (50 * 2), 670, 0xffffff, "%d", element[3]->GetVolume());
	DrawFormatString(element_thing + (50 * 3), 670, 0xffffff, "%d", element[1]->GetVolume());
	DrawFormatString(element_thing + (50 * 4), 670, 0xffffff, "%d", element[4]->GetVolume());
	DrawFormatString(element_thing + (50 * 5), 670, 0xffffff, "%d", element[5]->GetVolume());
}

void Player::ChemicalFormulaDraw(int i, int plus_y) const
{
	switch (i)
	{
	case 0: //通常弾
		DrawFormatString(CHEMICAL_FORMURA_DRAW_X, CHEMICAL_FORMURA_DRAW_Y + plus_y, 0x778877, "--");
		break;
	case 1: //爆発
		if (explosion != nullptr)
		{
			DrawFormatString(CHEMICAL_FORMURA_DRAW_X, CHEMICAL_FORMURA_DRAW_Y + plus_y, 0x778877, "%d", explosion->number_of_bullets);
			DrawFormatString(CHEMICAL_FORMURA_DRAW_X + 30, CHEMICAL_FORMURA_DRAW_Y + plus_y, 0x778877, "%s", explosion->chemical_formula_name);
		}
		else
		{
			DrawFormatString(CHEMICAL_FORMURA_DRAW_X, CHEMICAL_FORMURA_DRAW_Y + plus_y, 0x778877, "0");
		}
		break;
	case 2: //溶解
		if (melt != nullptr)
		{
			DrawFormatString(CHEMICAL_FORMURA_DRAW_X, CHEMICAL_FORMURA_DRAW_Y + plus_y, 0x778877, "%d", melt->number_of_bullets);
			DrawFormatString(CHEMICAL_FORMURA_DRAW_X + 30, CHEMICAL_FORMURA_DRAW_Y + plus_y, 0x778877, "%s", melt->chemical_formula_name);
		}
		else
		{
			DrawFormatString(CHEMICAL_FORMURA_DRAW_X, CHEMICAL_FORMURA_DRAW_Y + plus_y, 0x778877, "0");
		}
		break;
	case 3: //毒
		if (poison != nullptr)
		{
			DrawFormatString(CHEMICAL_FORMURA_DRAW_X, CHEMICAL_FORMURA_DRAW_Y + plus_y, 0x778877, "%d", poison->number_of_bullets);
			DrawFormatString(CHEMICAL_FORMURA_DRAW_X + 30, CHEMICAL_FORMURA_DRAW_Y + plus_y, 0x778877, "%s", poison->chemical_formula_name);
		}
		else
		{
			DrawFormatString(CHEMICAL_FORMURA_DRAW_X, CHEMICAL_FORMURA_DRAW_Y + plus_y, 0x778877, "0");
		}
		break;
	case 4: //麻痺
		if (pararysis != nullptr)
		{
			DrawFormatString(CHEMICAL_FORMURA_DRAW_X, CHEMICAL_FORMURA_DRAW_Y + plus_y, 0x778877, "%d", pararysis->number_of_bullets);
			DrawFormatString(CHEMICAL_FORMURA_DRAW_X + 30, CHEMICAL_FORMURA_DRAW_Y + plus_y, 0x778877, "%s", pararysis->chemical_formula_name);
		}
		else
		{
			DrawFormatString(CHEMICAL_FORMURA_DRAW_X, CHEMICAL_FORMURA_DRAW_Y + plus_y, 0x778877, "0");
		}
		break;
	case 5: //回復
		if (heal != nullptr)
		{
			DrawFormatString(CHEMICAL_FORMURA_DRAW_X, CHEMICAL_FORMURA_DRAW_Y + plus_y, 0x778877, "%d", heal->number_of_bullets);
			DrawFormatString(CHEMICAL_FORMURA_DRAW_X + 30, CHEMICAL_FORMURA_DRAW_Y + plus_y, 0x778877, "%s", heal->chemical_formula_name);
		}
		else
		{
			DrawFormatString(CHEMICAL_FORMURA_DRAW_X, CHEMICAL_FORMURA_DRAW_Y + plus_y, 0x778877, "0");
		}
		break;
	default:
		break;
	}
}

//-----------------------------------
// 更新
//-----------------------------------
void Player::Update()
{
	old_x = location.x;
	old_y = location.y;

	//ボス部屋に入った際、動きを止め飛んでいたら落下させる
	if (CameraWork::GetCameraState() == CameraWork::STATE::BOSS)
	{
		if (player_state == PLAYER_STATE::FLY || player_state == PLAYER_STATE::DOWN || player_state == PLAYER_STATE::STOP)
		{
			NotFly();
		}
		return;
	}

	float old_rstick_r_x = PAD_INPUT::GetRStick().x;
	float old_rstick_l_x = PAD_INPUT::GetLStick().x;

	if (damage_flg == true)
	{
		damage_count++;
		if (damage_count <= damage)
		{
			if (hp > 0)
			{
				hp--;
			}
			else
			{
				hp = 0;
				player_state = PLAYER_STATE::DEATH;
			}
		}

		if (flashing_count++ >= 10)
		{
			flashing_count = 0;
		}

		if (damage_count % 120 == 0)
		{
			damage_flg = false;
			damage_count = 0;
		}
	}

	if (fire_flg)
	{
		if (++damage_count % 60 == 0)
		{
			if (!fire_second_bool)
			{
				fire_second_bool = true;
			}
		}

		if (fire_second_bool)
		{
			if (damage_second++ < damage_by_second)
			{
				hp--;
			}
			else
			{
				damage_second = 0;
				fire_second_bool = false;
			}
		}

		if (damage_count > damage_time)
		{
			fire_flg = false;
			damage_time = 0;
			damage_count = 0;
		}

	}




	if (PAD_INPUT::OnButton(XINPUT_BUTTON_Y) && !pouch_open)
	{
		pouch_open = true;
		for (int i = 0; i < PLAYER_ELEMENT; i++)
		{
			pouch->SetElement(element[i], i);
		}
	}
	else if (PAD_INPUT::OnButton(XINPUT_BUTTON_Y) && pouch_open)
	{
		pouch_open = false;
	}


	//ポーチオープン
	if (pouch_open)
	{
		pouch->Update();
		if (pouch->GetOnBool())
		{
			switch (pouch->GetAttribute())
			{
			case ATTRIBUTE::EXPLOSION:
				if (pouch->GetExplosion()->make_bool)
				{
					explosion = pouch->GetExplosion();
				}
				break;
			case ATTRIBUTE::MELT:
				if (pouch->GetMelt()->make_bool)
				{
					melt = pouch->GetMelt();
				}
				break;
			case ATTRIBUTE::POISON:
				if (pouch->GetPoison()->make_bool)
				{
					poison = pouch->GetPoison();
				}
				break;
			case ATTRIBUTE::PARALYSIS:
				if (pouch->GetPararysis()->make_bool)
				{
					pararysis = pouch->GetPararysis();
				}
				break;
			case ATTRIBUTE::HEAL:
				if (pouch->GetHeal()->make_bool)
				{
					heal = pouch->GetHeal();
				}
				break;
			default:
				break;
			}
			pouch->SetOnBool(false);
		}
	}

	//左スティック右入力
	if (PAD_INPUT::GetLStick().x >= 10000)
	{
		move_left = false;
		RightMove();
	}
	//左スティック左入力
	else if (PAD_INPUT::GetLStick().x <= -10000)
	{
		move_left = true;
		LeftMove();
	}
	//スティック未入力
	else
	{
		NotInputStick();
	}

	//RBボタン入力
	if (!pouch_open)
	{
		if (PAD_INPUT::OnPressed(XINPUT_BUTTON_RIGHT_SHOULDER))
		{
			if (display_attribute == 5)
			{
				if (pouch->GetHeal()->make_bool)
				{
					if (heal != nullptr)
					{
						if (heal != nullptr)
						{
							if (hp < HP_MAX)
							{
								Hp_Heal(heal->damage);
							}
							if (heal->number_of_bullets <= 0)
							{
								display_attribute = 0;
								heal = nullptr;
								pouch->InitializeHeal();
							}
						}
					}
				}
			}
			else if (shoot_count++ % 10 == 0)
			{
				Shoot_Gun();
			}
			else {}
		}
	}



	//Bボタン長押し
	if (PAD_INPUT::GetOldKey(XINPUT_BUTTON_B) & PAD_INPUT::GetNowKey(XINPUT_BUTTON_B)
		&& fuel > 0)
	{
		if (PAD_INPUT::OnPressed(XINPUT_BUTTON_LEFT_SHOULDER))
		{
			if (fuel > 0)
			{
				player_state = PLAYER_STATE::FLY;
				Hovering();
			}
			else
			{
				NotFly();
			}
		}
		else
		{
			Fly();
		}
	}
	//Bボタン未入力
	else
	{
		jump_bottun_count = 0;
		if (PAD_INPUT::OnPressed(XINPUT_BUTTON_LEFT_SHOULDER))
		{
			if (fuel > 0)
			{
				player_state = PLAYER_STATE::FLY;
				Hovering();
			}
			else
			{
				NotFly();
			}
		}
		else
		{
			NotFly();
		}
	}


	//Bボタン解放時
	if (PAD_INPUT::OnRelease(XINPUT_BUTTON_B))
	{
		player_state = PLAYER_STATE::DOWN;
	}

	//弾のアップデート呼び出し
	for (int i = 0; i < bullet_count; i++)
	{
		if (bullet[i] != nullptr)
		{
			if (bullet[i]->GetEfectFlg())
			{
				delete bullet[i];
				bullet[i] = nullptr;
				SortBullet(i);
			}
			else
			{
				bullet[i]->Update(stage);
			}
		}
	}

	//弾の属性の切り替え処理
	if (!pouch_open)
	{
		ElementUpdate();
	}

	int x = location.x - CameraWork::GetCamera().x;
	int y = location.y - CameraWork::GetCamera().y;

	if (y > 740)
	{
		player_state = PLAYER_STATE::DEATH;
	}

	if (hp <= 0)
	{
		player_state = PLAYER_STATE::DEATH;
	}

	//プレイヤーが画面外へ出たら移動前に戻す
	if (x - area.width / 2 < 0 || SCREEN_WIDTH < x + area.width / 2)
	{
		location.x = old_x;
	}

	if (y - area.height / 2 < 0)
	{
		location.y = old_y;
	}
}

//スティックを入力していないとき
void Player::NotInputStick()
{
	image_count = 0;
	if (speed_x > 0)
	{
		if (player_state == PLAYER_STATE::JUMP || player_state == PLAYER_STATE::DOWN)
		{
			speed_x -= JUMP_INERTIA;
		}
		else
		{
			speed_x -= WARK_INERTIA;
		}

		if (speed_x < 0)
		{
			player_state = PLAYER_STATE::STOP;
			speed_x = 0;
		}
	}

	if (speed_x < 0)
	{
		if (player_state == PLAYER_STATE::JUMP || player_state == PLAYER_STATE::DOWN)
		{
			speed_x += JUMP_INERTIA;
		}
		else
		{
			speed_x += WARK_INERTIA;
		}

		if (speed_x > 0)
		{
			speed_x = 0;
		}
	}

	if (speed_x > -JUMP_INERTIA && speed_x < JUMP_INERTIA)
	{
		player_state = PLAYER_STATE::STOP;
		speed_x = 0;
	}

	if (!HitBlock(stage))
	{
		location.x += speed_x;
	}
	if (HitBlock(stage))
	{
		speed_x = 0.0;
		location.x = old_x;
	}
}

//左移動
void Player::LeftMove()
{

	if (player_state == PLAYER_STATE::JUMP || player_state == PLAYER_STATE::DOWN)
	{
		if (speed_x > -PLAYER_SPEED_X)
		{
			speed_x = speed_x - JUMP_INERTIA;
		}
		else
		{
			speed_x = -PLAYER_SPEED_X;
		}
	}
	else
	{

		MoveAnimation();
		player_state = PLAYER_STATE::MOVE_LEFT;
		if (speed_x > -PLAYER_SPEED_X)
		{
			speed_x = speed_x - WARK_INERTIA;
		}
		else
		{
			speed_x = -PLAYER_SPEED_X;
		}
	}

	if (!HitBlock(stage))
	{
		location.x += speed_x;
	}

	if (location.x < 0)
	{
		speed_x = 0.0;
	}

	if (HitBlock(stage))
	{
		if (hit_line)
		{
			float radian = hit_line->GetRadian();
			float move_x = cosf(radian) * speed_x;
			float move_y = sinf(radian) * speed_x;

			location.x = old_x + move_x;
			location.y = old_y + move_y;
			old_y = location.y;
		}
		else
		{
			location.x = old_x;
			speed_x = 0.0;
		}
	}
}

//右移動
void Player::RightMove()
{
	if (player_state == PLAYER_STATE::JUMP || player_state == PLAYER_STATE::DOWN)
	{
		if (speed_x < PLAYER_SPEED_X)
		{
			speed_x = speed_x + JUMP_INERTIA;
		}
		else
		{
			speed_x = PLAYER_SPEED_X;
		}
	}
	else
	{
		MoveAnimation();
		player_state = PLAYER_STATE::MOVE_RIGHT;
		if (speed_x < PLAYER_SPEED_X)
		{
			speed_x = speed_x + WARK_INERTIA;
		}
		else
		{
			speed_x = PLAYER_SPEED_X;
		}
	}

	if (!HitBlock(stage))
	{
		location.x += speed_x;
	}

	if (HitBlock(stage))
	{
		if (hit_line)
		{
			float radian = hit_line->GetRadian();
			float move_x = cosf(radian) * speed_x;
			float move_y = sinf(radian) * speed_x;

			location.x = old_x + move_x;
			location.y = old_y + move_y;
			old_y = location.y;
		}
		else
		{
			location.x = old_x;
			speed_x = 0.0;
		}
	}

	if (location.x < 0)
	{
		speed_x = 0.0;
	}
}


//ジャンプ
void Player::Jump()
{
	float jump;
	float y = location.y - CameraWork::GetCamera().y;
	jump = jump_power - GRAVITY;
	if (jump > 0)
	{
		jump_power -= 0.25;
	}
	else
	{
		jump_power = 20.0f;
	}

	if (jump < 0)
	{
		player_state = PLAYER_STATE::DOWN;
	}

	if (!HitBlock(stage) || y > 0)
	{
		location.y -= jump;
	}

	if (HitBlock(stage))
	{
		jump_power = 0.0;
		location.y = old_y;
	}
}


void Player::Hovering()
{
	MoveAnimation();
	if (fly > 0)
	{
		if (!HitBlock(stage))
		{
			fly -= 0.5;
			location.y -= fly;


			if (location.y < 40.0)
			{
				location.y = 40.0;
			}
		}

		if (HitBlock(stage))
		{
			fly = 0.0;
			location.y = old_y;
		}
	}
	else if (fly < 0)
	{
		if (!HitBlock(stage))
		{
			fly += 0.5;
			location.y -= fly;
			if (location.y < 40.0)
			{
				location.y = 40.0;
				fly = 0;
			}
		}
		if (HitBlock(stage))
		{
			fly = 0.0;
			location.y = old_y;
		}
	}
	else
	{
		fly = 0;
	}


	if (fuel < 0)
	{
		fuel = 0;
	}
	else
	{
		fuel -= 0.5;
	}
}

//飛ぶ
void Player::Fly()
{

	float y = location.y - CameraWork::GetCamera().y;
	image_count = 0;
	player_state = PLAYER_STATE::FLY;
	not_jet_count = 0;
	MoveAnimation();

	gravity_down = 0.0;

	fly += 0.5;
	fuel -= 0.25;

	if (fly > 10)
	{
		fly = 10.0;
	}

	if (fuel < 0)
	{
		fuel = 0;
		player_state = PLAYER_STATE::DOWN;
	}

	if (!HitBlock(stage))
	{

		location.y -= fly;


		if (location.y < 40.0)
		{
			location.y = 40.0;
		}
	}

	if (HitBlock(stage))
	{
		fly = 0.0;
		location.y = old_y;
	}
}



//飛んでない
void Player::NotFly()
{
	float y = location.y - CameraWork::GetCamera().y;

	player_state = PLAYER_STATE::DOWN;

	fly -= 0.5;

	if (fly < -10)
	{
		fly = -10;
	}

	if (not_jet_count++ >= 120)
	{
		if (fuel < 100)
		{
			fuel += 2.5;
		}
		else
		{
			fuel = 100;
		}
	}

	if (not_jet_count >= 120)
	{
		not_jet_count = 120;
	}

	if (!HitBlock(stage))
	{
		location.y -= fly;
		if (location.y < 40.0)
		{
			location.y = 40.0;
			fly = 0;
		}
	}



	if (HitBlock(stage))
	{
		fly = 0;
		location.y = old_y;
		player_state = PLAYER_STATE::STOP;
	}
}



//-----------------------------------
// 弾を発射
//-----------------------------------
void Player::Shoot_Gun()
{
	if (bullet[bullet_count] == nullptr)
	{
		switch (display_attribute)
		{
		case 0:
			bullet[bullet_count] = new NormalBullet(location.x, location.y, move_left, &normal);
			bullet_count++;
			break;
		case 1:
			if (pouch->GetExplosion()->make_bool)
			{
				if (explosion != nullptr)
				{
					if (explosion->number_of_bullets > 0)
					{
						bullet[bullet_count] = new NormalBullet(location.x, location.y, move_left, explosion);
						bullet_count++;
						pouch->ReduceAmmo(attribute[display_attribute]);
					}
					if (explosion->number_of_bullets <= 0)
					{
						display_attribute = 0;
						explosion = nullptr;
						pouch->InitializeExplosion();
					}
				}
			}
			break;
		case 2:
			if (pouch->GetMelt()->make_bool)
			{
				if (melt != nullptr)
				{
					if (melt->number_of_bullets > 0)
					{
						bullet[bullet_count] = new NormalBullet(location.x, location.y, move_left, melt);
						bullet_count++;
						pouch->ReduceAmmo(attribute[display_attribute]);
					}
					if (melt->number_of_bullets <= 0)
					{
						display_attribute = 0;
						melt = nullptr;
						pouch->InitializeMelt();
					}
				}
			}
			break;
		case 3:
			if (pouch->GetPoison()->make_bool)
			{
				if (poison != nullptr)
				{
					if (poison->number_of_bullets > 0)
					{
						bullet[bullet_count] = new NormalBullet(location.x, location.y, move_left, poison);
						bullet_count++;
						pouch->ReduceAmmo(attribute[display_attribute]);
					}
					if (poison->number_of_bullets <= 0)
					{
						display_attribute = 0;
						poison = nullptr;
						pouch->InitializePoison();
					}
				}
			}
			break;
		case 4:
			if (pouch->GetPararysis()->make_bool)
			{
				if (pararysis != nullptr)
				{
					if (pararysis->number_of_bullets > 0)
					{
						bullet[bullet_count] = new NormalBullet(location.x, location.y, move_left, pararysis);
						bullet_count++;
						pouch->ReduceAmmo(attribute[display_attribute]);
					}
					if (pararysis->number_of_bullets <= 0)
					{
						display_attribute = 0;
						pararysis = nullptr;
						pouch->InitializePararysis();
					}
				}
			}
			break;
		case 5:

			break;
		default:
			break;
		}
	}
}



//-----------------------------------
// 弾を並べ替え
//-----------------------------------
void Player::SortBullet(int delete_bullet)
{
	for (int i = delete_bullet + 1; i < BULLET_MAX; i++)
	{
		if ((bullet[i] == nullptr))
		{
			break;
		}

		bullet[i - 1] = bullet[i];
		bullet[i] = nullptr;
	}
	bullet_count--;
}



//-----------------------------------
//属性を変更
//-----------------------------------
void Player::ElementUpdate()
{
	bool chemical_formula[6];
	for (int i = 0; i < 6; i++)
	{
		chemical_formula[i] = false;
	}
	chemical_formula[0] = normal.make_bool;
	if (explosion != nullptr)
	{
		chemical_formula[1] = explosion->make_bool;
	}
	if (melt != nullptr)
	{
		chemical_formula[2] = melt->make_bool;
	}
	if (poison != nullptr)
	{
		chemical_formula[3] = poison->make_bool;
	}
	if (pararysis != nullptr)
	{
		chemical_formula[4] = pararysis->make_bool;
	}
	if (heal != nullptr)
	{
		chemical_formula[5] = heal->make_bool;
	}

	if (PAD_INPUT::GetRStick().x > 5000)
	{
		if (select_count % 10 == 0)
		{
			int a = display_attribute - 1;
			if (a < 0)
			{
				a = 5;
			}
			while (!chemical_formula[a])
			{
				display_attribute--;
				if (display_attribute < 0)
				{
					display_attribute = 5;
				}
				a--;
				if (a < 0)
				{
					a = 5;
				}
			}
			if (chemical_formula[a])
			{
				display_attribute--;
				if (display_attribute < 0)
				{
					display_attribute = 5;
				}
			}
		}
	}

	if (PAD_INPUT::GetRStick().x < -5000)
	{
		if (select_count % 10 == 0)
		{
			int a = display_attribute + 1;
			if (a > 5)
			{
				a = 0;
			}
			while (!chemical_formula[a])
			{
				display_attribute++;
				if (display_attribute > 5)
				{
					display_attribute = 0;
				}
				a++;
				if (a > 5)
				{
					a = 0;
				}
			}
			if (chemical_formula[a])
			{
				display_attribute++;
				if (display_attribute > 5)
				{
					display_attribute = 0;
				}
			}
		}
	}

	select_count++;
}



//-----------------------------------
//ダメージを受けた時
//-----------------------------------
void Player::HpDamage(AttackResource attack)
{
	if (!damage_flg)
	{
		if (attack.damage > 0)
		{
			damage_flg = true;
			damage = attack.damage;

			if (attack.type != nullptr)
			{
				for (int i = 0; i < attack.type_count; i++)
				{
					switch (attack.type[i])
					{
					case ENEMY_TYPE::NORMAL:
						break;
					case ENEMY_TYPE::FIRE:
						if (damage_time <= 0)
						{
							damage_by_second = 2;
							damage_time = 240;
							fire_flg = true;
						}
						break;
					case ENEMY_TYPE::WATER:
						break;
					case ENEMY_TYPE::WIND:
						break;
					case ENEMY_TYPE::SOIL:
						break;
					case ENEMY_TYPE::THUNDER:
						break;
					default:
						break;
					}
				}
			}
		}
	}
}



////敵からダメージを受けた時
//void Player::Being_Attacked(EnemyBase* enemy_base)
//{}



//回復
//-----------------------------------
void Player::Hp_Heal(int heal_value)
{
	pouch->ReduceAmmo(attribute[display_attribute]);
	hp += heal_value;
	if (hp >= HP_MAX)
	{
		hp = HP_MAX;
	}
}

//-----------------------------------wer
//元素の量の設定
//-----------------------------------
void Player::SetElementItem(class Item* item)
{

	int num = static_cast<int>(item->GetElementType());

	element[num]->SetVolume(element[num]->GetVolume() + 1);

}

bool Player::HitBlock(const Stage* stage_pointa)
{

	//マップチップ
	std::vector<MapChip*>map_chip;// = stage_pointa->GetMapChip();

	//描画範囲
	Location camera = CameraWork::GetCamera();

	for (MapChip* map_chip : map_chip)
	{
		if (map_chip != nullptr)
		{

			Location draw_location = map_chip->GetLocation();
			Area draw = { SCREEN_HEIGHT + CHIP_SIZE,SCREEN_WIDTH + CHIP_SIZE };

			// 画面内にあるMapChipオブジェクトだけUpdateする
			if ((camera.x < draw_location.x + draw.width) && (draw_location.x < camera.x + draw.width)
				&& (camera.y < draw_location.y + draw.height) && (draw_location.y < camera.y + draw.height))
			{
				if (HitBox(map_chip))
				{
					return true;
				}
			}
		}
	}


	for (ObjectBase* object : objects)
	{
		if (object != nullptr)
		{

			Location draw_location = object->GetColllider()->GetLocation();
			Area draw = { SCREEN_HEIGHT,SCREEN_WIDTH };

			// 画面内にあるオブジェクトだけUpdateする
			if ((camera.x < draw_location.x + draw.width) && (draw_location.x < camera.x + draw.width)
				&& (camera.y < draw_location.y + draw.height) && (draw_location.y < camera.y + draw.height))
			{
				if (HitCheck(object->GetColllider()))
				{
					hit_line = dynamic_cast<PolyLine*>(object->GetColllider());
					return true;
				}
				else
				{
					hit_line = nullptr;
				}
			}
		}
	}

	return false;
}

bool Player::GetMoveDirection()
{

	return move_left;
}


void Player::SetExplosion(ChemicalFormulaParameter* a)
{
	explosion = a;
}

void Player::SetPoison(ChemicalFormulaParameter* a)
{

	poison = a;
}

void Player::SetMelt(ChemicalFormulaParameter* a)
{

	melt = a;
}

void Player::SetPararysis(ChemicalFormulaParameter* a)
{
	pararysis = a;
}

void Player::SetHeal(ChemicalFormulaParameter* a)
{

	heal = a;
}

void Player::MoveAnimation()
{
	int image_chenge = 0;

	if (player_state == PLAYER_STATE::FLY)
	{
		image_chenge = 3;
	}
	else
	{
		image_chenge = PLAYER_IMAGES;
	}

	animation++;
	if (animation % ANIMATION_MOVE == 0)
	{
		image_count++;
	}

	if (image_count >= image_chenge)
	{
		image_count = 1;
	}
}

#ifdef _DEBUG
//-----------------------------------
// 更新(DotByDot)
//-----------------------------------
void Player::Update(const PLAYER_DEBUG_STATE state)
{
	switch (state)
	{
	case PLAYER_DEBUG_STATE::STOP:
		break;
	case PLAYER_DEBUG_STATE::MOVE:
		if (player_state != PLAYER_STATE::MOVE_LEFT)
		{
			player_state = PLAYER_STATE::MOVE_LEFT;
		}
		MoveAnimation();
		break;
	case PLAYER_DEBUG_STATE::FLY:
		if (player_state != PLAYER_STATE::FLY)
		{
			player_state = PLAYER_STATE::FLY;
		}
		MoveAnimation();
		break;
	case PLAYER_DEBUG_STATE::DOWN:
		break;
	case PLAYER_DEBUG_STATE::DEATH:
		break;
	default:
		break;
	}
}
//-----------------------------------
//描画(DotByDot)
//-----------------------------------
void Player::DebugDraw()
{
	if (player_state == PLAYER_STATE::FLY)
	{
		DrawRotaGraphF(location.x, location.y, PLAYER_SIZE, 0, jump_image[image_count], TRUE, TRUE);

	}
	else
	{
		DrawRotaGraphF(location.x, location.y, PLAYER_SIZE, 0, image[image_count], TRUE, TRUE);
	}

	DrawBox(location.x - area.width / 2, location.y - area.height / 2,
		location.x + area.width / 2, location.y + area.height / 2,
		0xff0000, FALSE);
}
#endif