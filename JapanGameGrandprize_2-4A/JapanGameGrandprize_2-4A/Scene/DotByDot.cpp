#include "DotByDot.h"
#include "../PadInput.h"
#include "GameMain.h"
#include "../Undead.h"
#include"../EnemySlime.h"
#include"../EnemyGhost.h"
#include"../Harpy.h"
#include "../Mage.h"
#include "../Torrent.h"
#include "../EnemySlimeBoss.h"


#define ENEMY_NUM 4



//-----------------------------------
//コンストラクタ
//-----------------------------------
DotByDot::DotByDot()
{
	state = 0;
	enemy = new EnemyBase * [ENEMY_NUM];
	Location spawn_location;
	spawn_location.y = 100;
	spawn_location.x = 280;
	enemy[0] = new Undead(spawn_location);
	spawn_location.y = 78;
	spawn_location.x = 450;
	enemy[1] = new EnemyGhost(spawn_location);
	spawn_location.y = 93;
	spawn_location.x = 700;
	enemy[2] = new EnemySlime(spawn_location);
	spawn_location.y = 80;
	spawn_location.x = 900;
	enemy[3] = new Harpy(spawn_location);

	player = new Player();
	element = new Stage_Element();

	element->InitDemo({100.0f,400.0f});


	font = CreateFontToHandle("DotByDotFont", 32, 1, DX_FONTTYPE_NORMAL);
}

//-----------------------------------
//デストラクタ
//-----------------------------------
DotByDot::~DotByDot()
{

	for (int i = 0; i < 1; i++)
	{
		delete enemy[i];
	}
	delete[] enemy;

	delete player;

	delete element;

	DeleteFontToHandle(font);
}

//-----------------------------------
// 更新
//-----------------------------------
AbstractScene* DotByDot::Update()
{
	if (PAD_INPUT::OnButton(XINPUT_BUTTON_DPAD_LEFT))
	{
		return new GameMain(1);
	}

	if (PAD_INPUT::OnButton(XINPUT_BUTTON_A))
	{
		state++;
	}

#ifdef _DEBUG
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		enemy[i]->Update(static_cast<ENEMY_STATE>(state % 5));
	}

	player->Update(static_cast<PLAYER_DEBUG_STATE>(state % 5));
#endif // _DEBUG

	element->Update(player);

	return this;

	
}

//-----------------------------------
// 描画
//-----------------------------------
void DotByDot::Draw() const
{
	SetBackgroundColor(0,255,255);
	DrawLine(0, 100, SCREEN_WIDTH, 100, 0x000000, true);
#ifdef _DEBUG
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		enemy[i]->DebugDraw();
	}

	player->DebugDraw();
#endif // _DEBUG

	element->Draw();

	DrawFormatStringToHandle(1060, 610, 0x000000, font, "Player:%s", player_str[state % 5]);

	DrawFormatStringToHandle(1060, 660, 0x000000, font, " Enemy:%s", enemy_str[state % 5]);
}